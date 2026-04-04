#include "terrain_generator.hh"

#include <algorithm>
#include <cmath>
#include <limits>
#include <random>

#include "face_cull.hh"
#include "objects/minecraft.hh"
#include "objects/object.hh"
#include "objects/tree.hh"

#ifdef _OPENMP
#  include <omp.h>
#endif

namespace isim
{

  constexpr int kBiomeSeedCount = 6;
  constexpr double kSeaLevelThreshold = 0.30;
  constexpr double kBeachThreshold = 0.33;
  constexpr double kBiomeWarpScale = 0.02;
  constexpr double kBiomeWarpStrength = 20.0;
  constexpr int kSeaLevelY = 0;
  constexpr int kTerrainBaseY = -16;
  constexpr double tree_chance = 0.005;
  constexpr int kTerraceStep = 2;
  constexpr double kRidgeMix = 0.35;
  constexpr double kSnowHeightRatio = 0.72;

  TerrainGenerator::TerrainGenerator(bool enable_water_reflection)
    : perlin_()
    , world_seed_(std::random_device{}())
    , biome_seeds_(Seed::generate(world_seed_, kBiomeSeedCount))
    , enable_reflection(enable_water_reflection)
  {}

  TerrainGenerator::TerrainGenerator(unsigned int seed,
                                     bool enable_water_reflection)
    : perlin_(seed)
    , world_seed_(seed)
    , biome_seeds_(Seed::generate(world_seed_, kBiomeSeedCount))
    , enable_reflection(enable_water_reflection)
  {}

  double TerrainGenerator::fbm(double x,
                               double z,
                               double base_frequency,
                               int octaves,
                               double persistence,
                               double lacunarity) const
  {
    double total = 0.0;
    double amplitude = 1.0;
    double frequency = base_frequency;
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

  BiomeKind TerrainGenerator::pick_biome(int x,
                                         int z,
                                         int width,
                                         int depth,
                                         double noise_warp) const
  {
    if (biome_seeds_.empty())
      {
        return BiomeKind::Plain;
      }

    const double warp = noise_warp * kBiomeWarpStrength;
    const double warped_x = static_cast<double>(x) + warp;
    const double warped_z = static_cast<double>(z) + warp;

    double min_distance = std::numeric_limits<double>::max();
    BiomeKind biome = BiomeKind::Plain;

    for (const Seed& seed : biome_seeds_)
      {
        const double sx = seed.x() * static_cast<double>(width);
        const double sz = seed.z() * static_cast<double>(depth);
        const double dx = warped_x - sx;
        const double dz = warped_z - sz;
        const double distance = std::sqrt(dx * dx + dz * dz);

        if (distance < min_distance)
          {
            min_distance = distance;
            biome = seed.biome_kind();
          }
      }

    return biome;
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
    std::vector<std::vector<const Object*>> columns(
      static_cast<size_t>(width) * static_cast<size_t>(depth));

#pragma omp parallel for collapse(2) schedule(dynamic)
    for (int z = 0; z < depth; ++z)
      {
        for (int x = 0; x < width; ++x)
          {
            const int world_x = x + offsetX;
            const int world_z = z + offsetZ;
            const size_t idx =
              static_cast<size_t>(z) * static_cast<size_t>(width)
              + static_cast<size_t>(x);

            auto& column = columns[idx];
            column.reserve(static_cast<size_t>(maxHeight + 32));

            const double noise_x = static_cast<double>(world_x);
            const double noise_z = static_cast<double>(world_z);

            const double value =
              fbm(noise_x, noise_z, scale, octaves, persistence, lacunarity);
            const double ridge_noise = 1.0
              - std::abs(2.0
                           * perlin_.noise(noise_x * scale * 1.8,
                                           noise_z * scale * 1.8)
                         - 1.0);
            const double terrain_value = std::clamp(
              value * (1.0 - kRidgeMix) + ridge_noise * kRidgeMix, 0.0, 1.0);
            const double biome_noise = perlin_.noise(noise_x * kBiomeWarpScale,
                                                     noise_z * kBiomeWarpScale);

            int surface_y =
              static_cast<int>(std::lround(((terrain_value - kSeaLevelThreshold)
                                            / (1.0 - kSeaLevelThreshold))
                                           * maxHeight));

            BiomeKind biome = BiomeKind::Plain;
            if (terrain_value >= kSeaLevelThreshold)
              {
                biome = pick_biome(x, z, width, depth, biome_noise);
                if (biome == BiomeKind::Mountain)
                  {
                    surface_y += static_cast<int>(std::lround(
                      (terrain_value - kSeaLevelThreshold) * maxHeight * 0.35));
                  }
              }

            if (surface_y < kTerrainBaseY)
              {
                surface_y = kTerrainBaseY;
              }

            if (kTerraceStep > 1)
              {
                surface_y = (surface_y / kTerraceStep) * kTerraceStep;
              }

            const bool is_underwater_surface = surface_y <= kSeaLevelY;
            const bool is_beach_band = terrain_value < kBeachThreshold;
            const bool is_snow_band =
              surface_y >= static_cast<int>(
                std::lround(kSnowHeightRatio * static_cast<double>(maxHeight)));

            for (int y = kTerrainBaseY; y <= surface_y; ++y)
              {
                const isim::Point3 pos(static_cast<double>(x),
                                       static_cast<double>(y),
                                       static_cast<double>(z));

                if (y == surface_y)
                  {
                    if (is_underwater_surface || is_beach_band
                        || biome == BiomeKind::Desert)
                      {
                        column.push_back(Block::new_sand(pos));
                      }
                    else if (biome == BiomeKind::Mountain)
                      {
                        if (is_snow_band)
                          {
                            column.push_back(Block::new_snow(pos));
                          }
                        else
                          {
                            column.push_back(Block::new_stone(pos));
                          }
                      }
                    else
                      {
                        column.push_back(Block::new_grass(pos));
                      }
                  }
                else
                  {
                    if (biome == BiomeKind::Desert || is_underwater_surface)
                      {
                        column.push_back(Block::new_sand(pos));
                      }
                    else if (biome == BiomeKind::Mountain)
                      {
                        column.push_back(Block::new_stone(pos));
                      }
                    else
                      {
                        const int depth_from_surface = surface_y - y;
                        if (depth_from_surface <= 3)
                          {
                            column.push_back(Block::new_dirt(pos));
                          }
                        else
                          {
                            column.push_back(Block::new_stone(pos));
                          }
                      }
                  }
              }

            if (surface_y < kSeaLevelY)
              {
                for (int y = surface_y + 1; y <= kSeaLevelY; ++y)
                  {
                    const isim::Point3 water_pos(static_cast<double>(x),
                                                 static_cast<double>(y),
                                                 static_cast<double>(z));
                    column.push_back(
                      Block::new_water(water_pos, 1, 1, enable_reflection));
                  }
              }

            if (!is_underwater_surface && biome == BiomeKind::Plain
                && random_at(world_x, world_z) < tree_chance)
              {
                const isim::Point3 tree_base(static_cast<double>(x),
                                             static_cast<double>(surface_y + 1),
                                             static_cast<double>(z));
                std::vector<isim::Block*> tree_blocks =
                  Tree::place_tree(tree_base);
                column.insert(column.end(), tree_blocks.begin(),
                              tree_blocks.end());
              }
          }
      }

    std::vector<const Object*> out;
    out.reserve(static_cast<size_t>(width) * static_cast<size_t>(depth)
                * static_cast<size_t>(maxHeight + 8));

    for (auto& column : columns)
      {
        out.insert(out.end(), column.begin(), column.end());
      }

    return FaceCull::remove_fully_enclosed(out);
  }

  double TerrainGenerator::random_at(int x, int z) const
  {
    unsigned int block_seed = world_seed_ + x * 374761393 + z * 668265263;
    std::mt19937 rng(block_seed);
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng);
  }

} // namespace isim
