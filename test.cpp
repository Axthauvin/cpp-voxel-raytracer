#include <cmath>
#include <iostream>
#include <vector>

#include "scenes/scenes.hh"
#include "utils/image.hh"

int main()
{
  std::cout << "Rendering scene..." << std::endl;

  isim::Scene* scene = isim::minecraft_terrain_scene(50, 50, 0.03);
  // isim::Scene* scene = isim::water_lake_scene();

  std::cout << "Scene rendered, saving image..." << std::endl;

  std::vector<isim::Image> image =
    scene->render_all_cameras(isim::Resolution::SmallHD(), true);

  for (size_t i = 0; i < image.size(); i++)
    {
      std::string filename = "minecraft.png";
      image[i].save(filename.c_str());
      std::cout << "Scene rendered and saved to " << filename << std::endl;
    }

  return 0;
}