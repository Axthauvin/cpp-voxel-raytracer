#pragma once

#include <vector>

#include "objects/object.hh"
#include "perlin.hh"
#include "seed.hh"

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
                                        int octaves = 2,
                                        double persistence = 0.5,
                                        double lacunarity = 2.0) const;

  private:
    PerlinNoise perlin_;
    unsigned int world_seed_;
    std::vector<Seed> biome_seeds_;
    bool
      enable_reflection; // regular minecraft water doesn't have reflection, but it looks nicer with it in our renderer

    double fbm(double x,
               double z,
               double base_frequency,
               int octaves,
               double persistence,
               double lacunarity) const;

    BiomeKind
    pick_biome(int x, int z, int width, int depth, double noise_warp) const;

  private:
    double random_at(int x, int z) const;
  };

} // namespace isim
