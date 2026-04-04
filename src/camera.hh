#ifndef CAMERA_HH
#define CAMERA_HH

#include <cstddef>
#include <stdexcept>
#include "ray.hh"

namespace isim
{

  class Camera
  {
  public:
    Camera(Point3 center,
           Point3 lookAt,
           Vector3 up,
           double fov_x_angle,
           double fov_y_angle,
           double zmin)
      : center(center)
      , lookAt(lookAt)
      , up(up * -1.0) // Invert the up vector
      , fov_x_angle(fov_x_angle)
      , fov_y_angle(fov_y_angle)
      , zmin(zmin)
    {
      // check that up is not nane and not zero vector
      if (up.x == 0 && up.y == 0 && up.z == 0)
        {
          throw std::invalid_argument("Up vector cannot be zero");
        }
    }

    Ray generate_ray(size_t x, size_t y, size_t res_x, size_t res_y) const;

    Point3 center;
    Point3 lookAt;
    Vector3 up;

    double fov_x_angle;
    double fov_y_angle;

    double zmin;
  };
} // namespace isim
#endif /* CAMERA_HH */
