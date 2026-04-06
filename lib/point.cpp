#include "point.hh"

#include <iostream>

#include "vector.hh"

namespace isim
{

  Point3 Point3::operator*(const float& l) const
  {
    return Point3(x * l, y * l, z * l);
  }

  Vector3 Point3::operator-(const Point3& v) const
  {
    return Vector3(x - v.x, y - v.y, z - v.z);
  }

  Point3 Point3::operator+(const Vector3& p) const
  {
    return Point3(x + p.x, y + p.y, z + p.z);
  }

  Point3 Point3::operator-(const Vector3& p) const
  {
    return Point3(x - p.x, y - p.y, z - p.z);
  }

  Point3 Point3::operator*(const Vector3& p) const
  {
    return Point3(x * p.x, y * p.y, z * p.z);
  }

  bool Point3::operator==(const Point3& other) const
  {
    return x == other.x && y == other.y && z == other.z;
  }

  bool Point3::operator!=(const Point3& other) const
  {
    return !(*this == other);
  }

  std::ostream& operator<<(std::ostream& os, const Point3& p)
  {
    os << "Point3(" << p.x << ", " << p.y << ", " << p.z << ")";
    return os;
  }
} // namespace isim