#pragma once

#include <vector>

#include "object.hh"
#include "objects/image_texture_material.hh"
#include "point.hh"
#include "vector.hh"

namespace isim
{

  class Cuboid : public Object
  {
  public:
    Cuboid(const Point3& min_point,
           const Point3& max_point,
           Texture_material* material)
      : min_point(min_point)
      , max_point(max_point)
    {
      this->material = material;
    }

    Cuboid(const Point3& min_point, const Point3& max_point, const Color color)
      : min_point(min_point)
      , max_point(max_point)
    {
      this->material = new Uniform_texture_material(color);
    }

    Cuboid(const Point3& center,
           float width,
           float height,
           float depth,
           Texture_material* material)
      : Cuboid(Point3(center.x - width / 2,
                      center.y - height / 2,
                      center.z - depth / 2),
               Point3(center.x + width / 2,
                      center.y + height / 2,
                      center.z + depth / 2),
               material)
    {}

    Cuboid(const Point3& center,
           float width,
           float height,
           float depth,
           const Color color)
      : Cuboid(Point3(center.x - width / 2,
                      center.y - height / 2,
                      center.z - depth / 2),
               Point3(center.x + width / 2,
                      center.y + height / 2,
                      center.z + depth / 2),
               color)
    {}

    Point3 min_point;
    Point3 max_point;
    std::vector<const Image_texture_material*> face_materials;

    float intersect(const Ray& ray) const override;
    Vector3 getNormalAt(const Point3& point) const override;
    Texture_properties
    getTextureCoordinatesAt(const Point3& point) const override;

    std::string getClassName() const override { return "Cuboid"; }

    void load_texture(const char* top_texture_filename,
                      const char* bottom_texture_filename,
                      const char* left_texture_filename,
                      const char* right_texture_filename,
                      const char* front_texture_filename,
                      const char* back_texture_filename);

    void load_texture(const char* top_texture_filename,
                      const char* bottom_texture_filename,
                      const char* side_texture_filename);
  };

} // namespace isim
