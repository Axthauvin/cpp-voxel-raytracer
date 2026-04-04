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
                                      int max_height = 12,
                                      size_t camera_count = 1);
  SceneOutput water_test();
} // namespace isim
