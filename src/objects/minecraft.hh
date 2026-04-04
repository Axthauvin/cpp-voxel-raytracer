#pragma once

#include <iostream>
#include "color.hh"
#include "cuboid.hh"
#include "objects/texture_material.hh"
#include "point.hh"
#include "vector.hh"

namespace isim
{
  inline Texture_material* shared_water_material(bool enable_reflection)
  {
    static Uniform_texture_material reflective(Color::white, 1.0f, 0.2f, 255.0f,
                                               5.0f);
    static Uniform_texture_material non_reflective(Color::white, 1.0f, 0.0f,
                                                   255.0f, 1.0f);
    return enable_reflection ? static_cast<Texture_material*>(&reflective)
                             : static_cast<Texture_material*>(&non_reflective);
  }

  class Block : public Cuboid
  {
  public:
    // Face activation flags used by precomputed face culling.
    bool active_top = true;
    bool active_bottom = true;
    bool active_left = true;
    bool active_right = true;
    bool active_front = true;
    bool active_back = true;

    std::string getClassName() const override { return block_type; }

    std::string block_type;

    Point3 position() const
    {
      Point3 center;
      center.x = (max_point.x - min_point.x) / 2 + min_point.x;
      center.y = (max_point.y - min_point.y) / 2 + min_point.y;
      center.z = (max_point.z - min_point.z) / 2 + min_point.z;
      return center;
    }

    Block(const Point3& position,
          const char* top_texture_filename,
          const char* bottom_texture_filename,
          const char* side_texture_filename,
          std::string block_type = "generic")
      : Cuboid(position, 1, 1, 1, Color::white)
    {
      this->block_type = block_type;
      load_texture(top_texture_filename, bottom_texture_filename,
                   side_texture_filename);
    }

    Block(const Point3& position,
          const char* top_texture_filename,
          const char* bottom_texture_filename,
          const char* side_texture_filename,
          Texture_material* mat,
          std::string block_type = "generic")
      : Cuboid(position, 1, 1, 1, mat)
    {
      this->block_type = block_type;
      load_texture(top_texture_filename, bottom_texture_filename,
                   side_texture_filename);
    }

    Block(const Point3& position,
          float width,
          float height,
          float depth,
          const char* top_texture_filename,
          const char* bottom_texture_filename,
          const char* side_texture_filename,
          Texture_material* mat,
          std::string block_type = "generic")

      : Cuboid(position, width, height, depth, mat)
    {
      this->block_type = block_type;
      load_texture(top_texture_filename, bottom_texture_filename,
                   side_texture_filename);
    }

    float intersect(const Ray& ray) const override
    {
      const float t = Cuboid::intersect(ray);
      if (t <= 0.0f)
        return t;

      const Point3 hit_point(ray.origin.x + ray.direction.x * t,
                             ray.origin.y + ray.direction.y * t,
                             ray.origin.z + ray.direction.z * t);

      const Vector3 normal = Cuboid::getNormalAt(hit_point);

      // +x right, -x left, +y top, -y bottom, +z front, -z back.
      if ((normal.x > 0.5f && !active_right)
          || (normal.x < -0.5f && !active_left)
          || (normal.y > 0.5f && !active_top)
          || (normal.y < -0.5f && !active_bottom)
          || (normal.z > 0.5f && !active_front)
          || (normal.z < -0.5f && !active_back))
        {
          return -1.0f;
        }

      return t;
    }

    static Block grass(const Point3& position)
    {
      return Block(position, "textures/grass/grass_block_top.png",
                   "textures/grass/dirt.png",
                   "textures/grass/grass_block_side.png", "grass");
    }

    static Block* new_grass(const Point3& position)
    {
      return new Block(position, "textures/grass/grass_block_top.png",
                       "textures/grass/dirt.png",
                       "textures/grass/grass_block_side.png", "grass");
    }

    static Block leaves(const Point3& position)
    {
      return Block(position, "textures/tree/oak_leaves.png",
                   "textures/tree/oak_leaves.png",
                   "textures/tree/oak_leaves.png", "leaves");
    }

    static Block* new_leaves(const Point3& position)
    {
      return new Block(position, "textures/tree/oak_leaves.png",
                       "textures/tree/oak_leaves.png",
                       "textures/tree/oak_leaves.png", "leaves");
    }

    static Block log(const Point3& position)
    {
      return Block(position, "textures/tree/oak_log_top.png",
                   "textures/tree/oak_log_top.png", "textures/tree/oak_log.png",
                   "log");
    }

    static Block* new_log(const Point3& position)
    {
      return new Block(position, "textures/tree/oak_log_top.png",
                       "textures/tree/oak_log_top.png",
                       "textures/tree/oak_log.png", "log");
    }

    static Block water(const Point3& position)
    {
      // as water is not opaque, ill reduce the actual height of it so
      // that calculation will work !

      return Block(position + Vector3(0, 0.2f, 0), 1, 0.8f, 1,
                   "textures/water/water.png", "textures/water/water.png",
                   "textures/water/water.png", shared_water_material(true),
                   "water");
    }

    static Block* new_water(const Point3& position,
                            float width = 1,
                            float depth = 1,
                            bool enable_reflection = true)
    {
      return new Block(position + Vector3(0, 0.1f, 0), width, 0.7f, depth,
                       "textures/water/water.png", "textures/water/water.png",
                       "textures/water/water.png",
                       shared_water_material(enable_reflection), "water");
    }

    static Block dirt(const Point3& position)
    {
      return Block(position, "textures/grass/dirt.png",
                   "textures/grass/dirt.png", "textures/grass/dirt.png",
                   "dirt");
    }

    static Block* new_dirt(const Point3& position)
    {
      return new Block(position, "textures/grass/dirt.png",
                       "textures/grass/dirt.png", "textures/grass/dirt.png",
                       "dirt");
    }

    static Block sand(const Point3& position)
    {
      return Block(position, "textures/sand/sand.png", "textures/sand/sand.png",
                   "textures/sand/sand.png", "sand");
    }

    static Block* new_sand(const Point3& position)
    {
      return new Block(position, "textures/sand/sand.png",
                       "textures/sand/sand.png", "textures/sand/sand.png",
                       "sand");
    }

    static Block stone(const Point3& position)
    {
      return Block(position, "textures/stone/stone.png",
                   "textures/stone/stone.png", "textures/stone/stone.png",
                   "stone");
    }

    static Block* new_stone(const Point3& position)
    {
      return new Block(position, "textures/stone/stone.png",
                       "textures/stone/stone.png", "textures/stone/stone.png",
                       "stone");
    };

    static Block snow(const Point3& position)
    {
      return Block(position, "textures/snow/snow.png", "textures/snow/snow.png",
                   "textures/snow/snow.png", "snow");
    }

    static Block* new_snow(const Point3& position)
    {
      return new Block(position, "textures/snow/snow.png",
                       "textures/snow/snow.png", "textures/snow/snow.png",
                       "snow");
    }
  };

} // namespace isim