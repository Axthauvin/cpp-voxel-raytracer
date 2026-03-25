#pragma once

#include <cstdint>
#include <ostream>

namespace isim
{
    uint8_t clamp_color(double value);
    class Color
    {
    public:
        Color() = default;
        Color(int r, int g, int b, int a = 255)
        {
            this->r = clamp_color(r);
            this->g = clamp_color(g);
            this->b = clamp_color(b);
            this->a = clamp_color(a);
        }

    public:
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;

        Color operator*(const Color &l) const;
        Color operator+(const Color &l) const;
        Color operator*(double mult) const;
        Color operator/(double div) const;

        // to string for debugging
        std::ostream &operator<<(std::ostream &os);

        static const Color red;
        static const Color green;
        static const Color blue;
        static const Color white;
        static const Color black;

        static const Color sky_blue;
        static const Color peach_pink;
        static const Color hot_pink;
        static const Color pale_pink;
        static const Color yellow;
        static const Color dark_pink;
    };

    // Initialize static color constants
    inline const Color Color::red = { 255, 0, 0 };
    inline const Color Color::green = { 0, 255, 0 };
    inline const Color Color::blue = { 0, 0, 255 };
    inline const Color Color::white = { 255, 255, 255 };
    inline const Color Color::black = { 0, 0, 0 };
    inline const Color Color::sky_blue = { 135, 206, 235 };
    inline const Color Color::peach_pink = { 255, 160, 150 };
    inline const Color Color::hot_pink = { 255, 120, 180 };
    inline const Color Color::pale_pink = { 255, 210, 200 };
    inline const Color Color::yellow = { 255, 240, 140 };
    inline const Color Color::dark_pink = { 210, 100, 140 };

} // namespace isim