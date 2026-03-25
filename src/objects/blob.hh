#pragma once

#include <cmath>
#include <vector>

#include "objects/object.hh"
#include "objects/texture_material.hh"
#include "point.hh"

namespace isim
{
    class Blob
    {
    public:
        Blob(float d, std::vector<Point3> points, Point3 center, float S,
             Texture_material *mat)
            : d(d)
            , points(points)
            , center(center)
            , S(S)
            , material(mat)

        {}

        std::vector<const Object *> marching_cubes() const;

    private:
        float potentiel(const Point3 &p) const;
        int e = 2;
        float d = 0.5f;
        std::vector<Point3> points;
        Point3 center;
        float S;
        Texture_material *material;
    };
} // namespace isim