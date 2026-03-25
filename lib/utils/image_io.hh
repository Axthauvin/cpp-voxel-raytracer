//************************************************
//*                                              *
//*   TP 1&2    (c) 2017 J. FABRIZIO             *
//*                                              *
//*                               LRDE EPITA     *
//*                                              *
//************************************************

#ifndef IMAGE_IO_HH
#define IMAGE_IO_HH

#include "image.hh"

namespace isim
{

    bool save_image(const Image &image, const char *filename);
    Image *load_image(const char *filename);

} // namespace isim

#endif
