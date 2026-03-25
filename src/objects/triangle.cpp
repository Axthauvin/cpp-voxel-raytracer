#include "triangle.hh"

#include <cmath>

#include "point.hh"
#include "vector.hh"

namespace isim
{
    Vector3 Triangle::getNormalAt(const Point3 &point) const
    {
        // https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution.html

        Vector3 v0v1 = point2 - point1;
        Vector3 v0v2 = point3 - point1;
        // No need to normalize
        return v0v1.crossProduct(v0v2);
    }

    float Triangle::intersect(const Ray &ray) const
    {
        // https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution.html

        Vector3 v0v1 = point2 - point1;
        Vector3 v0v2 = point3 - point1;

        // Compute the plane's normal
        Vector3 N = v0v1.crossProduct(v0v2);

        // Step 1: Finding P

        // Check if the ray and plane are parallel
        float NdotRayDirection = N.dotProduct(ray.direction);

        if (fabs(NdotRayDirection) < 0.001f) // Almost 0
            return -1.0f; // They are parallel, so they don't intersect!

        // Compute d parameter using equation 2
        float d = -N.dotProduct(point1.toVect());

        // Compute t (equation 3)
        float t = -(N.dotProduct(ray.origin.toVect()) + d) / NdotRayDirection;

        // Check if the triangle is behind the ray
        if (t < 0)
            return t; // The triangle is behind

        // Compute the intersection point using equation 1
        Point3 P = ray.origin + ray.direction * t;

        // Step 2: Inside-Outside Test
        Vector3 Ne; // Vector perpendicular to triangle's plane

        // Test sidedness of P w.r.t. edge v0v1
        Vector3 v0p = P - point1;
        Ne = v0v1.crossProduct(v0p);
        if (N.dotProduct(Ne) < 0)
            return -1.0f; // P is on the right side

        // Test sidedness of P w.r.t. edge v2v1
        Vector3 v2v1 = point3 - point2;
        Vector3 v1p = P - point2;
        Ne = v2v1.crossProduct(v1p);
        if (N.dotProduct(Ne) < 0)
            return -1.0f; // P is on the right side

        // Test sidedness of P w.r.t. edge v2v0
        Vector3 v2v0 = point1 - point3;
        Vector3 v2p = P - point3;
        Ne = v2v0.crossProduct(v2p);
        if (N.dotProduct(Ne) < 0)
            return -1.0f; // P is on the right side

        return t; // The ray hits the triangle
    }

    Texture_properties
    Triangle::getTextureCoordinatesAt(const Point3 &point) const
    {
        // for now no texture mapping my brother
        return material->getElementsAt(0, 0);
    }
} // namespace isim