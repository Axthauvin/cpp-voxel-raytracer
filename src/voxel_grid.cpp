#include "voxel_grid.hh"

#include <cmath>
#include <cstddef>
#include <utility>

#include "objects/cuboid.hh"

namespace isim
{

  constexpr double EDGE_EPSILON = 1e-6;

  VoxelGrid::VoxelGrid(const std::vector<const Object*>& objects)
  {
    build(objects);
  }

  void VoxelGrid::build(const std::vector<const Object*>& objects)
  {
    voxels_.clear();
    min_x_ = std::numeric_limits<int>::max();
    max_x_ = std::numeric_limits<int>::min();
    min_y_ = std::numeric_limits<int>::max();
    max_y_ = std::numeric_limits<int>::min();
    min_z_ = std::numeric_limits<int>::max();
    max_z_ = std::numeric_limits<int>::min();

    for (const Object* object : objects)
      {
        add_object(object);
      }
  }

  void VoxelGrid::add_object(const Object* object)
  {
    const auto* cuboid = dynamic_cast<const Cuboid*>(object);
    if (!cuboid)
      {
        return;
      }

    const auto [x_begin, x_end] =
      voxel_index_range(cuboid->min_point.x, cuboid->max_point.x);
    const auto [y_begin, y_end] =
      voxel_index_range(cuboid->min_point.y, cuboid->max_point.y);
    const auto [z_begin, z_end] =
      voxel_index_range(cuboid->min_point.z, cuboid->max_point.z);

    for (int x = x_begin; x <= x_end; ++x)
      {
        for (int y = y_begin; y <= y_end; ++y)
          {
            for (int z = z_begin; z <= z_end; ++z)
              {
                voxels_[VoxelKey{x, y, z}] = object;
                min_x_ = std::min(min_x_, x);
                max_x_ = std::max(max_x_, x);
                min_y_ = std::min(min_y_, y);
                max_y_ = std::max(max_y_, y);
                min_z_ = std::min(min_z_, z);
                max_z_ = std::max(max_z_, z);
              }
          }
      }
  }

  const Object* VoxelGrid::get_block_at(int x, int y, int z) const
  {
    const auto it = voxels_.find(VoxelKey{x, y, z});
    if (it == voxels_.end())
      {
        return nullptr;
      }
    return it->second;
  }

  bool VoxelGrid::empty() const { return voxels_.empty(); }

  int VoxelGrid::min_x() const { return min_x_; }

  int VoxelGrid::max_x() const { return max_x_; }

  int VoxelGrid::min_y() const { return min_y_; }

  int VoxelGrid::max_y() const { return max_y_; }

  int VoxelGrid::min_z() const { return min_z_; }

  int VoxelGrid::max_z() const { return max_z_; }

  std::size_t VoxelGrid::VoxelKeyHasher::operator()(const VoxelKey& key) const
  {
    const std::size_t hx = std::hash<int>{}(key.x);
    const std::size_t hy = std::hash<int>{}(key.y);
    const std::size_t hz = std::hash<int>{}(key.z);
    return hx ^ (hy << 1) ^ (hz << 2);
  }

  std::pair<int, int> VoxelGrid::voxel_index_range(double min_value,
                                                   double max_value)
  {
    const int start =
      static_cast<int>(std::floor(min_value + 0.5 + EDGE_EPSILON));
    const int end =
      static_cast<int>(std::floor(max_value + 0.5 - EDGE_EPSILON));
    return {start, end};
  }
} // namespace isim
