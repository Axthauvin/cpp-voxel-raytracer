#include "blob.hh"

#include <iostream>
#include <vector>

#include "objects/object.hh"
#include "objects/triangle.hh"
#include "point.hh"

namespace isim
{
    float Blob::potentiel(const Point3 &p) const
    {
        float potentiel = 0.0f;
        for (const auto &point : points)
        {
            float distance = std::sqrt(std::pow(p.x - point.x, 2)
                                       + std::pow(p.y - point.y, 2)
                                       + std::pow(p.z - point.z, 2));
            potentiel += S / (distance * distance + d * d);
        }
        return potentiel;
    }

    std::vector<const Object *> Blob::marching_cubes() const
    {
        std::vector<const Object *> vertices;

        for (int x = -e; x < e; x++)
        {
            for (int y = -e; y < e; y++)
            {
                for (int z = -e; z < e; z++)
                {
                    std::vector<Point3> edges = {
                        // Bas du cube si j'ai compris
                        Point3(x, y, z),
                        Point3(x + d, y, z),
                        Point3(x + d, y, z + d),
                        Point3(x, y, z + d),

                        // Haut du cube ??
                        Point3(x, y + d, z),
                        Point3(x + d, y + d, z),
                        Point3(x + d, y + d, z + d),
                        Point3(x, y + d, z + d),
                    };

                    float potentiels[8];
                    for (int i = 0; i < 8; i++)
                    {
                        potentiels[i] = potentiel(edges[i]);
                    }

                    int index = 0;
                    if (potentiels[0] < S)
                        index |= 1;
                    if (potentiels[1] < S)
                        index |= 2;
                    if (potentiels[2] < S)
                        index |= 4;
                    if (potentiels[3] < S)
                        index |= 8;
                    if (potentiels[4] < S)
                        index |= 16;
                    if (potentiels[5] < S)
                        index |= 32;
                    if (potentiels[6] < S)
                        index |= 64;
                    if (potentiels[7] < S)
                        index |= 128;

                    // On prend l'index dans le vecteur de point, et on espère
                    // que ya pas d'overflow
                    vertices.push_back(
                        new Triangle(points[index], points[index + 1],
                                     points[index + 2], material));
                }
            }
        }

        std::cout << "Marching cubes done, generated " << vertices.size()
                  << " vertices." << std::endl;

        return vertices;
    }
} // namespace isim
