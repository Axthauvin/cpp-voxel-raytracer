#include "point.hh"
#include "scene.hh"

namespace isim
{

  struct SceneOutput
  {
    std::string scene_name;
    Scene* result;
  };

  SceneOutput grass_block_scene();
  SceneOutput test_block_texure(const char* top_texture_filename,
                                const char* bottom_texture_filename,
                                const char* side_texture_filename);
  SceneOutput minecraft_tree();
  SceneOutput water_lake_scene();
  SceneOutput triangle();
  SceneOutput teacher_shape();
  SceneOutput minecraft_terrain_scene(int width,
                                      int depth,
                                      double scale,
                                      size_t seed,
                                      int max_height = 12,
                                      size_t camera_count = 1);
  SceneOutput water_test();
  SceneOutput load_schematic(const std::string& filename,
                             bool replace_unknown_with_dirt = false,
                             float rotation_angle = 0,
                             Point3 camera_position = Point3(0, 0, 0),
                             Point3 camera_look_at = Point3(0, 0, 0));
} // namespace isim
