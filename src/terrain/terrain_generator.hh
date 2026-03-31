#pragma once

#include <vector>

#include "objects/object.hh"
#include "perlin.hh"

namespace isim
{
  class TerrainGenerator
  {
  public:
    TerrainGenerator(bool enable_water__reflection);
    TerrainGenerator(unsigned int seed, bool enable_water_reflection);

    std::vector<const Object*> generate(int width,
                                        int depth,
                                        double scale,
                                        int maxHeight,
                                        int offsetX = 0,
                                        int offsetZ = 0,
                                        int octaves = 4,
                                        double persistence = 0.5,
                                        double lacunarity = 2.0) const;

  private:
    PerlinNoise perlin_;
    bool
      enable_reflection; // regular minecraft water doesn't have reflection, but it looks nicer with it in our renderer

    double fbm(double x,
               double z,
               int octaves,
               double persistence,
               double lacunarity) const;
  };

} // namespace isim
