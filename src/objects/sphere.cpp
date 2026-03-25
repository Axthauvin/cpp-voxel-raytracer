#include "sphere.hh"

#include <algorithm>
#include <cmath>

namespace isim
{

    Vector3 Sphere::getNormalAt(const Point3 &point) const
    {
        return (point - center) / radius;
    }

    float Sphere::intersect(const Ray &ray) const
    {
        // Geometric ray/sphere intersection.
        float t0, t1;

        Vector3 L = center - ray.origin;
        float tca = L.dotProduct(ray.direction);
        float d2 = L.dotProduct(L) - tca * tca;
        if (d2 > radius * radius)
            return -1.0f;

        float thc = sqrt(radius * radius - d2);
        t0 = tca - thc;
        t1 = tca + thc;

        if (t0 > t1)
            std::swap(t0, t1);

        if (t0 < 0)
        {
            t0 = t1;
            if (t0 < 0)
                return t0;
        }

        return t0;
    }

    Texture_properties
    Sphere::getTextureCoordinatesAt(const Point3 &point) const
    {
        Vector3 dir = (point - this->center) / radius;

        // storing the direction before rotation
        float x = dir.x;
        float y = dir.y;
        float z = dir.z;
        float temp_x, temp_y, temp_z;

        if (rotation.x != 0.0f)
        {
            float cos_rx = cosf(rotation.x);
            float sin_rx = sinf(rotation.x);

            temp_y = y * cos_rx - z * sin_rx;
            temp_z = y * sin_rx + z * cos_rx;

            y = temp_y;
            z = temp_z;
        }

        if (rotation.y != 0.0f)
        {
            float cos_ry = cosf(rotation.y);
            float sin_ry = sinf(rotation.y);

            temp_x = x * cos_ry + z * sin_ry;
            temp_z = -x * sin_ry + z * cos_ry;

            x = temp_x;
            z = temp_z;
        }

        if (rotation.z != 0.0f)
        {
            float cos_rz = cosf(rotation.z);
            float sin_rz = sinf(rotation.z);

            temp_x = x * cos_rz - y * sin_rz;
            temp_y = x * sin_rz + y * cos_rz;

            x = temp_x;
            y = temp_y;
        }

        dir.x = x;
        dir.y = y;
        dir.z = z;

        // dir = dir / radius;

        float u = 0.5f + (atan2f(dir.z, dir.x) / (2.0f * M_PI));

        float y_clamped =
            std::max((double)-1.0f, std::min(dir.y, (double)1.0f));

        float v = 0.5f + (asinf(y_clamped) / M_PI);

        return material->getElementsAt(u, v);
    }

} // namespace isim
