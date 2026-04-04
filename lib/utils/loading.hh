#pragma once

#include <chrono>
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
    std::chrono::steady_clock::time_point last_update;
    static const size_t refresh_rate_ms = 500;
  };
} // namespace isim