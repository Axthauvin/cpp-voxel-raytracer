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
#include "vector.hh"

namespace isim
{
    Scene *sphere_scene()
    {
        static isim::Camera camera(isim::Point3(0, 1.5, -3),
                                   isim::Point3(0, 1, 5),
                                   isim::Vector3(0, 1, 0), 60, 45, 1);

        static isim::Uniform_texture_material peach_mat(isim::Color::peach_pink,
                                                        1, 0.1, 100.0f);
        static isim::Uniform_texture_material hotpink_mat(isim::Color::hot_pink,
                                                          1, 0.1, 100.0f);
        static isim::Uniform_texture_material palepink_mat(
            isim::Color::pale_pink, 1, 0.1, 100.0f);
        static isim::Uniform_texture_material yellow_mat(isim::Color::yellow, 1,
                                                         0.1, 100.0f);
        static isim::Uniform_texture_material darkpink_mat(
            isim::Color::dark_pink, 1, 0.1, 100.0f);

        static isim::Uniform_texture_material ground_mat(isim::Color::white, .8,
                                                         0, 20.0f);

        static isim::Sphere ground(isim::Point3(0, -1000, 5), 1000,
                                   &ground_mat);

        static isim::Sphere center_sphere(isim::Point3(0, 1.5, 5), 1.5,
                                          &peach_mat);
        static isim::Sphere right_sphere(isim::Point3(3.2, 2.0, 5.5), 2.0,
                                         &hotpink_mat);
        static isim::Sphere front_right_sphere(isim::Point3(1.8, 0.6, 3.5), 0.6,
                                               &palepink_mat);
        static isim::Sphere front_left_sphere(isim::Point3(-1.2, 0.5, 2.5), 0.5,
                                              &yellow_mat);
        static isim::Sphere left_sphere(isim::Point3(-2.8, 1.2, 4.0), 1.2,
                                        &darkpink_mat);

        static const std::vector<const isim::Object *> objects = {
            &ground,
            &center_sphere,
            &right_sphere,
            &front_right_sphere,
            &front_left_sphere,
            &left_sphere,
        };

        static isim::PointLight main_light(isim::Point3(-5, 10, -2),
                                           isim::Color{ 255, 255, 255 });

        static const std::vector<const isim::Light *> lights = { &main_light };

        static Scene scene(objects, lights, { &camera }, isim::Color::sky_blue,
                           isim::DEFAULT_AMBIENT_LIGHT.color);

        // isim::Image image = scene.render(camera, isim::Resolution::FullHD());
        // image.save("output.tga");

        return &scene;
    }

    Scene *the_earth_scene()
    {
        static isim::Camera camera(isim::Point3(0, 1.5, -3),
                                   isim::Point3(0, 1, 5),
                                   isim::Vector3(0, 1, 0), 60, 45, 1);

        static isim::Image_texture_material earth_mat(
            "textures/earth_texture.tga", 3, 0, 255.0f);
        static isim::Sphere earth(isim::Point3(0, 1.5, 5), 1.5, &earth_mat);

        earth.rotate(isim::Vector3(0.5, -2, 0));

        // big sphere for the background stars

        static isim::Image_texture_material stars_mat(
            "textures/stars_texture.tga", 1, 0, 250.0f);
        static isim::Sphere big_sphere(isim::Point3(0, 0, 2000), 1000,
                                       &stars_mat);

        static const std::vector<const isim::Object *> objects = {
            &earth, &big_sphere
        };

        static isim::PointLight main_light(isim::Point3(-5, 1.5, 0),
                                           isim::Color{ 255, 255, 255 });

        static const std::vector<const isim::Light *> lights = { &main_light };

        static Scene scene(objects, lights, { &camera }, Color::black,
                           Color::black);

        return &scene;
    }

    Scene *the_moon_scene()
    {
        static isim::Camera camera(isim::Point3(0, 1.5, -3),
                                   isim::Point3(0, 1, 5),
                                   isim::Vector3(0, 1, 0), 60, 45, 1);

        static isim::Image_texture_material earth_mat(
            "textures/moon_texture.tga", 1, 0, 250.0f);
        static isim::Sphere moon(isim::Point3(0, 1.5, 5), 1.5, &earth_mat);

        // big sphere for the background stars

        static isim::Image_texture_material stars_mat(
            "textures/stars_texture.tga", 1, 0, 250.0f);
        static isim::Sphere big_sphere(isim::Point3(0, 0, 2000), 1000,
                                       &stars_mat);

        static const std::vector<const isim::Object *> objects = {
            &moon, &big_sphere
        };

        static isim::PointLight main_light(isim::Point3(-5, 1.5, 0),
                                           isim::Color{ 255, 255, 255 });

        static const std::vector<const isim::Light *> lights = { &main_light };

        static Scene scene(objects, lights, { &camera }, Color::black,
                           Color::black);

        return &scene;
    }

    Scene *grass_block_scene()
    {
        static isim::Camera camera(isim::Point3(0, 1.5, 0),
                                   isim::Point3(0, 0.5, 5),
                                   isim::Vector3(0, 1, 0), 60, 45, 1);

        // static isim::Image_texture_material grass_mat(
        //     "textures/grass_block_texture.tga", 1, 0, 250.0f);

        static isim::Uniform_texture_material grass_mat(isim::Color::green, 1,
                                                        0, 250.0f);
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
            for (auto &grass_block : grass_blocks)
            {
                grass_block.load_texture("textures/grass/grass_block_top.tga",
                                         "textures/grass/dirt.tga",
                                         "textures/grass/grass_block_side.tga");
            }
            return true;
        }();
        (void)textures_loaded;

        static const std::vector<const isim::Object *> objects = []() {
            std::vector<const isim::Object *> terrain;
            terrain.reserve(grass_blocks.size());
            for (const auto &grass_block : grass_blocks)
            {
                terrain.push_back(&grass_block);
            }
            return terrain;
        }();

        static isim::PointLight main_light(isim::Point3(-5, 10, -2),
                                           isim::Color{ 255, 255, 255 });

        static const std::vector<const isim::Light *> lights = { &main_light };

        static Scene scene(objects, lights, { &camera }, Color::sky_blue,
                           Color::black);

        return &scene;
    }

    Scene *test_block_texure(const char *top_texture_filename,
                             const char *bottom_texture_filename,
                             const char *side_texture_filename)
    {
        static isim::Camera camera(isim::Point3(0, -1, -2),
                                   isim::Point3(0, 0, 0),
                                   isim::Vector3(0, 1, 0), 60, 45, 1);

        static isim::Block grass_block = Block::grass(isim::Point3(0, 0, 0));

        static isim::Uniform_texture_material ground_mat(isim::Color::white, .8,
                                                         0, 20.0f);

        static isim::Sphere ground(isim::Point3(0, 1001, 5), 1000, &ground_mat);

        static const std::vector<const isim::Object *> objects = { &grass_block,
                                                                   &ground };

        static isim::PointLight main_light(isim::Point3(-5, -5, -5),
                                           isim::Color{ 255, 255, 255 });

        static const std::vector<const isim::Light *> lights = { &main_light };

        static Scene scene(objects, lights, { &camera }, Color::sky_blue,
                           Color::black);

        return &scene;
    }

    Scene *minecraft_tree()
    {
        static isim::Camera camera(isim::Point3(0, 5, -10),
                                   isim::Point3(0, 3, 0),
                                   isim::Vector3(0, 1, 0), 60, 45, 1);

        static std::vector<isim::Block *> tree_blocks =
            Tree::place_tree(isim::Point3(0, 0, 0), 5);

        static const std::vector<const isim::Object *> objects = []() {
            std::vector<const isim::Object *> ptrs;
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
                    ptrs.push_back(
                        isim::Block::new_grass(isim::Point3(x, -1, z)));
                }
            }

            return ptrs;
        }();

        static isim::PointLight main_light(isim::Point3(-5, 10, -5),
                                           isim::Color{ 255, 255, 255 });

        static const std::vector<const isim::Light *> lights = { &main_light };

        static Scene scene(objects, lights, { &camera }, Color::sky_blue,
                           Color::black);

        return &scene;
    }

    Scene *water_lake_scene()
    {
        static isim::Camera camera(isim::Point3(7, 12, -14),
                                   isim::Point3(0, 0, 2),
                                   isim::Vector3(0, 1, 0), 60, 45, 1);

        static const std::vector<const isim::Object *> objects = []() {
            std::vector<const isim::Object *> scene_objects;

            const int lake_radius = 3;
            const int lake_size = (lake_radius * 2) + 1;
            std::vector<std::vector<isim::Block *>> water_blocks(
                lake_size, std::vector<isim::Block *>(lake_size, nullptr));

            // Build a larger lake with a grass rim and a grass base.
            for (int x = -lake_radius; x <= lake_radius; x++)
            {
                for (int z = -lake_radius; z <= lake_radius; z++)
                {
                    isim::Block *water =
                        Block::new_water(isim::Point3(x, 0, z));
                    water_blocks[x + lake_radius][z + lake_radius] = water;
                    scene_objects.push_back(water);
                    scene_objects.push_back(
                        Block::new_grass(isim::Point3(x, -1, z)));
                }
            }

            // little culling implementation
            // will have to make it automatic later, for this scene, it works !
            for (int i = 0; i < lake_size; i++)
            {
                for (int j = 0; j < lake_size; j++)
                {
                    isim::Block *current = water_blocks[i][j];
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
            std::vector<isim::Block *> tree_blocks =
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
                                           isim::Color{ 255, 255, 255 });

        static const std::vector<const isim::Light *> lights = { &main_light };

        static Scene scene(objects, lights, { &camera }, Color::sky_blue,
                           Color::black);

        return &scene;
    }

    Scene *triangle()
    {
        static isim::Camera camera(isim::Point3(0, 0, -3),
                                   isim::Point3(0, 0, 0),
                                   isim::Vector3(0, 1, 0), 60, 45, 1);

        static isim::Uniform_texture_material peach_mat(isim::Color::peach_pink,
                                                        1, 0.1, 100.0f);
        static isim::Triangle Vertex(isim::Point3(-1, 0, 0),
                                     isim::Point3(1, 0, 0),
                                     isim::Point3(0, 1, 0), &peach_mat);

        static const std::vector<const isim::Object *> objects = { &Vertex };

        static isim::PointLight main_light(isim::Point3(-5, 1.5, 0),
                                           isim::Color{ 255, 255, 255 });

        static const std::vector<const isim::Light *> lights = { &main_light };

        static Scene scene(objects, lights, { &camera }, Color::black,
                           Color::black);

        return &scene;
    }

    Scene *teacher_shape()
    {
        static isim::Camera camera(isim::Point3(0, 0, -3),
                                   isim::Point3(0, 0, 0),
                                   isim::Vector3(0, 1, 0), 60, 45, 1);

        static isim::Uniform_texture_material peach_mat(isim::Color::peach_pink,
                                                        1, 0.1, 100.0f);

        static std::vector<std::vector<int>> pointsCords = {
            { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1 },
            { 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1 },
            { 3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1 },
            { 3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1 },
            { 9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1 },
            { 1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1 },
            { 9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1 },
            { 2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1 },
            { 8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1 },
            { 9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1 },
            { 4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1 },
            { 3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1 },
            { 1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1 },
            { 4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1 },
            { 4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1 },
            { 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1 },
            { 1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1 },
            { 5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1 },
            { 2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1 },
            { 9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1 },
            { 0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1 },
            { 2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1 },
            { 10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1 },
            { 4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1 },
            { 5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1 },
            { 5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1 },
            { 9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1 },
            { 0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1 },
            { 1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1 },
            { 10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1 },
            { 8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1 },
            { 2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1 },
            { 7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1 },
            { 9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1 },
            { 2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1 },
            { 11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1 },
            { 9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1 },
            { 5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0 },
            { 11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0 },
            { 11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1 },
            { 1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1 },
            { 9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1 },
            { 5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1 },
            { 2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1 },
            { 0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1 },
            { 5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1 },
            { 6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1 },
            { 0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1 },
            { 3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1 },
            { 6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1 },
            { 5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1 },

            { 4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1 },
            { 1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1 },
            { 10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1 },
            { 6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1 },
            { 1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1 },
            { 8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1 },
            { 7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9 },
            { 3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1 },
            { 5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1 },
            { 0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1 },
            { 9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6 },
            { 8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1 },
            { 5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11 },
            { 0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7 },
            { 6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1 },
            { 10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1 },
            { 10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1 },
            { 8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1 },
            { 1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1 },
            { 3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1 },
            { 0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1 },
            { 10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1 },
            { 0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1 },
            { 3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1 },
            { 6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1 },
            { 9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1 },
            { 8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1 },
            { 3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1 },
            { 6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1 },
            { 0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1 },
            { 10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1 },
            { 10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1 },
            { 1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1 },
            { 2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9 },
            { 7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1 },
            { 7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1 },
            { 2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7 },
            { 1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11 },
            { 11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1 },
            { 8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6 },
            { 0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1 },
            { 7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1 },
            { 10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1 },
            { 2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1 },
            { 6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1 },
            { 7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1 },
            { 2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1 },
            { 1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1 },
            { 10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1 },
            { 10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1 },
            { 0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1 },
            { 7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1 },
            { 6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1 },
            { 8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1 },
            { 9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1 },
            { 6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1 },
            { 1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1 },
            { 4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1 },
            { 10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3 },
            { 8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1 },
            { 0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1 },
            { 1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1 },
            { 8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1 },
            { 10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1 },
            { 4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3 },
            { 10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1 },
            { 5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1 },
            { 11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1 },
            { 9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1 },
            { 6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1 },
            { 7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1 },
            { 3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6 },
            { 7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1 },
            { 9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1 },
            { 3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1 },
            { 6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8 },
            { 9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1 },
            { 1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4 },
            { 4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10 },
            { 7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1 },
            { 6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1 },
            { 3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1 },
            { 0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1 },
            { 6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1 },
            { 1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1 },
            { 0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10 },
            { 11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5 },
            { 6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1 },
            { 5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1 },
            { 9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1 },
            { 1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8 },
            { 1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6 },
            { 10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1 },
            { 0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1 },
            { 5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1 },
            { 10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1 },
            { 11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1 },
            { 0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1 },
            { 9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1 },
            { 7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2 },
            { 2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1 },
            { 8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1 },
            { 9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1 },
            { 9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2 },
            { 1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1 },
            { 9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1 },
            { 9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1 },
            { 5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1 },
            { 0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1 },
            { 10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4 },
            { 2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1 },
            { 0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11 },
            { 0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5 },
            { 9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1 },
            { 5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1 },
            { 3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9 },
            { 5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1 },
            { 8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1 },
            { 0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1 },
            { 9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1 },
            { 0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1 },
            { 1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1 },
            { 3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4 },
            { 4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1 },
            { 9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3 },
            { 11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1 },
            { 11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1 },
            { 2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1 },
            { 9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7 },
            { 3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10 },
            { 1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1 },
            { 4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1 },
            { 4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1 },
            { 0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1 },
            { 3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1 },
            { 3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1 },
            { 0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1 },
            { 9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1 },
            { 1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { 0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
            { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
        };

        static std::vector<Point3> points;

        for (size_t i = 0; i < pointsCords.size(); i++)
        {
            points.emplace_back(Point3(static_cast<double>(pointsCords[i][0]),
                                       static_cast<double>(pointsCords[i][1]),
                                       static_cast<double>(pointsCords[i][2])));
        }

        static isim::Blob blob(1.0f, points, Point3(0, 0, 0), 0.2f, &peach_mat);

        static isim::PointLight main_light(isim::Point3(-5, 1.5, 0),
                                           isim::Color{ 255, 255, 255 });

        static const std::vector<const isim::Light *> lights = { &main_light };

        static const std::vector<const Object *> vertices =
            blob.marching_cubes();

        std::cout << "Vertices count: " << vertices.size() << std::endl;

        static Scene scene(vertices, lights, { &camera }, Color::black,
                           Color::black);

        return &scene;
    }

} // namespace isim
