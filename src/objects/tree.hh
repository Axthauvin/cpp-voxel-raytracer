#pragma once

#include "minecraft.hh"
#include "point.hh"

namespace isim
{
  class Tree
  {
  public:
    Tree() = default;
    ~Tree() = delete;

    static std::vector<Block*> place_tree(Point3 position,
                                          float trunk_height = 5);

    Point3 position;
  };
} // namespace isim