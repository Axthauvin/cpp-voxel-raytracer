#pragma once

#include <vector>

#include "objects/object.hh"

namespace isim
{
  class FaceCull
  {
  public:
    static std::vector<const Object*>
    remove_fully_enclosed(const std::vector<const Object*>& objects);
  };
} // namespace isim
