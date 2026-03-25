#include "scene.hh"

namespace isim
{
    Scene *sphere_scene();
    Scene *the_earth_scene();
    Scene *the_moon_scene();
    Scene *grass_block_scene();
    Scene *test_block_texure(const char *top_texture_filename,
                             const char *bottom_texture_filename,
                             const char *side_texture_filename);
    Scene *minecraft_tree();
    Scene *water_lake_scene();
    Scene *triangle();
    Scene *teacher_shape();
} // namespace isim
