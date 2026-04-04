#pragma once

#include <stdlib.h>

#include "color.hh"
#include "objects/texture_material.hh"
#include "point.hh"
#include "vector.hh"

namespace isim
{

  class Light
  {
  public:
    virtual ~Light() = default;
    Point3 position;
    virtual Color renderLight(const Texture_properties properties,
                              float intensity,
                              float specular_intensity,
                              Vector3 normal_at_collision,
                              Vector3 light_dir) const = 0;
    virtual Vector3 get_direction_from(const Point3& point) const = 0;

    static Vector3
    refract(const Vector3& I, const Vector3& N, const float& ior);
  };

  class PointLight : public Light
  {
  public:
    PointLight(const Point3& position, const Color& color)
      : color(color)
    {
      this->position = position;
    }

    Color color;
    Color renderLight(const Texture_properties properties,
                      float intensity,
                      float specular_intensity,
                      Vector3 normal_at_collision,
                      Vector3 light_dir) const override;
    Vector3 get_direction_from(const Point3& point) const override;
  };

  class AmbientLight : public Light
  {
  public:
    AmbientLight(const Color& color, const float intensity = 0.2f)
      : color(color)
      , intensity(intensity)
    {
      position = Point3(0, 0, 0);
    }
    Color color;
    float intensity;

    [[maybe_unused]] Color renderLight(
      [[maybe_unused]] const Texture_properties properties,
      [[maybe_unused]] float intensity = 0.2f,
      [[maybe_unused]] float specular_intensity = 0.0f,
      [[maybe_unused]] Vector3 normal_at_collision = Vector3(0, 0, 0),
      [[maybe_unused]] Vector3 light_dir = Vector3(0, 0, 0)) const override
    {
      return Color(
        static_cast<uint8_t>(properties.ambient_color.r * this->intensity),
        static_cast<uint8_t>(properties.ambient_color.g * this->intensity),
        static_cast<uint8_t>(properties.ambient_color.b * this->intensity),
        properties.ambient_color.a);
    }

    Vector3 get_direction_from(const Point3& point) const override
    {
      return Vector3(0, 0, 0); // Ambient light has no direction
    }
  };

  class DirectionalLight : public Light
  {
  public:
    DirectionalLight(const Vector3& direction,
                     const Color& color,
                     float intensity = 0.3f)
      : direction(direction)
      , color(color)
      , directional_intensity(intensity)
    {
      position = Point3(0, 0,
                        0); // Directional lights position is kind of useless
    }

    Vector3 direction;
    Color color;
    float directional_intensity = 0.3f;

    Color renderLight(const Texture_properties properties,
                      float intensity,
                      float specular_intensity,
                      Vector3 normal_at_collision,
                      Vector3 light_dir) const override;
    Vector3 get_direction_from(const Point3& point) const override;
  };

  const float AMBIENT_INTENSITY = 0.3f;
  const size_t MAX_REFLECTION_DEPTH = 6;
  const AmbientLight DEFAULT_AMBIENT_LIGHT(Color{255, 255, 255},
                                           AMBIENT_INTENSITY);

} // namespace isim
