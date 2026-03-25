#ifndef TEXTURE_MATERIAL_HH
#define TEXTURE_MATERIAL_HH

#include "color.hh"

namespace isim
{

    typedef struct
    {
        Color ambient_color;
        double kd;
        double ks;
        double ns;
        double ior;
    } Texture_properties;

    class Texture_material
    {
    public:
        virtual ~Texture_material() = default;
        Texture_properties properties;

        virtual Texture_properties getElementsAt(float u, float v) const = 0;
    };

    class Uniform_texture_material : public Texture_material
    {
    public:
        Uniform_texture_material(const Color &color, const double kd = 1.0f,
                                 const double specularity = 0.0f,
                                 const double shininess = 32.0f,
                                 const double refraction = 1.0f)
        {
            this->properties.ambient_color = color;
            this->properties.kd = kd;
            this->properties.ks = specularity;
            this->properties.ns = shininess;
            this->properties.ior = refraction;
        }
        Texture_properties getElementsAt(float u, float v) const override;
    };
} // namespace isim
#endif /* TEXTURE_MATERIAL_HH */
