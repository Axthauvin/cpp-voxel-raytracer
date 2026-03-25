#pragma once

#include "object.hh"
#include "vector.hh"

namespace isim
{

    class Sphere : public Object
    {
    public:
        Sphere(const Point3 &center, float radius, Texture_material *material,
               Vector3 rotation = Vector3(0, 0, 0))
            : center(center)
            , radius(radius)
            , rotation(rotation)
        {
            this->material = material;
        }

        Sphere(const Point3 &center, float radius, const Color color,
               Vector3 rotation = Vector3(0, 0, 0))
            : center(center)
            , radius(radius)
            , rotation(rotation)
        {
            this->material = new Uniform_texture_material(color);
        }

        Point3 center;
        float radius;
        Vector3 rotation;

        float intersect(const Ray &ray) const override;
        void rotate(const Vector3 &rotation)
        {
            this->rotation = this->rotation + rotation;
        }

        Vector3 getNormalAt(const Point3 &point) const override;
        Texture_properties
        getTextureCoordinatesAt(const Point3 &point) const override;
        std::string getClassName() const override
        {
            return "Sphere";
        }
    };
} // namespace isim
