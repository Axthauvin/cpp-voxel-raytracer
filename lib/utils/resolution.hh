#pragma once

#include <cstddef>

namespace isim
{
    class Resolution
    {
    public:
        Resolution() = default;
        Resolution(size_t width, size_t height)
            : width(width)
            , height(height)
        {}

        size_t width;
        size_t height;

        static const Resolution HD()
        {
            return { 1280, 720 };
        }
        static const Resolution FullHD()
        {
            return { 1920, 1080 };
        }
        static const Resolution UHD()
        {
            return { 3840, 2160 };
        }
        static const Resolution Cinema4K()
        {
            return { 4096, 2160 };
        }

        static const Resolution SmallSquare()
        {
            return { 800, 800 };
        }

        static const Resolution MediumSquare()
        {
            return { 1600, 1600 };
        }

        static const Resolution LargeSquare()
        {
            return { 3200, 3200 };
        }

        static const Resolution SmallHD()
        {
            return { 960, 540 };
        }

        static const Resolution Custom(size_t width, size_t height)
        {
            return { width, height };
        }
    };
}; // namespace isim