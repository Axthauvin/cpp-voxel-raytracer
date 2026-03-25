#include "texture_material.hh"

namespace isim
{

    // as the teacher explained (hope i understood well), uniform has hardcoded
    // properties, so we can ignore the u and v parameters
    Texture_properties
    Uniform_texture_material::getElementsAt([[maybe_unused]] float u,
                                            [[maybe_unused]] float v) const
    {
        return properties;
    }
} // namespace isim