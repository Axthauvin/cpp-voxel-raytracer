#include "color.hh"

#include <algorithm>

namespace isim
{

    uint8_t clamp_color(double value)
    {
        return std::min(255, std::max(0, static_cast<int>(value)));
    }

    Color Color::operator*(const Color &l) const
    {
        return Color((r * l.r) / 255, (g * l.g) / 255, (b * l.b) / 255,
                     (a * l.a) / 255);
    }

    Color Color::operator+(const Color &l) const
    {
        return Color(clamp_color(r + l.r), clamp_color(g + l.g),
                     clamp_color(b + l.b), clamp_color(a + l.a));
    }

    Color Color::operator*(double mult) const
    {
        return Color(clamp_color(r * mult), clamp_color(g * mult),
                     clamp_color(b * mult), clamp_color(a * mult));
    }

    Color Color::operator/(double div) const
    {
        if (div == 0)
            return Color(0, 0, 0, 255);
        return Color(clamp_color(r / div), clamp_color(g / div),
                     clamp_color(b / div), clamp_color(a / div));
    }

    std::ostream &Color::operator<<(std::ostream &os)
    {
        os << "Color(r=" << r << ", g=" << g << ", b=" << b << ")";
        return os;
    }
} // namespace isim