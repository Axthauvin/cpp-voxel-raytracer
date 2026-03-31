#include "scenes.hh"

#include <cerrno>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <vector>

#include "camera.hh"
#include "color.hh"
#include "light.hh"
#include "objects/blob.hh"
#include "objects/cuboid.hh"
#include "objects/image_texture_material.hh"
#include "objects/minecraft.hh"
#include "objects/object.hh"
#include "objects/sphere.hh"
#include "objects/texture_material.hh"
#include "objects/tree.hh"
#include "objects/triangle.hh"
#include "point.hh"
#include "scene.hh"
#include "terrain/terrain_generator.hh"
#include "vector.hh"

namespace isim
{

  Scene* grass_block_scene()
  {
    static isim::Camera camera(isim::Point3(0, 1.5, 0), isim::Point3(0, 0.5, 5),
                               isim::Vector3(0, 1, 0), 60, 45, 1);

    // static isim::Image_texture_material grass_mat(
    //     "textures/grass_block_texture.tga", 1, 0, 250.0f);

    static isim::Uniform_texture_material grass_mat(isim::Color::green, 1, 0,
                                                    250.0f);
    static std::vector<isim::Cuboid> grass_blocks = {
      isim::Cuboid(isim::Point3(-2, 0.5, 3), 1, 1, 1, &grass_mat),
      isim::Cuboid(isim::Point3(-1, 0.5, 3), 1, 1, 1, &grass_mat),
      isim::Cuboid(isim::Point3(0, 0.5, 3), 1, 1, 1, &grass_mat),
      isim::Cuboid(isim::Point3(1, 0.5, 3), 1, 1, 1, &grass_mat),
      isim::Cuboid(isim::Point3(2, 0.5, 3), 1, 1, 1, &grass_mat),

      isim::Cuboid(isim::Point3(-2, 0.5, 4), 1, 1, 1, &grass_mat),
      isim::Cuboid(isim::Point3(-1, 0.5, 4), 1, 1, 1, &grass_mat),
      isim::Cuboid(isim::Point3(0, 0.5, 4), 1, 1, 1, &grass_mat),
      isim::Cuboid(isim::Point3(1, 0.5, 4), 1, 1, 1, &grass_mat),
      isim::Cuboid(isim::Point3(2, 0.5, 4), 1, 1, 1, &grass_mat),

      isim::Cuboid(isim::Point3(-2, 0.5, 5), 1, 1, 1, &grass_mat),
      isim::Cuboid(isim::Point3(-1, 0.5, 5), 1, 1, 1, &grass_mat),
      isim::Cuboid(isim::Point3(0, 0.5, 5), 1, 1, 1, &grass_mat),
      isim::Cuboid(isim::Point3(1, 0.5, 5), 1, 1, 1, &grass_mat),
      isim::Cuboid(isim::Point3(2, 0.5, 5), 1, 1, 1, &grass_mat),

      isim::Cuboid(isim::Point3(-2, 0.5, 6), 1, 1, 1, &grass_mat),
      isim::Cuboid(isim::Point3(-1, 0.5, 6), 1, 1, 1, &grass_mat),
      isim::Cuboid(isim::Point3(0, 0.5, 6), 1, 1, 1, &grass_mat),
      isim::Cuboid(isim::Point3(1, 0.5, 6), 1, 1, 1, &grass_mat),
      isim::Cuboid(isim::Point3(2, 0.5, 6), 1, 1, 1, &grass_mat),

      isim::Cuboid(isim::Point3(-2, 0.5, 7), 1, 1, 1, &grass_mat),
      isim::Cuboid(isim::Point3(-1, 0.5, 7), 1, 1, 1, &grass_mat),
      isim::Cuboid(isim::Point3(0, 0.5, 7), 1, 1, 1, &grass_mat),
      isim::Cuboid(isim::Point3(1, 0.5, 7), 1, 1, 1, &grass_mat),
      isim::Cuboid(isim::Point3(2, 0.5, 7), 1, 1, 1, &grass_mat),
    };

    static bool textures_loaded = []() {
      for (auto& grass_block : grass_blocks)
        {
          grass_block.load_texture("textures/grass/grass_block_top.tga",
                                   "textures/grass/dirt.tga",
                                   "textures/grass/grass_block_side.tga");
        }
      return true;
    }();
    (void)textures_loaded;

    static const std::vector<const isim::Object*> objects = []() {
      std::vector<const isim::Object*> terrain;
      terrain.reserve(grass_blocks.size());
      for (const auto& grass_block : grass_blocks)
        {
          terrain.push_back(&grass_block);
        }
      return terrain;
    }();

    static isim::PointLight main_light(isim::Point3(-5, 10, -2),
                                       isim::Color{255, 255, 255});

    static const std::vector<const isim::Light*> lights = {&main_light};

    static Scene scene(objects, lights, {&camera}, Color::sky_blue,
                       Color::black);

    return &scene;
  }

  Scene* test_block_texure(const char* top_texture_filename,
                           const char* bottom_texture_filename,
                           const char* side_texture_filename)
  {
    static isim::Camera camera(isim::Point3(0, -1, -2), isim::Point3(0, 0, 0),
                               isim::Vector3(0, 1, 0), 60, 45, 1);

    static isim::Block grass_block = Block::grass(isim::Point3(0, 0, 0));

    static isim::Uniform_texture_material ground_mat(isim::Color::white, .8, 0,
                                                     20.0f);

    static isim::Sphere ground(isim::Point3(0, 1001, 5), 1000, &ground_mat);

    static const std::vector<const isim::Object*> objects = {&grass_block,
                                                             &ground};

    static isim::PointLight main_light(isim::Point3(-5, -5, -5),
                                       isim::Color{255, 255, 255});

    static const std::vector<const isim::Light*> lights = {&main_light};

    static Scene scene(objects, lights, {&camera}, Color::sky_blue,
                       Color::black);

    return &scene;
  }

  Scene* minecraft_tree()
  {
    static isim::Camera camera(isim::Point3(0, 5, -10), isim::Point3(0, 3, 0),
                               isim::Vector3(0, 1, 0), 60, 45, 1);

    static std::vector<isim::Block*> tree_blocks =
      Tree::place_tree(isim::Point3(0, 0, 0), 5);

    static const std::vector<const isim::Object*> objects = []() {
      std::vector<const isim::Object*> ptrs;
      ptrs.reserve(tree_blocks.size());
      for (const auto block : tree_blocks)
        {
          ptrs.push_back(block);
        }

      // add a ground plane to make the tree look better
      for (int x = -10; x <= 10; x++)
        {
          for (int z = -10; z <= 10; z++)
            {
              ptrs.push_back(isim::Block::new_grass(isim::Point3(x, -1, z)));
            }
        }

      return ptrs;
    }();

    static isim::PointLight main_light(isim::Point3(-5, 10, -5),
                                       isim::Color{255, 255, 255});

    static const std::vector<const isim::Light*> lights = {&main_light};

    static Scene scene(objects, lights, {&camera}, Color::sky_blue,
                       Color::black);

    return &scene;
  }

  Scene* water_lake_scene()
  {
    static isim::Camera camera(isim::Point3(7, 12, -14), isim::Point3(0, 0, 2),
                               isim::Vector3(0, 1, 0), 60, 45, 1);

    static const std::vector<const isim::Object*> objects = []() {
      std::vector<const isim::Object*> scene_objects;

      const int lake_radius = 3;
      const int lake_size = (lake_radius * 2) + 1;
      std::vector<std::vector<isim::Block*>> water_blocks(
        lake_size, std::vector<isim::Block*>(lake_size, nullptr));

      // Build a larger lake with a grass rim and a grass base.
      for (int x = -lake_radius; x <= lake_radius; x++)
        {
          for (int z = -lake_radius; z <= lake_radius; z++)
            {
              isim::Block* water = Block::new_water(isim::Point3(x, 0, z));
              water_blocks[x + lake_radius][z + lake_radius] = water;
              scene_objects.push_back(water);
              scene_objects.push_back(Block::new_grass(isim::Point3(x, -1, z)));
            }
        }

      // little culling implementation
      // will have to make it automatic later, for this scene, it works !
      for (int i = 0; i < lake_size; i++)
        {
          for (int j = 0; j < lake_size; j++)
            {
              isim::Block* current = water_blocks[i][j];
              if (!current)
                continue;

              current->active_bottom = false;

              if (i + 1 < lake_size && water_blocks[i + 1][j])
                {
                  current->active_right = false;
                  water_blocks[i + 1][j]->active_left = false;
                }

              if (j + 1 < lake_size && water_blocks[i][j + 1])
                {
                  current->active_front = false;
                  water_blocks[i][j + 1]->active_back = false;
                }
            }
        }

      for (int x = -lake_radius - 1; x <= lake_radius + 1; x++)
        {
          scene_objects.push_back(
            Block::new_grass(isim::Point3(x, 0, -lake_radius - 1)));
          scene_objects.push_back(
            Block::new_grass(isim::Point3(x, 0, lake_radius + 1)));
        }

      for (int z = -lake_radius; z <= lake_radius; z++)
        {
          scene_objects.push_back(
            Block::new_grass(isim::Point3(-lake_radius - 1, 0, z)));
          scene_objects.push_back(
            Block::new_grass(isim::Point3(lake_radius + 1, 0, z)));
        }

      // place a tree next to the lake for more visual interest
      std::vector<isim::Block*> tree_blocks =
        Tree::place_tree(isim::Point3(-3, 1, 3), 5);
      for (const auto block : tree_blocks)
        {
          scene_objects.push_back(block);
        }

      // scene_objects.push_back(Block::new_sand(isim::Point3(0, 0, 0)));

      // add sand patchwork in the top right corner of the lake
      scene_objects.push_back(Block::new_sand(isim::Point3(-3, 0, 3)));
      scene_objects.push_back(Block::new_sand(isim::Point3(-2, 0, 3)));
      scene_objects.push_back(Block::new_sand(isim::Point3(-1, 0, 3)));
      scene_objects.push_back(Block::new_sand(isim::Point3(-2, 0, 2)));
      scene_objects.push_back(Block::new_sand(isim::Point3(-3, 0, 2)));
      scene_objects.push_back(Block::new_sand(isim::Point3(-3, 0, 1)));

      // scene_objects.push_back(Block::new_sand(isim::Point3(-3, 0, 3)));

      return scene_objects;
    }();

    static isim::PointLight main_light(isim::Point3(-5, 10, -5),
                                       isim::Color{255, 255, 255});

    static const std::vector<const isim::Light*> lights = {&main_light};

    static Scene scene(objects, lights, {&camera}, Color::sky_blue,
                       Color::black);

    return &scene;
  }

  Scene*
  minecraft_terrain_scene(int width, int depth, double scale, int max_height)
  {
    TerrainGenerator terrain(42, true);

    static std::vector<const Object*> blocks =
      terrain.generate(width, depth, scale, max_height);

    // iterate on all blocks to find the lowest y value
    double min_y = std::numeric_limits<double>::max();
    for (const Object* block : blocks)
      {
        const Point3 pos = static_cast<const Block*>(block)->position();
        if (pos.y < min_y)
          {
            min_y = pos.y;
          }
      }

    // add a layer of grass blocks at the bottom of the terrain
    for (int x = 0; x < width; x++)
      {
        for (int z = 0; z < depth; z++)
          {
            blocks.push_back(Block::new_grass(Point3(x, min_y, z)));
          }
      }

    static isim::Camera camera(Point3(width, 10, depth), Point3(0, 0, 0),
                               isim::Vector3(0, 1, 0), 60, 45, 1.0f);

    static isim::PointLight main_light(isim::Point3(-5, 10, -5),
                                       isim::Color{255, 255, 255});

    static const std::vector<const isim::Light*> lights = {&main_light};

    static Scene scene(blocks, lights, {&camera}, Color::sky_blue,
                       Color::black);

    return &scene;
  }

  Scene* water_test()
  {
    // just a wall of grass, with water blocks in front of it, to test water rendering and transparency
    static isim::Camera camera(Point3(0, 1.5, 5), Point3(0, 0.5, 0),
                               isim::Vector3(0, 1, 0), 60, 45, 1);

    static std::vector<const Object*> objects = []() {
      std::vector<const Object*> scene_objects;

      for (int x = -2; x <= 2; x++)
        {
          scene_objects.push_back(Block::new_grass(Point3(x, 0.5, 0)));
        }

      for (int x = -2; x <= 2; x++)
        {
          scene_objects.push_back(Block::new_water(Point3(x, 0.5, 1)));
        }

      return scene_objects;
    }();

    static isim::PointLight main_light(isim::Point3(-5, 10, -5),
                                       isim::Color{255, 255, 255});

    static const std::vector<const isim::Light*> lights = {&main_light};

    static Scene scene(objects, lights, {&camera}, Color::sky_blue,
                       Color::black);

    return &scene;
  }

} // namespace isim
