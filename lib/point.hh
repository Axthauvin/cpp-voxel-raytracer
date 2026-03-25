#pragma once

#include "vector.hh"

namespace isim
{

    class Point3
    {
    public:
        Point3() = default;
        Point3(const double &x, const double &y, const double &z)
            : x(x)
            , y(y)
            , z(z)
        {}

        Point3 operator*(const float &l) const;
        Vector3 operator-(const Point3 &v) const;
        Point3 operator+(const Vector3 &p) const;
        Point3 operator-(const Vector3 &p) const;
        Point3 operator*(const Vector3 &p) const;

        Vector3 toVect() const
        {
            return Vector3(x, y, z);
        };

    public:
        double x;
        double y;
        double z;
    };
} // namespace isim