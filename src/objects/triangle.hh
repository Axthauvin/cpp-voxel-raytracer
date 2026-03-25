#pragma once

#include "objects/object.hh"
#include "point.hh"

namespace isim
{
    class Triangle : public Object
    {
    public:
        Triangle(Point3 point1, Point3 point2, Point3 point3,
                 Texture_material *material)
            : point1(point1)
            , point2(point2)
            , point3(point3)
        {
            this->material = material;
        }

        float intersect(const Ray &ray) const override;
        Vector3 getNormalAt(const Point3 &point) const override;
        Texture_properties
        getTextureCoordinatesAt(const Point3 &point) const override;

        std::string getClassName() const override
        {
            return "Triangle";
        }

    private:
        Point3 point1;
        Point3 point2;
        Point3 point3;
    };
} // namespace isim