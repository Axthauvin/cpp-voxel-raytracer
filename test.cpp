#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <vector>

#include "scenes/scenes.hh"
#include "utils/image.hh"

int main()
{
  std::cout << "Rendering scene..." << std::endl;
  size_t terrain_size = 500;
  double scale = 4.0 / terrain_size;
  size_t camera_count = 24;

  isim::SceneOutput output = isim::minecraft_terrain_scene(
    terrain_size, terrain_size, scale, 50, camera_count);
  // isim::SceneOutput output = isim::minecraft_tree();
  isim::Scene* scene = output.result;
  std::string output_name = output.scene_name;

  std::cout << "Scene " << output_name << " created, rendering..." << std::endl;

  std::vector<isim::Image> image =
    scene->render_all_cameras(isim::Resolution::SmallHD(), true);

  for (size_t i = 0; i < image.size(); i++)
    {
      std::string filename;
      if (image.size() == 1)
        {
          filename = output_name + ".png";
        }
      else
        {
          filename = output_name + "_camera_" + std::to_string(i) + ".png";
        }
      image[i].save(filename.c_str());
      std::cout << "Scene rendered and saved to " << filename << std::endl;
    }

  return 0;
}