#pragma once

#include <limits>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "objects/object.hh"

namespace isim
{
  class VoxelGrid
  {
  public:
    VoxelGrid() = default;
    explicit VoxelGrid(const std::vector<const Object*>& objects);

    void build(const std::vector<const Object*>& objects);
    void add_object(const Object* object);

    const Object* get_block_at(int x, int y, int z) const;
    bool empty() const;

    int min_x() const;
    int max_x() const;
    int min_y() const;
    int max_y() const;
    int min_z() const;
    int max_z() const;

  private:
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

    struct VoxelKeyHasher
    {
      std::size_t operator()(const VoxelKey& key) const;
    };

    static std::pair<int, int> voxel_index_range(double min_value,
                                                 double max_value);

    std::unordered_map<VoxelKey, const Object*, VoxelKeyHasher> voxels_;

    int min_x_ = std::numeric_limits<int>::max();
    int max_x_ = std::numeric_limits<int>::min();
    int min_y_ = std::numeric_limits<int>::max();
    int max_y_ = std::numeric_limits<int>::min();
    int min_z_ = std::numeric_limits<int>::max();
    int max_z_ = std::numeric_limits<int>::min();
  };
} // namespace isim
