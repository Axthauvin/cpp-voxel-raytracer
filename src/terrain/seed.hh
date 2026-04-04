#pragma once

#include <vector>

namespace isim
{
  enum class BiomeKind
  {
    Sea,
    Desert,
    Plain,
    Mountain,
  };

  class Seed
  {
  public:
    Seed(double x, double z, BiomeKind biome_kind);

    static std::vector<Seed> generate(unsigned int world_seed, int count);

    double x() const;
    double z() const;
    BiomeKind biome_kind() const;

  private:
    double x_;
    double z_;
    BiomeKind biome_kind_;
  };

} // namespace isim
