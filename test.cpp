#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <vector>

#include "point.hh"
#include "scenes/scenes.hh"
#include "utils/image.hh"

#include "schematica/parser.hh"

int main()
{
  std::cout << "Rendering scene..." << std::endl;
  size_t terrain_size = 500;
  double scale = 4.0 / terrain_size;
  size_t camera_count = 1;

  // isim::SceneOutput output = isim::minecraft_terrain_scene(
  //   terrain_size, terrain_size, scale, 50, camera_count);
  // isim::SceneOutput output = isim::minecraft_tree();

  // isim::SceneOutput output =
  //   isim::load_schematic("schematics/temple_of_notch.schem", true, 0,
  //                        isim::Point3(71, 67, 77), isim::Point3(72, 78, 106));

  isim::SceneOutput output =
    isim::load_schematic("schematics/dragongamer.schem", true, 0);
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