#pragma once

#include <cstddef>
#include <vector>

#include "camera.hh"
#include "color.hh"
#include "light.hh"
#include "ray.hh"
#include "utils/image.hh"
#include "voxel_grid.hh"

namespace isim
{
  class Renderer
  {
  public:
    Renderer(const VoxelGrid& grid,
             const std::vector<const Light*>& lights,
             Color background_color,
             Color scene_ambient,
             bool ray_tracing = true);

    Image render(const Camera& camera, size_t res_x, size_t res_y) const;

  private:
    struct RayHit
    {
      bool hit = false;
      const Object* object = nullptr;
      Point3 point;
      Vector3 geometric_normal;
      Vector3 shading_normal;
      Texture_properties properties;
      float distance = 0.0f;
      bool is_entering = true;
    };

    Color cast_ray(const Ray& ray,
                   double current_ior,
                   size_t depth,
                   bool calculate_alpha_shadows) const;

    RayHit cast_dda(const Ray& ray,
                    float t_min,
                    float t_max,
                    bool cull_refractive_backfaces) const;

    bool is_shadowed(const Point3& origin,
                     const Vector3& light_dir,
                     float distance_to_light,
                     bool calculate_alpha_shadows) const;

    static bool is_refractive_material(const Texture_properties& properties,
                                       const Object* object);

    static int signum(double value);

    const VoxelGrid& grid_;
    const std::vector<const Light*>& lights_;
    Color background_color_;
    Color scene_ambient_;
    size_t max_reflection_depth_;
  };
} // namespace isim
