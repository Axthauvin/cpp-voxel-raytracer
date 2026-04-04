#include "scene.hh"

#include <cstddef>

#include "renderer.hh"
#include "utils/resolution.hh"

namespace isim
{
  Image Scene::render(size_t cameraIndex, Resolution res, bool raytracing) const
  {
    return render(*cameras.at(cameraIndex), res, raytracing);
  }

  Image
  Scene::render(const Camera camera, Resolution res, bool raytracing) const
  {
    Renderer renderer(voxelGrid, lights, backgroundColor, sceneAmbiant,
                      raytracing);
    return renderer.render(camera, res.width, res.height);
  }

  std::vector<Image> Scene::render_all_cameras(Resolution res,
                                               bool raytracing) const
  {
    std::vector<Image> images;
    for (const auto* camera : cameras)
      {
        images.push_back(render(*camera, res, raytracing));
      }
    return images;
  }
} // namespace isim
