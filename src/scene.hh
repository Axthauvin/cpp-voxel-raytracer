//************************************************
//*                                              *
//*   TP 1&2    (c) 2017 J. FABRIZIO             *
//*                                              *
//*                               LRDE EPITA     *
//*                                              *
//************************************************

#ifndef SCENE_HH
#define SCENE_HH

#include <cstddef>
#include <vector>

#include "camera.hh"
#include "color.hh"
#include "light.hh"
#include "objects/object.hh"
#include "utils/image.hh"
#include "utils/resolution.hh"
#include "voxel_grid.hh"

namespace isim
{

  class Scene
  {
  public:
    Scene() = delete;
    ~Scene() = default;

    Scene(const std::vector<const Object*>& objects,
          const std::vector<const Light*>& lights,
          const std::vector<const Camera*>& cameras,
          const Color backgroundColor,
          const Color sceneAmbiant)
      : objects(objects)
      , lights(lights)
      , cameras(cameras)
      , backgroundColor(backgroundColor)
      , sceneAmbiant(sceneAmbiant)
      , voxelGrid(objects)
    {}

    std::vector<const Object*> objects;
    std::vector<const Light*> lights;
    std::vector<const Camera*> cameras;
    Color backgroundColor;
    Color sceneAmbiant;
    VoxelGrid voxelGrid;

    Image
    render(size_t cameraIndex, Resolution res, bool raytracing = true) const;
    Image
    render(const Camera camera, Resolution res, bool raytracing = true) const;
    std::vector<Image> render_all_cameras(Resolution res,
                                          bool raytracing = true) const;
  };
} // namespace isim
#endif /* SCENE_HH */
