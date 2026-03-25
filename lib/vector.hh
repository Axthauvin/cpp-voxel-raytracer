#pragma once

namespace isim
{

    class Vector3
    {
    public:
        Vector3() = default;
        Vector3(const double &x, const double &y, const double &z,
                bool error = false)
            : x(x)
            , y(y)
            , z(z)
            , error(error)

        {}
        Vector3 operator*(const float &l) const;
        Vector3 operator*(const double &l) const;
        Vector3 operator*(const int &l) const;
        Vector3 operator-() const;
        Vector3 operator-(const Vector3 &v) const;
        Vector3 operator+(const Vector3 &v) const;
        Vector3 operator/(const float &l) const;

        float dotProduct(Vector3 v) const;
        Vector3 crossProduct(Vector3 v) const;
        Vector3 normalize() const;

        // Point3 toPoint()
        // {
        //     return Point3(x, y, z);
        // };

    public:
        double x;
        double y;
        double z;
        bool error = false;
    };
} // namespace isim