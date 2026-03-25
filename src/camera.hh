#ifndef CAMERA_HH
#define CAMERA_HH

#include <stdexcept>
#include <vector>

#include "light.hh"
#include "objects/object.hh"
#include "ray.hh"
#include "utils/image.hh"

namespace isim
{

    class Camera
    {
    public:
        Camera(Point3 center, Point3 lookAt, Vector3 up, double fov_x_angle,
               double fov_y_angle, double zmin)
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

        Image render(const std::vector<const Object *> &spheres,
                     const std::vector<const Light *> &lights, size_t res_x,
                     size_t res_y, Color backgroundColor, Color scene_ambient,
                     bool ray_tracing = true) const;

        Color trace_ray(Ray ray, Color backgroundColor, Color scene_ambient,
                        const std::vector<const Object *> &objects,
                        const std::vector<const Light *> &lights,
                        double current_ior, size_t depth,
                        bool calculate_alpha_shadow) const;

        Point3 center;
        Point3 lookAt;
        Vector3 up;

        double fov_x_angle;
        double fov_y_angle;

        double zmin;
    };
} // namespace isim
#endif /* CAMERA_HH */
