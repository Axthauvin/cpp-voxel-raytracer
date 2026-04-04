#include "camera.hh"

#include <cmath>
#include <cstddef>
#include <stdexcept>

namespace isim
{
  Ray Camera::generate_ray(size_t x, size_t y, size_t res_x, size_t res_y) const
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

    const double px =
      (2.0 * (static_cast<double>(x) + 0.5) / static_cast<double>(res_x) - 1.0)
      * half_width;
    const double py =
      (1.0 - 2.0 * (static_cast<double>(y) + 0.5) / static_cast<double>(res_y))
      * half_height;

    Vector3 pixel_3d =
      centerVector + (forward * zmin) + (right * px) + (true_up * py);

    Vector3 ray_direction = (pixel_3d - centerVector).normalize();
    return Ray(center, ray_direction);
  }
} // namespace isim