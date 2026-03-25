//************************************************
//*                                              *
//*   TP 1&2    (c) 2017 J. FABRIZIO             *
//*                                              *
//*                               LRDE EPITA     *
//*                                              *
//************************************************

#ifndef IMAGE_HH
#define IMAGE_HH

#include <cstdint>

#include "color.hh"

#define IMAGE_NB_LEVELS 256
#define IMAGE_MAX_LEVEL 255
#define TL_IMAGE_ALIGNMENT 64

namespace isim
{

    class Image
    {
    public:
        Image() = default;
        Image(const uint32_t &w, const uint32_t &h);
        Image(const Image &other);
        Image &operator=(const Image &other);
        Image(Image &&other) noexcept;
        Image &operator=(Image &&other) noexcept;
        ~Image();

        uint32_t width() const;
        uint32_t height() const;
        Color *data() const;
        void save(const char *filename) const;
        void setPixel(uint32_t x, uint32_t y, const Color &color);
        Color getPixel(uint32_t x, uint32_t y) const;

    private:
        uint32_t w = 0;
        uint32_t h = 0;
        Color *pixels = nullptr;
    };

} // namespace isim
#endif /* IMAGE_HH */
