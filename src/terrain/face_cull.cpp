#include "face_cull.hh"

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <unordered_set>
#include <vector>

#include "objects/minecraft.hh"

#ifdef _OPENMP
#  include <omp.h>
#endif

namespace isim
{

  struct VoxelKey
  {
    int x;
    int y;
    int z;

    bool operator==(const VoxelKey& other) const
    {
      return x == other.x && y == other.y && z == other.z;
    }
  };

  struct VoxelKeyHash
  {
    std::size_t operator()(const VoxelKey& k) const
    {
      const std::uint64_t hx =
        static_cast<std::uint64_t>(static_cast<std::uint32_t>(k.x * 73856093));
      const std::uint64_t hy =
        static_cast<std::uint64_t>(static_cast<std::uint32_t>(k.y * 19349663));
      const std::uint64_t hz =
        static_cast<std::uint64_t>(static_cast<std::uint32_t>(k.z * 83492791));
      return static_cast<std::size_t>(hx ^ (hy << 1) ^ (hz << 2));
    }
  };

  VoxelKey to_key(const Block* block)
  {
    const Point3 p = block->position();
    return VoxelKey{static_cast<int>(std::lround(p.x)),
                    static_cast<int>(std::lround(p.y)),
                    static_cast<int>(std::lround(p.z))};
  }

  bool has_neighbor(const std::unordered_set<VoxelKey, VoxelKeyHash>& occupied,
                    int x,
                    int y,
                    int z)
  {
    return occupied.find(VoxelKey{x, y, z}) != occupied.end();
  }

  std::vector<const Object*>
  FaceCull::remove_fully_enclosed(const std::vector<const Object*>& objects)
  {
    if (objects.empty())
      {
        return {};
      }

    std::unordered_set<VoxelKey, VoxelKeyHash> occupied;
    occupied.reserve(objects.size() * 2);

    for (const Object* object : objects)
      {
        const Block* block = dynamic_cast<const Block*>(object);
        if (!block)
          {
            continue;
          }

        occupied.insert(to_key(block));
      }

    std::vector<unsigned char> keep(objects.size(), 1);

#pragma omp parallel for schedule(static)
    for (size_t i = 0; i < objects.size(); ++i)
      {
        const Block* block = dynamic_cast<const Block*>(objects[i]);
        if (!block)
          {
            keep[i] = 1;
            continue;
          }

        const VoxelKey k = to_key(block);

        const bool enclosed = has_neighbor(occupied, k.x + 1, k.y, k.z)
          && has_neighbor(occupied, k.x - 1, k.y, k.z)
          && has_neighbor(occupied, k.x, k.y + 1, k.z)
          && has_neighbor(occupied, k.x, k.y - 1, k.z)
          && has_neighbor(occupied, k.x, k.y, k.z + 1)
          && has_neighbor(occupied, k.x, k.y, k.z - 1);

        keep[i] = enclosed ? 0 : 1;
      }

    std::vector<const Object*> filtered;
    filtered.reserve(objects.size());

    for (size_t i = 0; i < objects.size(); ++i)
      {
        if (keep[i])
          {
            filtered.push_back(objects[i]);
          }
      }

    return filtered;
  }

} // namespace isim
