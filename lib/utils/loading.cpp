#include "loading.hh"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <string>

namespace isim
{
  void Loading::show()
  {
    // Initialise le chrono au moment où on affiche la barre pour la première fois
    last_update = std::chrono::steady_clock::now();
    update(0.0);
  }

  void Loading::update(double new_progress)
  {
    auto now = std::chrono::steady_clock::now();
    auto elapsed =
      std::chrono::duration_cast<std::chrono::milliseconds>(now - last_update);

    if (elapsed.count() < refresh_rate_ms && new_progress < 1.0
        && new_progress > 0.0)
      return;

    last_update = now;
    progress = std::max(0.0, std::min(1.0, new_progress));

    size_t bar_width = termial_width > 12 ? termial_width - 12 : 10;
    size_t pos = static_cast<size_t>(bar_width * progress);

    std::cout << "\r[";

    for (size_t i = 0; i < bar_width; ++i)
      {
        if (i < pos)
          {
            std::cout << "\u2588";
          }
        else if (i == pos && progress < 1.0)
          {
            std::cout << "\u2592";
          }
        else
          {
            std::cout << " ";
          }
      }

    std::cout << "] " << std::fixed << std::setprecision(1)
              << (progress * 100.0) << " %" << std::flush;
  }

  void Loading::hide()
  {
    std::cout << "\r" << std::string(termial_width, ' ') << "\r" << std::flush;
  }
} // namespace isim