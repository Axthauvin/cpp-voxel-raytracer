#include "image_texture_material.hh"

#include <algorithm>
#include <memory>
#include <mutex>
#include <sstream>
#include <unordered_map>

#include "color.hh"
#include "objects/texture_material.hh"

namespace isim
{

  std::mutex g_texture_cache_mutex;
  std::unordered_map<std::string, std::shared_ptr<const Image>> g_image_cache;
  std::unordered_map<std::string, std::unique_ptr<Image_texture_material>>
    g_material_cache;

  std::shared_ptr<const Image> load_cached_image(const std::string& path)
  {
    auto it = g_image_cache.find(path);
    if (it != g_image_cache.end())
      {
        return it->second;
      }

    Image* raw = load_image(path.c_str());
    std::shared_ptr<const Image> image(raw);
    g_image_cache.emplace(path, image);
    return image;
  }

  std::string make_material_key(const char* filename,
                                double kd,
                                double specularity,
                                double shininess,
                                double ior)
  {
    std::ostringstream oss;
    oss << filename << "|" << kd << "|" << specularity << "|" << shininess
        << "|" << ior;
    return oss.str();
  }

  Image_texture_material::Image_texture_material(const char* filename,
                                                 const double kd,
                                                 const double specularity,
                                                 const double shininess,
                                                 const double ior)
  {
    this->properties.ambient_color = isim::Color::red;
    this->properties.kd = kd;
    this->properties.ks = specularity;
    this->properties.ns = shininess;
    this->properties.ior = ior;

    this->cache_key_ =
      make_material_key(filename, kd, specularity, shininess, ior);
    this->texture_ = load_cached_image(filename);
  }

  const Image_texture_material*
  Image_texture_material::get_cached(const char* filename,
                                     const double kd,
                                     const double specularity,
                                     const double shininess,
                                     const double ior)
  {
    const std::string key =
      make_material_key(filename, kd, specularity, shininess, ior);

    std::lock_guard<std::mutex> lock(g_texture_cache_mutex);

    auto it = g_material_cache.find(key);
    if (it != g_material_cache.end())
      {
        return it->second.get();
      }

    auto material = std::make_unique<Image_texture_material>(
      filename, kd, specularity, shininess, ior);
    const Image_texture_material* ptr = material.get();
    g_material_cache.emplace(key, std::move(material));
    return ptr;
  }

  Texture_properties Image_texture_material::getElementsAt(float u,
                                                           float v) const
  {
    float safe_u = std::clamp(u, 0.0f, 1.0f);
    float safe_v = std::clamp(v, 0.0f, 1.0f);

    const uint32_t x = static_cast<uint32_t>(safe_u * (texture_->width() - 1));
    const uint32_t y = static_cast<uint32_t>(safe_v * (texture_->height() - 1));

    const Color pixel = texture_->getPixel(x, y);

    Texture_properties materialForPixel;
    materialForPixel.kd = properties.kd;
    materialForPixel.ks = properties.ks;
    materialForPixel.ns = properties.ns;
    materialForPixel.ior = properties.ior;
    materialForPixel.ambient_color = pixel;
    return materialForPixel;
  }
} // namespace isim