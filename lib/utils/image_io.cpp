//************************************************
//*                                              *
//*   TP 1&2    (c) 2017 J. FABRIZIO             *
//*                                              *
//*                               LRDE EPITA     *
//*                                              *
//************************************************

#include "image_io.hh"

#include <cstddef>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <vector>

#include "image.hh"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.hh"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.hh"

namespace isim
{

    struct struct_tga_header
    {
        uint8_t idl_length; // nombre de bits du champs d'identification de
                            // l'image  commençant au bit 12h
        uint8_t
            color_map_type; // 01h indique si le fichier TGA contient une
                            // palette  (contient 1 si c'est le cas , 0 sinon)
        uint8_t image_type; // 02h contient le code du type de l'image contenue
                            // dans le fichier TGA
        uint16_t cmap_start; // 03h defini la position de la premiére entrée de
                             // la colormap
        uint16_t cmap_length; // 05h nombre d'éléments de la colormap
        uint8_t
            cmap_depth; // 07h nombre de bits de chaque entrée de la colormap
        uint16_t x_offset; // 08h abscisse X de l' image
        uint16_t y_offset; // 0Ah ordonnée Y de l' image
        uint16_t width; // 0Ch Largeur de l'image en pixels
        uint16_t height; // 0Eh Hauteur de l'image en pixels
        uint8_t pixel_depth; // 10h nombre de bits par pixel
        uint8_t image_descriptor_alpha_channel_bits : 4; // 11h  contient 8 bits
                                                         // servant a décrire
                                                         // l'image
        uint8_t image_descriptor_image_origin : 2;
        uint8_t image_descriptor_unused : 2;
    } __attribute__((packed));

    typedef struct struct_tga_header tga_header;

    /**
     * Create a default tga header for 24 bits image without colormap, with
     * (0,0) as origin
     *
     */
    tga_header new_tga_header(int width, int height)
    {
        tga_header header;
        header.idl_length = 0; // nombre de bits du champs d'identification de
                               // l'image  commençant au bit 12h
        header.color_map_type =
            0; // 01h indique si le fichier TGA contient une palette  (contient
               // 1 si c'est le cas , 0 sinon)
        header.image_type = 2; // 02h contient le code du type de l'image
                               // contenue dans le fichier TGA
        header.cmap_start =
            0; // 03h defini la position de la premiére entrée de la colormap
        header.cmap_length = 0; // 05h nombre d'éléments de la colormap
        header.cmap_depth =
            0; // 07h nombre de bits de chaque entrée de la colormap
        header.x_offset = 0; // 08h abscisse X de l' image
        header.y_offset = 0; // 0Ah ordonnée Y de l' image
        header.width = width; // 0Ch Largeur de l'image en pixels
        header.height = height; // 0Eh Hauteur de l'image en pixels
        header.pixel_depth =
            24; // 10h nombre de bits par pixel
                // 11h  contient 8 bits servant a décrire l'image
        header.image_descriptor_unused = 0;
        header.image_descriptor_image_origin = 0;
        header.image_descriptor_alpha_channel_bits = 0;
        return header;
    }

    bool save_png(const Image &image, const char *filename)
    {
        std::vector<uint8_t> data(image.width() * image.height() * 4);
        for (std::size_t i = 0; i < image.width() * image.height(); i++)
        {
            Color pixel = image.data()[i];
            data[4 * i] = pixel.r;
            data[4 * i + 1] = pixel.g;
            data[4 * i + 2] = pixel.b;
            data[4 * i + 3] = pixel.a;
        }
        int result = stbi_write_png(filename, image.width(), image.height(), 4,
                                    data.data(), image.width() * 4);
        if (result == 0)
        {
            std::cerr << "ERROR: can not save " << filename << "!\n";
            return false;
        }
        return true;
    }

    bool is_png(const char *filename)
    {
        const char *dot = strrchr(filename, '.');
        if (dot == nullptr)
            return false;
        return strcmp(dot, ".png") == 0 || strcmp(dot, ".PNG") == 0;
    }

    bool save_image(const Image &image, const char *filename)
    {
        if (is_png(filename))
        {
            std::cerr << "INFO: Saving " << filename
                      << " as PNG because of its extension.\n";
            return save_png(image, filename);
        }

        tga_header header = new_tga_header(image.width(), image.height());
        uint8_t *buffer_bgr;
        // FILE *f = fopen(filename, "w");
        std::ofstream outfile(filename, std::ofstream::binary);
        // if (f==0) {
        if (!outfile.is_open())
        {
            std::cerr << "ERROR: can not open " << filename
                      << " for writing!\n";
            return false;
        }

        outfile.write((char *)(&header), sizeof(tga_header));
        // fwrite(&header, sizeof(tga_header), 1, f);

        const size_t image_size = image.width() * image.height() * 3;

        buffer_bgr = new uint8_t[image_size];
        for (std::size_t i = 0; i < image_size; i += 3)
        { // rgb2bgr

            Color pixel = image.data()[i / 3];

            buffer_bgr[i] = pixel.b;
            buffer_bgr[i + 1] = pixel.g;
            buffer_bgr[i + 2] = pixel.r;
        }
        // fwrite(buffer_bgr, 1, image_size, f);
        outfile.write((char *)(buffer_bgr), image_size);
        delete[] buffer_bgr;

        // fclose(f);
        outfile.close();
        return true;
    }

    Image *load_png(const char *filename)
    {
        int width, height, channels;
        uint8_t *data = stbi_load(filename, &width, &height, &channels, 4);
        if (data == nullptr)
        {
            std::cerr << "ERROR: can not load " << filename << "!\n";
            return nullptr;
        }

        Image *image = new Image(width, height);
        for (std::size_t i = 0; i < width * height; i++)
        {
            size_t reverse_i =
                width * height - 1 - i; // TGA files are stored in reverse
                                        // order, so we need to reverse
                                        // the index to get the correct
                                        // pixel
            Color pixel;
            pixel.r = data[4 * reverse_i];
            pixel.g = data[4 * reverse_i + 1];
            pixel.b = data[4 * reverse_i + 2];
            pixel.a = data[4 * reverse_i + 3];
            image->data()[i] = pixel;
        }
        stbi_image_free(data);
        return image;
    }

    Image *load_image(const char *filename)
    {
        tga_header header;
        Image *image;
        uint8_t *buffer_bgr;
        // FILE *f = fopen(filename, "r");
        std::ifstream input(filename, std::ofstream::binary);
        // if (f==0) {
        if (!input.is_open())
        {
            std::cerr << "ERROR: can not open " << filename
                      << " for reading!\n";
            return 0;
        }

        // if png load with load_png
        if (is_png(filename))
        {
            input.close();
            return load_png(filename);
        }

        input.read((char *)&header, sizeof(tga_header));
        if (!input)
        {
            // if (fread(&header, sizeof(tga_header), 1, f)!=1) {
            std::cerr << "ERROR: can not read " << filename << "!\n";
            input.close();
            return 0;
        }

        if (header.pixel_depth != 24)
        {
            std::cerr << "ERROR: Wrong image format (not 24bits)!\n";
            input.close();
            return 0;
        }

        image = new Image(header.width, header.height);

        size_t image_data_size = header.width * header.height * 3;

        buffer_bgr = new uint8_t[image_data_size];
        input.read((char *)buffer_bgr, image_data_size);
        if (!input)
        {
            // if (fread(buffer_bgr, 1, image_data_size,
            // f)!=(unsigned)image_data_size) {
            std::cerr << "ERROR: can not read image data!\n";
            delete image;
            input.close();
            return 0;
        }
        for (std::size_t i = 0; i < image_data_size; i += 3)
        { // bgr2rgb

            Color pixel;
            pixel.b = buffer_bgr[i];
            pixel.g = buffer_bgr[i + 1];
            pixel.r = buffer_bgr[i + 2];

            image->data()[i / 3] = pixel;
        }
        delete[] buffer_bgr;

        // fclose(f);
        input.close();
        return image;
    }

} // namespace isim
