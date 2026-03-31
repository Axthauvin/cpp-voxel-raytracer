#pragma once

namespace isim
{
  class PerlinNoise
  {
  public:
    PerlinNoise();
    PerlinNoise(unsigned int seed);
    double noise(double x, double y) const;

  private:
    int p[512];
    double fade(double t) const;
    double lerp(double t, double a, double b) const;
    double grad(int hash, double x, double y) const;
  };
} // namespace isim