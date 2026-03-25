#include "tree.hh"

namespace isim
{
    std::vector<Block *> Tree::place_tree(Point3 position,
                                          float trunk_height = 5)
    {
        std::vector<Block *> blocks;

        for (int i = 0; i < trunk_height; i++)
        {
            blocks.push_back(
                Block::new_log(Point3(position.x, position.y + i, position.z)));
        }

        int leafStart = trunk_height - 2;
        int leafEnd = trunk_height + 1;

        for (int y = leafStart; y <= leafEnd; y++)
        {
            // if johan is reading this, i tried to put 2 layers to the tree
            // to make it look like a real minecraft tree
            int radius = (y <= leafStart + 1) ? 2 : 1;

            for (int x = -radius; x <= radius; x++)
            {
                for (int z = -radius; z <= radius; z++)
                {
                    if (x == 0 && z == 0 && y < trunk_height)
                        continue;

                    bool isCorner =
                        (std::abs(x) == radius && std::abs(z) == radius);

                    if (isCorner) // this will cut the corner on 2 levels
                    {
                        if (radius == 2)
                            continue;

                        if (radius == 1 && y == leafEnd)
                            continue;
                    }

                    blocks.push_back(Block::new_leaves(Point3(
                        position.x + x, position.y + y, position.z + z)));
                }
            }
        }

        return blocks;
    }
} // namespace isim