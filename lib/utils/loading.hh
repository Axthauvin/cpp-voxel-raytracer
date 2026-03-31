#pragma once

#include <cstddef>

namespace isim
{
  class Loading
  {
  public:
    Loading() = default;

    void show();
    void update(double new_progress);

    void hide();

  private:
    double progress = 0.0;
    size_t termial_width = 80;
  };
} // namespace isim