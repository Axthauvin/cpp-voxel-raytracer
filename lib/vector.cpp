#include "vector.hh"

#include <cmath>
#include <ostream>
#include <stdexcept>

namespace isim
{

    Vector3 Vector3::operator*(const float &l) const
    {
        return Vector3(x * l, y * l, z * l);
    }

    Vector3 Vector3::operator*(const double &l) const
    {
        return Vector3(x * l, y * l, z * l);
    }

    Vector3 Vector3::operator*(const int &l) const
    {
        return Vector3(x * l, y * l, z * l);
    }

    Vector3 Vector3::operator-() const
    {
        return Vector3(x * -1, y * -1, z * -1);
    }

    Vector3 Vector3::operator-(const Vector3 &v) const
    {
        return Vector3(x - v.x, y - v.y, z - v.z);
    }

    Vector3 Vector3::operator+(const Vector3 &v) const
    {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }

    std::ostream &operator<<(std::ostream &os, const Vector3 &v)
    {
        os << "Vector3(" << v.x << ", " << v.y << ", " << v.z << ")";
        return os;
    }

    float Vector3::dotProduct(Vector3 v) const
    {
        return x * v.x + y * v.y + z * v.z;
    }

    Vector3 Vector3::crossProduct(Vector3 v) const
    {
        return Vector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }

    Vector3 Vector3::operator/(const float &l) const
    {
        if (l == 0)
        {
            throw std::invalid_argument("Cannot divide by zero");
        }
        return Vector3(x / l, y / l, z / l);
    }

    Vector3 Vector3::normalize() const
    {
        double length = sqrt(x * x + y * y + z * z);
        if (length == 0)
        {
            throw std::invalid_argument("Cannot normalize a zero vector");
        }

        return Vector3(x / length, y / length, z / length);
    }

} // namespace isim