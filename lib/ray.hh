#pragma once

// #include <ostream>

#include "point.hh"

namespace isim
{
    class Ray
    {
    public:
        Ray() = delete;
        Ray(const Point3 &origin, const Vector3 &direction)
            : origin(origin)
            , direction(direction)
        {}

    public:
        Point3 origin;
        Vector3 direction;
    };

    // Vector3 normalize(const Vector3 &v);
    // Vector3 normalize(const Point3 &v);

    // std::ostream &operator<<(std::ostream &os, const Point3 &p);
    // std::ostream &operator<<(std::ostream &os, const Vector3 &v);
    // std::ostream &operator<<(std::ostream &os, const Ray &r);
} // namespace isim