#pragma once

#include <string>

#include "point.hh"
#include "ray.hh"
#include "texture_material.hh"
#include "vector.hh"

namespace isim
{

    class Object
    {
    public:
        Object() = default;
        Texture_material *material;
        virtual ~Object() = default;
        virtual float intersect(const Ray &ray) const = 0;
        virtual Vector3 getNormalAt(const Point3 &point) const = 0;
        virtual Texture_properties
        getTextureCoordinatesAt(const Point3 &point) const = 0;

        virtual std::string getClassName() const = 0;
    };
} // namespace isim