#include <cmath>
#include <iostream>
#include <vector>

#include "scenes/scenes.hh"

int main()
{
    std::cout << "Rendering scene..." << std::endl;

    // isim::Scene *scene = isim::test_block_texure(
    //     "textures/grass/grass_block_top.png", "textures/grass/dirt.png",
    //     "textures/grass/grass_block_side.png");

    isim::Scene *scene = isim::teacher_shape();

    std::cout << "Scene rendered, saving image..." << std::endl;

    std::vector<isim::Image> image =
        scene->render_all_cameras(isim::Resolution::SmallSquare(), false);

    for (size_t i = 0; i < image.size(); i++)
    {
        std::string filename = "output_forme_prof.png";
        image[i].save(filename.c_str());
        std::cout << "Scene rendered and saved to " << filename << std::endl;
    }

    return 0;
}