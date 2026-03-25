#include "image_texture_material.hh"

#include <algorithm>
#include <iostream>

#include "color.hh"
#include "objects/texture_material.hh"

namespace isim
{
    Texture_properties Image_texture_material::getElementsAt(float u,
                                                             float v) const
    {
        float safe_u = std::clamp(u, 0.0f, 1.0f);
        float safe_v = std::clamp(v, 0.0f, 1.0f);

        const uint32_t x =
            static_cast<uint32_t>(safe_u * (texture->width() - 1));
        const uint32_t y =
            static_cast<uint32_t>(safe_v * (texture->height() - 1));

        const Color pixel = texture->getPixel(x, y);

        Texture_properties materialForPixel;
        materialForPixel.kd = properties.kd;
        materialForPixel.ks = properties.ks;
        materialForPixel.ns = properties.ns;
        materialForPixel.ior = properties.ior;
        materialForPixel.ambient_color = pixel;
        return materialForPixel;
    }
} // namespace isim