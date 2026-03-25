#pragma once
#include "objects/texture_material.hh"
#include "utils/image.hh"
#include "utils/image_io.hh"

namespace isim
{
    class Image_texture_material : public isim::Texture_material
    {
    public:
        Image_texture_material(const char *filename, const double kd = 1.0,
                               const double specularity = 0,
                               const double shininess = 250.0f)
        {
            this->properties.ambient_color = isim::Color::red;
            this->properties.kd = kd;
            this->properties.ks = specularity;
            this->properties.ns = shininess;
            this->properties.ior = 1.0f;

            texture = load_image(filename);
        }

        Image *texture;

        Texture_properties getElementsAt(float u, float v) const override;
    };

} // namespace isim
