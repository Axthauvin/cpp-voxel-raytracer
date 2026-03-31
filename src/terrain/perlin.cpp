#include "perlin.hh"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <random>

#include "utils/lists.hh"

namespace isim
{

  int toInt(double x) { return static_cast<int>(std::floor(x)); }

  PerlinNoise::PerlinNoise()
  {
    std::iota(p, p + 256, 0);
    std::shuffle(p, p + 256, std::default_random_engine());
    for (int i = 0; i < 256; i++)
      {
        p[256 + i] = p[i];
      }
  }

  PerlinNoise::PerlinNoise(unsigned int seed)
  {
    std::iota(p, p + 256, 0);
    std::shuffle(p, p + 256, std::default_random_engine(seed));

    for (int i = 0; i < 256; i++)
      {
        p[256 + i] = p[i];
      }
  }

  double PerlinNoise::fade(double t) const
  {
    // Smoothstep function of ken perlin
    return t * t * t * (t * (t * 6 - 15) + 10);
  }

  double PerlinNoise::lerp(double t, double a, double b) const
  {
    return a + t * (b - a);
  }

  double PerlinNoise::grad(int hash, double x, double y) const
  {
    int h = hash & 3;
    double u = h < 2 ? x : y;
    double v = h < 2 ? y : x;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
  }

  double PerlinNoise::noise(double x, double y) const
  {
    int X = toInt(x) & 255;
    int Y = toInt(y) & 255;

    x -= toInt(x);
    y -= toInt(y);

    double u = fade(x);
    double v = fade(y);

    double res =
      lerp(v, lerp(u, grad(p[p[X] + Y], x, y), grad(p[p[X + 1] + Y], x - 1, y)),
           lerp(u, grad(p[p[X] + Y + 1], x, y - 1),
                grad(p[p[X + 1] + Y + 1], x - 1, y - 1)));

    return (res + 1.0) / 2.0; // Normalize to [0,1]
  }

} // namespace isim