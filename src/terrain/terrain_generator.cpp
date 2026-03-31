#include "terrain_generator.hh"

#include <cmath>
#include "objects/minecraft.hh"
#include "objects/object.hh"

namespace isim
{
  TerrainGenerator::TerrainGenerator()
    : perlin_()
  {}

  TerrainGenerator::TerrainGenerator(unsigned int seed)
    : perlin_(seed)
  {}

  double TerrainGenerator::fbm(double x,
                               double z,
                               int octaves,
                               double persistence,
                               double lacunarity) const
  {
    double total = 0.0;
    double amplitude = 1.0;
    double frequency = 1.0;
    double norm = 0.0;

    for (int i = 0; i < octaves; ++i)
      {
        total += perlin_.noise(x * frequency, z * frequency) * amplitude;
        norm += amplitude;
        amplitude *= persistence;
        frequency *= lacunarity;
      }

    return total / norm;
  }

  std::vector<const Object*> TerrainGenerator::generate(int width,
                                                        int depth,
                                                        double scale,
                                                        int maxHeight,
                                                        int offsetX,
                                                        int offsetZ,
                                                        int octaves,
                                                        double persistence,
                                                        double lacunarity) const
  {
    std::vector<const Object*> out;
    out.reserve(static_cast<size_t>(width) * static_cast<size_t>(depth));

    for (int z = 0; z < depth; ++z)
      {
        for (int x = 0; x < width; ++x)
          {
            double vx = static_cast<double>(x + offsetX) * scale;
            double vz = static_cast<double>(z + offsetZ) * scale;

            double value = fbm(vx, vz, octaves, persistence, lacunarity);

            // Map value [0,1] -> centered [-1,1] then scale
            int y =
              static_cast<int>(std::lround(((value - 0.5) * 2.0) * maxHeight));

            isim::Point3 pos(static_cast<double>(x), static_cast<double>(y),
                             static_cast<double>(z));

            if (value < 0.40)
              {
                out.push_back(Block::new_water(pos));
              }
            else if (value < 0.45)
              {
                out.push_back(Block::new_sand(pos));
              }
            else
              {
                out.push_back(Block::new_grass(pos));
              }
          }
      }

    return out;
  }

} // namespace isim
