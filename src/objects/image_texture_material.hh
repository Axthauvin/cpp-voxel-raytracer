#pragma once

#include <memory>
#include <string>

#include "objects/texture_material.hh"
#include "utils/image.hh"
#include "utils/image_io.hh"

namespace isim
{
  class Image_texture_material : public isim::Texture_material
  {
  public:
    Image_texture_material(const char* filename,
                           const double kd = 1.0,
                           const double specularity = 0,
                           const double shininess = 250.0f,
                           const double ior = 1.0f);

    static const Image_texture_material*
    get_cached(const char* filename,
               const double kd = 1.0,
               const double specularity = 0,
               const double shininess = 250.0f,
               const double ior = 1.0f);

    Texture_properties getElementsAt(float u, float v) const override;

  private:
    std::shared_ptr<const Image> texture_;
    std::string cache_key_;
  };

} // namespace isim
