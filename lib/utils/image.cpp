//************************************************
//*                                              *
//*   TP 1&2    (c) 2017 J. FABRIZIO             *
//*                                              *
//*                               LRDE EPITA     *
//*                                              *
//************************************************

#include "image.hh"

#include <cstdlib>
#include <cstring>

#include "image_io.hh"

namespace isim
{
    Image::Image(const uint32_t &w, const uint32_t &h)
        : w(w)
        , h(h)
    {
        pixels = static_cast<Color *>(malloc(w * h * sizeof(Color)));
    }

    Image::Image(const Image &other)
        : w(other.w)
        , h(other.h)
    {
        const size_t size = static_cast<size_t>(w) * h * sizeof(Color);
        pixels = size == 0 ? nullptr : static_cast<Color *>(malloc(size));
        if (pixels != nullptr && other.pixels != nullptr)
            std::memcpy(pixels, other.pixels, size);
    }

    Image &Image::operator=(const Image &other)
    {
        if (this == &other)
            return *this;

        const size_t size =
            static_cast<size_t>(other.w) * other.h * sizeof(Color);
        Color *new_pixels =
            size == 0 ? nullptr : static_cast<Color *>(malloc(size));

        if (new_pixels != nullptr && other.pixels != nullptr)
            std::memcpy(new_pixels, other.pixels, size);

        free(pixels);
        pixels = new_pixels;
        w = other.w;
        h = other.h;
        return *this;
    }

    Image::Image(Image &&other) noexcept
        : w(other.w)
        , h(other.h)
        , pixels(other.pixels)
    {
        other.w = 0;
        other.h = 0;
        other.pixels = nullptr;
    }

    Image &Image::operator=(Image &&other) noexcept
    {
        if (this == &other)
            return *this;

        free(pixels);
        w = other.w;
        h = other.h;
        pixels = other.pixels;

        other.w = 0;
        other.h = 0;
        other.pixels = nullptr;
        return *this;
    }

    Image::~Image()
    {
        free(pixels);
    }

    uint32_t Image::width() const
    {
        return w;
    }

    uint32_t Image::height() const
    {
        return h;
    }

    Color *Image::data() const
    {
        return pixels;
    }

    void Image::save(const char *filename) const
    {
        save_image(*this, filename);
    }

    void Image::setPixel(uint32_t x, uint32_t y, const Color &color)
    {
        if (x >= w || y >= h)
            return; // Out of bounds check

        pixels[y * w + x] = color;
    }

    Color Image::getPixel(uint32_t x, uint32_t y) const
    {
        return pixels[y * w + x];
    }

} // namespace isim
