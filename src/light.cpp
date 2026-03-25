#include "light.hh"

#include <algorithm>
#include <cmath>

#include "vector.hh"

namespace isim
{

    Color PointLight::renderLight(const Texture_properties properties,
                                  [[maybe_unused]] float intensity,
                                  float specular_intensity,
                                  Vector3 normal_at_collision,
                                  Vector3 light_dir) const
    {
        Color result;

        float diff = std::max(0.0f, normal_at_collision.dotProduct(light_dir));

        result.r = std::min(
            255,
            static_cast<int>(properties.ambient_color.r * diff * properties.kd)
                + static_cast<int>(properties.ks * specular_intensity
                                   * color.r));
        result.g = std::min(
            255,
            static_cast<int>(properties.ambient_color.g * diff * properties.kd)
                + static_cast<int>(properties.ks * specular_intensity
                                   * color.g));
        result.b = std::min(
            255,
            static_cast<int>(properties.ambient_color.b * diff * properties.kd)
                + static_cast<int>(properties.ks * specular_intensity
                                   * color.b));
        result.a = properties.ambient_color.a;

        return result;
    }

    Vector3 Light::refract(const Vector3 &I, const Vector3 &N, const float &ior)
    {
        // https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel.html
        float cosi = std::clamp(-1.0f, 1.0f, I.dotProduct(N));
        float etai = 1, etat = ior;
        Vector3 n = N;
        if (cosi < 0)
        {
            cosi = -cosi;
        }
        else
        {
            std::swap(etai, etat);
            n = -N;
        }
        float eta = etai / etat;
        float k = 1 - eta * eta * (1 - cosi * cosi);
        return k < 0 ? Vector3(-1, -1, -1, true)
                     : I * eta + n * (eta * cosi - sqrtf(k));
    }

} // namespace isim