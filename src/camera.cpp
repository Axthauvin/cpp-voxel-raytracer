#include "camera.hh"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <math.h>

#include "color.hh"
#include "light.hh"
#include "objects/object.hh"
#include "utils/image.hh"

#include "utils/loading.hh"

namespace isim
{

  bool is_refractive_material(const Texture_properties& properties,
                              const Object* object)
  {
    return properties.ior > 1.0 || object->material->properties.ior > 1.0;
  }

  bool should_cull_transparent_backface(const Ray& ray,
                                        const Object* object,
                                        const Point3& hit_point,
                                        const Texture_properties& properties)
  {
    if (!is_refractive_material(properties, object))
      return false;

    const Vector3 normal = object->getNormalAt(hit_point);
    return ray.direction.dotProduct(normal) > 0.0f;
  }

  Color Camera::trace_ray(Ray ray,
                          Color backgroundColor,
                          Color scene_ambient,
                          const std::vector<const Object*>& objects,
                          const std::vector<const Light*>& lights,
                          double current_ior,
                          size_t depth,
                          bool calculate_alpha_shadows = true) const
  {
    if (depth == 0)
      return backgroundColor;

    float closest_distance = std::numeric_limits<float>::max();
    const Object* closest_object = nullptr;

    for (const auto* ob : objects)
      {
        float is_colliding = ob->intersect(ray);

        if (is_colliding < closest_distance && is_colliding > 0)
          {
            Point3 hit_point(ray.origin.x + ray.direction.x * is_colliding,
                             ray.origin.y + ray.direction.y * is_colliding,
                             ray.origin.z + ray.direction.z * is_colliding);

            Texture_properties hit_props =
              ob->getTextureCoordinatesAt(hit_point);

            if (should_cull_transparent_backface(ray, ob, hit_point, hit_props))
              {
                continue;
              }

            closest_distance = is_colliding;
            closest_object = ob;
          }
      }

    if (closest_object)
      {
        Point3 collision_point =
          Point3(ray.origin.x + ray.direction.x * closest_distance,
                 ray.origin.y + ray.direction.y * closest_distance,
                 ray.origin.z + ray.direction.z * closest_distance);

        Texture_properties properties =
          closest_object->getTextureCoordinatesAt(collision_point);

        Vector3 geometric_normal = closest_object->getNormalAt(collision_point);
        const bool is_entering =
          ray.direction.dotProduct(geometric_normal) < 0.0f;

        // Shading normal always faces the incident ray.
        Vector3 normal_at_collision =
          is_entering ? geometric_normal : -geometric_normal;

        Point3 corrected_collision_point =
          Point3(collision_point.x + normal_at_collision.x * 0.001f,
                 collision_point.y + normal_at_collision.y * 0.001f,
                 collision_point.z + normal_at_collision.z * 0.001f);

        // check is the propertis is transparent, if so we need to keep
        // tracing the ray until we find a non transparent pixel or reach
        // the max depth
        const double object_ior = closest_object->material->properties.ior;

        // Only treat fully transparent texels as cutout when material is
        // not refractive. Refractive materials (water/glass) must still
        // be shaded and refracted.
        if (properties.ambient_color.a == 0 && object_ior <= 1.0)
          {
            if (depth == 0)
              return backgroundColor;

            Point3 forward_collision_point(
              collision_point.x + ray.direction.x * 0.001f,
              collision_point.y + ray.direction.y * 0.001f,
              collision_point.z + ray.direction.z * 0.001f);

            Ray new_ray(forward_collision_point, ray.direction);
            return trace_ray(new_ray, backgroundColor, scene_ambient, objects,
                             lights, current_ior, depth - 1);
          }

        // compute light
        int final_r =
          clamp_color(properties.ambient_color.r * DEFAULT_AMBIENT_LIGHT.color.r
                      * DEFAULT_AMBIENT_LIGHT.intensity / 255.0);
        int final_g =
          clamp_color(properties.ambient_color.g * DEFAULT_AMBIENT_LIGHT.color.g
                      * DEFAULT_AMBIENT_LIGHT.intensity / 255.0);
        int final_b =
          clamp_color(properties.ambient_color.b * DEFAULT_AMBIENT_LIGHT.color.b
                      * DEFAULT_AMBIENT_LIGHT.intensity / 255.0);

        for (const auto* light : lights)
          {
            Vector3 light_dir = Vector3(light->position.x - collision_point.x,
                                        light->position.y - collision_point.y,
                                        light->position.z - collision_point.z)
                                  .normalize();

            const float distance_to_light = std::sqrt(
              std::pow(light->position.x - collision_point.x, 2.0f)
              + std::pow(light->position.y - collision_point.y, 2.0f)
              + std::pow(light->position.z - collision_point.z, 2.0f));

            float intensity = std::max(
              0.0f,
              static_cast<float>(normal_at_collision.dotProduct(light_dir)));

            Vector3 view_dir(-ray.direction.x, -ray.direction.y,
                             -ray.direction.z);

            float n_dot_v =
              std::max(normal_at_collision.dotProduct(view_dir), 0.0f);

            Vector3 S(2.0f * n_dot_v * normal_at_collision.x - view_dir.x,
                      2.0f * n_dot_v * normal_at_collision.y - view_dir.y,
                      2.0f * n_dot_v * normal_at_collision.z - view_dir.z);

            float ns = properties.ns > 0.0f ? properties.ns : 30.0f;

            float specular_intensity =
              pow(std::max(S.normalize().dotProduct(light_dir), 0.0f), ns);

            // Point3 corrected_collision_point =
            //     Point3(collision_point.x + normal_at_collision.x *
            //     0.001f,
            //            collision_point.y + normal_at_collision.y *
            //            0.001f, collision_point.z + normal_at_collision.z
            //            * 0.001f);

            bool in_shadow = false;

            // shadow computation

            if (calculate_alpha_shadows)
              {
                Point3 current_shadow_origin = corrected_collision_point;

                int max_transparent_bounces = 5;
                for (int b = 0; b < max_transparent_bounces; ++b)
                  {
                    Ray shadow_ray(current_shadow_origin, light_dir);

                    float closest_shadow_t = std::numeric_limits<float>::max();
                    const Object* shadow_ob = nullptr;

                    for (const auto* ob : objects)
                      {
                        float t = ob->intersect(shadow_ray);
                        if (t > 0 && t < closest_shadow_t
                            && t < distance_to_light)
                          {
                            Point3 shadow_hit_candidate = Point3(
                              shadow_ray.origin.x + shadow_ray.direction.x * t,
                              shadow_ray.origin.y + shadow_ray.direction.y * t,
                              shadow_ray.origin.z + shadow_ray.direction.z * t);

                            Texture_properties shadow_candidate_props =
                              ob->getTextureCoordinatesAt(shadow_hit_candidate);

                            if (should_cull_transparent_backface(
                                  shadow_ray, ob, shadow_hit_candidate,
                                  shadow_candidate_props))
                              {
                                continue;
                              }

                            closest_shadow_t = t;
                            shadow_ob = ob;
                          }
                      }

                    if (shadow_ob)
                      {
                        Point3 shadow_hit =
                          Point3(shadow_ray.origin.x
                                   + shadow_ray.direction.x * closest_shadow_t,
                                 shadow_ray.origin.y
                                   + shadow_ray.direction.y * closest_shadow_t,
                                 shadow_ray.origin.z
                                   + shadow_ray.direction.z * closest_shadow_t);

                        Texture_properties shadow_props =
                          shadow_ob->getTextureCoordinatesAt(shadow_hit);

                        if (shadow_props.ambient_color.a < 255)
                          {
                            current_shadow_origin =
                              Point3(shadow_hit.x + light_dir.x * 0.001f,
                                     shadow_hit.y + light_dir.y * 0.001f,
                                     shadow_hit.z + light_dir.z * 0.001f);
                            continue;
                          }
                        else
                          {
                            in_shadow = true;
                            break;
                          }
                      }
                    else
                      break;
                  }
              }
            else
              {
                Ray shadow_ray(corrected_collision_point, light_dir);
                bool in_shadow = false;
                for (const auto* ob : objects)
                  {
                    float t = ob->intersect(shadow_ray);
                    if (t > 0 && t < distance_to_light)
                      {
                        Point3 shadow_hit = Point3(
                          shadow_ray.origin.x + shadow_ray.direction.x * t,
                          shadow_ray.origin.y + shadow_ray.direction.y * t,
                          shadow_ray.origin.z + shadow_ray.direction.z * t);

                        Texture_properties shadow_props =
                          ob->getTextureCoordinatesAt(shadow_hit);

                        if (shadow_props.ambient_color.a < 255
                            || should_cull_transparent_backface(
                              shadow_ray, ob, shadow_hit, shadow_props))
                          {
                            continue;
                          }

                        in_shadow = true;
                        break;
                      }
                  }

                if (in_shadow)
                  continue;
              }

            if (in_shadow)
              continue;

            // compute diffuse + specular
            Color light_color =
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

        const bool is_refractive_material =
          (closest_object->material->properties.ior > 1.0f
           || properties.ambient_color.a < 255);

        Color reflected_color = pixel_color;
        bool has_reflected_color = false;

        if (closest_object->material->properties.ks > 0.0
            || is_refractive_material)
          {
            Vector3 reflect_dir = ray.direction
              - normal_at_collision * 2.0f
                * ray.direction.dotProduct(normal_at_collision);

            reflect_dir = reflect_dir.normalize();

            Ray reflected_ray(corrected_collision_point, reflect_dir);
            reflected_color =
              trace_ray(reflected_ray, backgroundColor, scene_ambient, objects,
                        lights, current_ior, depth - 1);
            has_reflected_color = true;

            const float ks = std::clamp(
              static_cast<float>(closest_object->material->properties.ks), 0.0f,
              1.0f);

            // For non-refractive materials, keep the 'normal' way to
            // compute ks reflection. Refractive materials are
            // combined later with Fresnel-Schlick.
            if (!is_refractive_material)
              {
                pixel_color.r = clamp_color(pixel_color.r * (1.0f - ks)
                                            + reflected_color.r * ks);
                pixel_color.g = clamp_color(pixel_color.g * (1.0f - ks)
                                            + reflected_color.g * ks);
                pixel_color.b = clamp_color(pixel_color.b * (1.0f - ks)
                                            + reflected_color.b * ks);
              }

            // pixel_color.r = clamp_color(
            //     pixel_color.r
            //     + reflected_color.r
            //         * closest_object->material->properties.ks);
            // pixel_color.g = clamp_color(
            //     pixel_color.g
            //     + reflected_color.g
            //         * closest_object->material->properties.ks);
            // pixel_color.b = clamp_color(
            //     pixel_color.b
            //     + reflected_color.b
            //         * closest_object->material->properties.ks);
          }

        // refraction + Fresnel-Schlick for transparent/refractive
        // materials (water for us, but i understood it can work with glass
        // too).
        if (is_refractive_material)
          {
            // https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel.html
            // the algo is very complicated, i did not understand it at all,
            // but it seems to work well for water, so i guess it's fine ??
            // hopefully, scratchapixel is a reliable source, and the code
            // they give seems to work.
            const float eta_i =
              current_ior > 0.0 ? static_cast<float>(current_ior) : 1.0f;
            const float eta_t =
              is_entering ? static_cast<float>(object_ior) : 1.0f;

            const float eta = eta_i / eta_t;
            const float cos_theta_i =
              std::clamp(static_cast<float>(
                           -ray.direction.dotProduct(normal_at_collision)),
                         0.0f, 1.0f);
            // const float k =
            //   1.0f - eta * eta * (1.0f - cos_theta_i * cos_theta_i);

            // Vector3 refract_dir(-1.0f, -1.0f, -1.0f, true);
            // if (k >= 0.0f)
            //   {
            //     refract_dir =
            //       (ray.direction * eta
            //        + normal_at_collision * (eta * cos_theta_i - std::sqrt(k)));
            //   }

            // in minecraft water is not really refractive, it's more like a transparent material
            Vector3 refract_dir = ray.direction;

            Color refracted_color = pixel_color;
            bool has_refracted_color = false;

            if (refract_dir.x != -1 || refract_dir.y != -1
                || refract_dir.z != -1)
              {
                refract_dir = refract_dir.normalize();

                Point3 corrected_collision_point =
                  Point3(collision_point.x - normal_at_collision.x * 0.001f,
                         collision_point.y - normal_at_collision.y * 0.001f,
                         collision_point.z - normal_at_collision.z * 0.001f);

                Ray refracted_ray(corrected_collision_point, refract_dir);
                const float next_ior = is_entering ? eta_t : 1.0f;
                refracted_color =
                  trace_ray(refracted_ray, backgroundColor, scene_ambient,
                            objects, lights, next_ior, depth - 1, false);
                has_refracted_color = true;

                const float transparency = std::clamp(
                  static_cast<float>(255 - properties.ambient_color.a) / 255.0f,
                  0.0f, 1.0f);

                const float r0 =
                  std::pow((eta_i - eta_t) / (eta_i + eta_t), 2.0f);
                float fresnel =
                  r0 + (1.0f - r0) * std::pow(1.0f - cos_theta_i, 5.0f);
                fresnel = std::clamp(fresnel, 0.0f, 1.0f);

                float reflection_weight = has_reflected_color ? fresnel : 0.0f;
                float refraction_weight =
                  has_refracted_color ? (1.0f - fresnel) * transparency : 0.0f;
                float base_weight =
                  std::max(0.0f, 1.0f - reflection_weight - refraction_weight);

                pixel_color.r =
                  clamp_color(pixel_color.r * base_weight
                              + reflected_color.r * reflection_weight
                              + refracted_color.r * refraction_weight);
                pixel_color.g =
                  clamp_color(pixel_color.g * base_weight
                              + reflected_color.g * reflection_weight
                              + refracted_color.g * refraction_weight);
                pixel_color.b =
                  clamp_color(pixel_color.b * base_weight
                              + reflected_color.b * reflection_weight
                              + refracted_color.b * refraction_weight);

                pixel_color.a =
                  clamp_color(pixel_color.a * (1.0f - transparency)
                              + refracted_color.a * transparency);
              }
            else if (has_reflected_color)
              {
                // Total internal reflection fallback.
                pixel_color = reflected_color;
              }
          }

        return pixel_color;
      }

    return backgroundColor;
  }

  Image Camera::render(const std::vector<const Object*>& objects,
                       const std::vector<const Light*>& lights,
                       size_t res_x,
                       size_t res_y,
                       Color backgroundColor,
                       Color scene_ambient,
                       bool ray_tracing) const
  {
    double aspect_ratio =
      static_cast<double>(res_x) / static_cast<double>(res_y);

    if (fov_x_angle <= 0.0 || fov_x_angle >= 180.0 || fov_y_angle <= 0.0
        || fov_y_angle >= 180.0)
      {
        throw std::invalid_argument("FOV angles must be in (0, 180)");
      }

    const double fov_x_rad = fov_x_angle * M_PI / 180.0;
    const double fov_y_rad = fov_y_angle * M_PI / 180.0;

    double half_width = zmin * tan(fov_x_rad / 2.0);
    double half_height = zmin * tan(fov_y_rad / 2.0);

    const double fov_aspect = half_width / half_height;
    if (std::abs(fov_aspect - aspect_ratio) > 1e-6)
      {
        half_width = half_height * aspect_ratio;
      }

    Vector3 direction(lookAt.x - center.x, lookAt.y - center.y,
                      lookAt.z - center.z);

    Vector3 forward = direction.normalize();
    Vector3 right = up.crossProduct(forward).normalize();
    Vector3 true_up = forward.crossProduct(right).normalize();

    Vector3 centerVector(center.x, center.y, center.z);

    Image result_image(res_x, res_y);

    Loading loader;
    loader.show();

    size_t total_pixels = res_x * res_y;

    for (size_t y = 0; y < res_y; y++)
      {
        for (size_t x = 0; x < res_x; x++)
          {
            double px = (2 * (x + 0.5) / res_x - 1) * half_width;
            double py = (1 - 2 * (y + 0.5) / res_y) * half_height;

            Vector3 pixel_3d =
              centerVector + (forward * zmin) + (right * px) + (true_up * py);

            Vector3 ray_direction = (pixel_3d - centerVector).normalize();

            Ray ray(center, ray_direction);

            size_t MAX_REFLECTION_DEPTH = 5;
            if (!ray_tracing)
              MAX_REFLECTION_DEPTH = 1;

            Color pixel_color =
              trace_ray(ray, backgroundColor, scene_ambient, objects, lights,
                        -1, MAX_REFLECTION_DEPTH);
            pixel_color.a = 255;

            // TGA reverts the y-axis, so we need to flip it
            result_image.setPixel(x, (res_y - 1) - y, pixel_color);
            // result_image.setPixel(x, y, pixel_color);

            loader.update(static_cast<double>(y * res_x + x) / total_pixels);
          }
      }

    loader.hide();

    return result_image;
  }
} // namespace isim