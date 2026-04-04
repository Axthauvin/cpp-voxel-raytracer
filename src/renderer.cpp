#include "renderer.hh"

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>

#include "objects/texture_material.hh"
#include "utils/loading.hh"

namespace isim
{

  constexpr float EPSILON = 0.001f;

  Vector3 dominant_axis_normal(const Vector3& direction)
  {
    const double ax = std::abs(direction.x);
    const double ay = std::abs(direction.y);
    const double az = std::abs(direction.z);

    if (ax >= ay && ax >= az)
      {
        return Vector3(direction.x >= 0.0 ? -1.0 : 1.0, 0.0, 0.0);
      }
    if (ay >= ax && ay >= az)
      {
        return Vector3(0.0, direction.y >= 0.0 ? -1.0 : 1.0, 0.0);
      }
    return Vector3(0.0, 0.0, direction.z >= 0.0 ? -1.0 : 1.0);
  }

  Renderer::Renderer(const VoxelGrid& grid,
                     const std::vector<const Light*>& lights,
                     Color background_color,
                     Color scene_ambient,
                     bool ray_tracing)
    : grid_(grid)
    , lights_(lights)
    , background_color_(background_color)
    , scene_ambient_(scene_ambient)
    , max_reflection_depth_(ray_tracing ? 5 : 1)
  {}

  Image Renderer::render(const Camera& camera, size_t res_x, size_t res_y) const
  {
    Loading loader;
    loader.show();
    Image result_image(res_x, res_y);

    const size_t total_pixels = res_x * res_y;
    std::atomic<size_t> processed_pixels{0};
    std::atomic<size_t> last_reported_pixels{0};
    constexpr size_t progress_chunk = 2048;

#pragma omp parallel for collapse(2) schedule(dynamic, 8)
    for (size_t y = 0; y < res_y; ++y)
      {
        for (size_t x = 0; x < res_x; ++x)
          {
            const Ray ray = camera.generate_ray(x, y, res_x, res_y);
            Color pixel_color =
              cast_ray(ray, -1.0, max_reflection_depth_, true);
            pixel_color.a = 255;
            result_image.setPixel(x, (res_y - 1) - y, pixel_color);

            const size_t done = processed_pixels.fetch_add(1) + 1;
            size_t previous = last_reported_pixels.load();
            if (done - previous >= progress_chunk || done == total_pixels)
              {
                if (last_reported_pixels.compare_exchange_strong(previous,
                                                                 done))
                  {
#pragma omp critical(loader_update)
                    loader.update(static_cast<double>(done)
                                  / static_cast<double>(total_pixels));
                  }
              }
          }
      }

    loader.hide();

    return result_image;
  }

  Color Renderer::cast_ray(const Ray& ray,
                           double current_ior,
                           size_t depth,
                           bool calculate_alpha_shadows) const
  {
    if (depth == 0)
      {
        return background_color_;
      }

    const RayHit hit =
      cast_dda(ray, EPSILON, std::numeric_limits<float>::max(), true);
    if (!hit.hit)
      {
        return background_color_;
      }

    const Object* closest_object = hit.object;
    const Point3 collision_point = hit.point;
    const Texture_properties properties = hit.properties;
    const Vector3 normal_at_collision = hit.shading_normal;
    const bool is_entering = hit.is_entering;

    const Point3 corrected_collision_point(
      collision_point.x + normal_at_collision.x * EPSILON,
      collision_point.y + normal_at_collision.y * EPSILON,
      collision_point.z + normal_at_collision.z * EPSILON);

    const double object_ior = closest_object->material->properties.ior;

    if (properties.ambient_color.a == 0 && object_ior <= 1.0)
      {
        const Point3 forward_collision_point(
          collision_point.x + ray.direction.x * EPSILON,
          collision_point.y + ray.direction.y * EPSILON,
          collision_point.z + ray.direction.z * EPSILON);

        return cast_ray(Ray(forward_collision_point, ray.direction),
                        current_ior, depth, calculate_alpha_shadows);
      }

    const bool is_refractive =
      (object_ior > 1.0f || properties.ambient_color.a < 255);

    if (is_refractive && is_entering
        && std::abs(current_ior - object_ior) < 0.001)
      {
        const Point3 forward_collision_point(
          collision_point.x + ray.direction.x * EPSILON,
          collision_point.y + ray.direction.y * EPSILON,
          collision_point.z + ray.direction.z * EPSILON);

        return cast_ray(Ray(forward_collision_point, ray.direction),
                        current_ior, depth, calculate_alpha_shadows);
      }

    int final_r =
      clamp_color(properties.ambient_color.r * DEFAULT_AMBIENT_LIGHT.color.r
                  * DEFAULT_AMBIENT_LIGHT.intensity / 255.0);
    int final_g =
      clamp_color(properties.ambient_color.g * DEFAULT_AMBIENT_LIGHT.color.g
                  * DEFAULT_AMBIENT_LIGHT.intensity / 255.0);
    int final_b =
      clamp_color(properties.ambient_color.b * DEFAULT_AMBIENT_LIGHT.color.b
                  * DEFAULT_AMBIENT_LIGHT.intensity / 255.0);

    for (const auto* light : lights_)
      {
        const Vector3 light_dir = light->get_direction_from(collision_point);

        const float distance_to_light =
          std::sqrt(std::pow(light->position.x - collision_point.x, 2.0f)
                    + std::pow(light->position.y - collision_point.y, 2.0f)
                    + std::pow(light->position.z - collision_point.z, 2.0f));

        const float intensity = std::max(
          0.0f, static_cast<float>(normal_at_collision.dotProduct(light_dir)));

        const Vector3 view_dir(-ray.direction.x, -ray.direction.y,
                               -ray.direction.z);
        const float n_dot_v =
          std::max(normal_at_collision.dotProduct(view_dir), 0.0f);

        const Vector3 S(2.0f * n_dot_v * normal_at_collision.x - view_dir.x,
                        2.0f * n_dot_v * normal_at_collision.y - view_dir.y,
                        2.0f * n_dot_v * normal_at_collision.z - view_dir.z);

        const float ns = properties.ns > 0.0f ? properties.ns : 30.0f;
        const float specular_intensity =
          std::pow(std::max(S.normalize().dotProduct(light_dir), 0.0f), ns);

        if (is_shadowed(corrected_collision_point, light_dir, distance_to_light,
                        calculate_alpha_shadows))
          {
            continue;
          }

        const Color light_color =
          light->renderLight(properties, intensity, specular_intensity,
                             normal_at_collision, light_dir);
        final_r += light_color.r;
        final_g += light_color.g;
        final_b += light_color.b;
      }

    Color pixel_color = {static_cast<uint8_t>(std::min(255, final_r)),
                         static_cast<uint8_t>(std::min(255, final_g)),
                         static_cast<uint8_t>(std::min(255, final_b)),
                         properties.ambient_color.a};

    const bool refractive = (closest_object->material->properties.ior > 1.0f
                             || properties.ambient_color.a < 255);

    Color reflected_color = pixel_color;
    bool has_reflected_color = false;

    if (closest_object->material->properties.ks > 0.0 || refractive)
      {
        Vector3 reflect_dir = ray.direction
          - normal_at_collision * 2.0f
            * ray.direction.dotProduct(normal_at_collision);
        reflect_dir = reflect_dir.normalize();

        reflected_color =
          cast_ray(Ray(corrected_collision_point, reflect_dir), current_ior,
                   depth - 1, calculate_alpha_shadows);
        has_reflected_color = true;

        const float ks = std::clamp(
          static_cast<float>(closest_object->material->properties.ks), 0.0f,
          1.0f);

        if (!refractive)
          {
            pixel_color.r =
              clamp_color(pixel_color.r * (1.0f - ks) + reflected_color.r * ks);
            pixel_color.g =
              clamp_color(pixel_color.g * (1.0f - ks) + reflected_color.g * ks);
            pixel_color.b =
              clamp_color(pixel_color.b * (1.0f - ks) + reflected_color.b * ks);
          }
      }

    if (refractive)
      {
        const float eta_i =
          current_ior > 0.0 ? static_cast<float>(current_ior) : 1.0f;
        const float eta_t = is_entering ? static_cast<float>(object_ior) : 1.0f;
        const float cos_theta_i = std::clamp(
          static_cast<float>(-ray.direction.dotProduct(normal_at_collision)),
          0.0f, 1.0f);

        Vector3 refract_dir = ray.direction;

        Color refracted_color = pixel_color;
        bool has_refracted_color = false;

        refract_dir = refract_dir.normalize();

        const Point3 refract_origin(
          collision_point.x - normal_at_collision.x * EPSILON,
          collision_point.y - normal_at_collision.y * EPSILON,
          collision_point.z - normal_at_collision.z * EPSILON);

        const float next_ior = is_entering ? eta_t : 1.0f;
        refracted_color =
          cast_ray(Ray(refract_origin, refract_dir), next_ior, depth - 1, true);
        has_refracted_color = true;

        const float transparency = std::clamp(
          static_cast<float>(255 - properties.ambient_color.a) / 255.0f, 0.0f,
          1.0f);

        const float r0 = std::pow((eta_i - eta_t) / (eta_i + eta_t), 2.0f);
        float fresnel = r0 + (1.0f - r0) * std::pow(1.0f - cos_theta_i, 5.0f);
        fresnel = std::clamp(fresnel, 0.0f, 1.0f);

        const float reflection_weight = has_reflected_color ? fresnel : 0.0f;
        const float refraction_weight =
          has_refracted_color ? (1.0f - fresnel) * transparency : 0.0f;
        const float base_weight =
          std::max(0.0f, 1.0f - reflection_weight - refraction_weight);

        pixel_color.r = clamp_color(pixel_color.r * base_weight
                                    + reflected_color.r * reflection_weight
                                    + refracted_color.r * refraction_weight);
        pixel_color.g = clamp_color(pixel_color.g * base_weight
                                    + reflected_color.g * reflection_weight
                                    + refracted_color.g * refraction_weight);
        pixel_color.b = clamp_color(pixel_color.b * base_weight
                                    + reflected_color.b * reflection_weight
                                    + refracted_color.b * refraction_weight);

        pixel_color.a = clamp_color(pixel_color.a * (1.0f - transparency)
                                    + refracted_color.a * transparency);
      }

    (void)scene_ambient_;
    return pixel_color;
  }

  Renderer::RayHit Renderer::cast_dda(const Ray& ray,
                                      float t_min,
                                      float t_max,
                                      bool cull_refractive_backfaces) const
  {
    RayHit miss{};
    if (grid_.empty())
      {
        return miss;
      }

    float t_enter = -std::numeric_limits<float>::infinity();
    float t_exit = std::numeric_limits<float>::infinity();
    int entry_axis = -1;

    const float bounds_min[3] = {static_cast<float>(grid_.min_x()) - 0.5f,
                                 static_cast<float>(grid_.min_y()) - 0.5f,
                                 static_cast<float>(grid_.min_z()) - 0.5f};
    const float bounds_max[3] = {static_cast<float>(grid_.max_x()) + 0.5f,
                                 static_cast<float>(grid_.max_y()) + 0.5f,
                                 static_cast<float>(grid_.max_z()) + 0.5f};

    const float origin[3] = {static_cast<float>(ray.origin.x),
                             static_cast<float>(ray.origin.y),
                             static_cast<float>(ray.origin.z)};
    const float direction[3] = {static_cast<float>(ray.direction.x),
                                static_cast<float>(ray.direction.y),
                                static_cast<float>(ray.direction.z)};

    for (int axis = 0; axis < 3; ++axis)
      {
        if (std::abs(direction[axis]) < std::numeric_limits<float>::epsilon())
          {
            if (origin[axis] < bounds_min[axis]
                || origin[axis] > bounds_max[axis])
              {
                return miss;
              }
            continue;
          }

        float t0 = (bounds_min[axis] - origin[axis]) / direction[axis];
        float t1 = (bounds_max[axis] - origin[axis]) / direction[axis];
        if (t0 > t1)
          {
            std::swap(t0, t1);
          }

        if (t0 > t_enter)
          {
            t_enter = t0;
            entry_axis = axis;
          }
        t_exit = std::min(t_exit, t1);

        if (t_exit < t_enter)
          {
            return miss;
          }
      }

    const float start_t = std::max(t_min, t_enter);
    const float end_t = std::min(t_max, t_exit);
    if (end_t < start_t)
      {
        return miss;
      }

    const Point3 start_point(ray.origin.x + ray.direction.x * start_t,
                             ray.origin.y + ray.direction.y * start_t,
                             ray.origin.z + ray.direction.z * start_t);

    const double shifted_x = start_point.x + 0.5;
    const double shifted_y = start_point.y + 0.5;
    const double shifted_z = start_point.z + 0.5;

    int cell_x = static_cast<int>(std::floor(shifted_x));
    int cell_y = static_cast<int>(std::floor(shifted_y));
    int cell_z = static_cast<int>(std::floor(shifted_z));

    const int step_x = signum(ray.direction.x);
    const int step_y = signum(ray.direction.y);
    const int step_z = signum(ray.direction.z);

    const float inv_dx = step_x == 0
      ? std::numeric_limits<float>::infinity()
      : std::abs(1.0f / static_cast<float>(ray.direction.x));
    const float inv_dy = step_y == 0
      ? std::numeric_limits<float>::infinity()
      : std::abs(1.0f / static_cast<float>(ray.direction.y));
    const float inv_dz = step_z == 0
      ? std::numeric_limits<float>::infinity()
      : std::abs(1.0f / static_cast<float>(ray.direction.z));

    const double next_boundary_x = step_x > 0 ? static_cast<double>(cell_x + 1)
                                              : static_cast<double>(cell_x);
    const double next_boundary_y = step_y > 0 ? static_cast<double>(cell_y + 1)
                                              : static_cast<double>(cell_y);
    const double next_boundary_z = step_z > 0 ? static_cast<double>(cell_z + 1)
                                              : static_cast<double>(cell_z);

    float t_max_x = step_x == 0
      ? std::numeric_limits<float>::infinity()
      : static_cast<float>((next_boundary_x - shifted_x) / ray.direction.x);
    float t_max_y = step_y == 0
      ? std::numeric_limits<float>::infinity()
      : static_cast<float>((next_boundary_y - shifted_y) / ray.direction.y);
    float t_max_z = step_z == 0
      ? std::numeric_limits<float>::infinity()
      : static_cast<float>((next_boundary_z - shifted_z) / ray.direction.z);

    if (t_max_x < 0.0f)
      {
        t_max_x = 0.0f;
      }
    if (t_max_y < 0.0f)
      {
        t_max_y = 0.0f;
      }
    if (t_max_z < 0.0f)
      {
        t_max_z = 0.0f;
      }

    float local_t = 0.0f;
    Vector3 geometric_normal = dominant_axis_normal(ray.direction);
    if (entry_axis == 0)
      {
        geometric_normal =
          Vector3(ray.direction.x >= 0.0 ? -1.0 : 1.0, 0.0, 0.0);
      }
    else if (entry_axis == 1)
      {
        geometric_normal =
          Vector3(0.0, ray.direction.y >= 0.0 ? -1.0 : 1.0, 0.0);
      }
    else if (entry_axis == 2)
      {
        geometric_normal =
          Vector3(0.0, 0.0, ray.direction.z >= 0.0 ? -1.0 : 1.0);
      }

    while (true)
      {
        const float world_t = start_t + local_t;
        if (world_t > end_t)
          {
            return miss;
          }

        const float next_cell_local_t =
          std::min(t_max_x, std::min(t_max_y, t_max_z));
        const float next_cell_world_t = start_t + next_cell_local_t;

        const Object* object = grid_.get_block_at(cell_x, cell_y, cell_z);
        if (object)
          {
            const float object_t = object->intersect(ray);

            const float tolerance = EPSILON * 2.0f;

            if (object_t > 0.0f && object_t + tolerance >= world_t
                && object_t <= next_cell_world_t + tolerance)
              {
                const Point3 hit_point(
                  ray.origin.x + ray.direction.x * object_t,
                  ray.origin.y + ray.direction.y * object_t,
                  ray.origin.z + ray.direction.z * object_t);

                const Vector3 object_normal = object->getNormalAt(hit_point);
                const bool is_entering =
                  ray.direction.dotProduct(object_normal) < 0.0f;
                const Vector3 shading_normal =
                  is_entering ? object_normal : -object_normal;

                const Point3 texture_sample_point(
                  hit_point.x - shading_normal.x * EPSILON,
                  hit_point.y - shading_normal.y * EPSILON,
                  hit_point.z - shading_normal.z * EPSILON);

                const Texture_properties properties =
                  object->getTextureCoordinatesAt(texture_sample_point);

                const bool refractive =
                  is_refractive_material(properties, object);

                if (!(cull_refractive_backfaces && refractive
                      && ray.direction.dotProduct(object_normal) > 0.0f))
                  {
                    RayHit hit;
                    hit.hit = true;
                    hit.object = object;
                    hit.point = hit_point;
                    hit.geometric_normal = object_normal;
                    hit.shading_normal = shading_normal;
                    hit.properties = properties;
                    hit.distance = object_t;
                    hit.is_entering = is_entering;
                    return hit;
                  }
              }
          }

        if (t_max_x <= t_max_y && t_max_x <= t_max_z)
          {
            local_t = t_max_x;
            t_max_x += inv_dx;
            cell_x += step_x;
            geometric_normal = Vector3(static_cast<double>(-step_x), 0.0, 0.0);
            continue;
          }
        if (t_max_y <= t_max_x && t_max_y <= t_max_z)
          {
            local_t = t_max_y;
            t_max_y += inv_dy;
            cell_y += step_y;
            geometric_normal = Vector3(0.0, static_cast<double>(-step_y), 0.0);
            continue;
          }

        local_t = t_max_z;
        t_max_z += inv_dz;
        cell_z += step_z;
        geometric_normal = Vector3(0.0, 0.0, static_cast<double>(-step_z));
      }
  }

  bool Renderer::is_shadowed(const Point3& origin,
                             const Vector3& light_dir,
                             float distance_to_light,
                             bool calculate_alpha_shadows) const
  {
    Point3 current_shadow_origin = origin;
    const int max_transparent_bounces = calculate_alpha_shadows ? 5 : 64;

    for (int bounce = 0; bounce < max_transparent_bounces; ++bounce)
      {
        const Ray shadow_ray(current_shadow_origin, light_dir);
        const RayHit shadow_hit =
          cast_dda(shadow_ray, EPSILON, distance_to_light, true);

        if (!shadow_hit.hit)
          {
            return false;
          }

        if (shadow_hit.properties.ambient_color.a < 255)
          {
            current_shadow_origin =
              Point3(shadow_hit.point.x + light_dir.x * EPSILON,
                     shadow_hit.point.y + light_dir.y * EPSILON,
                     shadow_hit.point.z + light_dir.z * EPSILON);
            continue;
          }

        return true;
      }

    return false;
  }

  bool Renderer::is_refractive_material(const Texture_properties& properties,
                                        const Object* object)
  {
    return properties.ior > 1.0 || object->material->properties.ior > 1.0;
  }

  int Renderer::signum(double value)
  {
    if (value > 0.0)
      {
        return 1;
      }
    if (value < 0.0)
      {
        return -1;
      }
    return 0;
  }
} // namespace isim
