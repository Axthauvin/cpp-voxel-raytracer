#include "seed.hh"
#include <random>

namespace isim
{

  Seed::Seed(double x, double z, BiomeKind biome_kind)
    : x_(x)
    , z_(z)
    , biome_kind_(biome_kind)
  {}

  std::vector<Seed> Seed::generate(unsigned int world_seed, int count)
  {
    std::vector<Seed> out;
    if (count <= 0)
      {
        return out;
      }

    out.reserve(static_cast<size_t>(count));

    std::mt19937 rng(world_seed);
    std::uniform_real_distribution<double> coord_dist(0.0, 1.0);
    std::uniform_int_distribution<int> biome_dist(1, 3); // Excludes sea.

    for (int i = 0; i < count; ++i)
      {
        const double x = coord_dist(rng);
        const double z = coord_dist(rng);
        const BiomeKind biome = static_cast<BiomeKind>(biome_dist(rng));
        out.emplace_back(x, z, biome);
      }

    return out;
  }

  double Seed::x() const { return x_; }

  double Seed::z() const { return z_; }

  BiomeKind Seed::biome_kind() const { return biome_kind_; }

} // namespace isim
