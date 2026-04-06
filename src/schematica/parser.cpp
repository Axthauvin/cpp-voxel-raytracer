#include "parser.hh"
#include <bit>
#include <cctype>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <zlib.h>

namespace isim
{

  std::string normalizeBlockName(const std::string& raw_name)
  {
    std::string base_name = raw_name;
    const size_t props_pos = base_name.find('[');
    if (props_pos != std::string::npos)
      {
        base_name = base_name.substr(0, props_pos);
      }

    const size_t ns_pos = base_name.find(':');
    if (ns_pos != std::string::npos)
      {
        base_name = base_name.substr(ns_pos + 1);
      }

    for (char& c : base_name)
      {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
      }

    return base_name;
  }

  std::vector<int32_t> decodeVarIntBlockData(const uint8_t* bytes,
                                             size_t byte_count)
  {
    std::vector<int32_t> values;
    size_t i = 0;

    while (i < byte_count)
      {
        int32_t value = 0;
        int shift = 0;
        int consumed = 0;

        while (true)
          {
            if (i >= byte_count)
              {
                throw std::runtime_error(
                  "BlockData tronque: varint incomplet.");
              }

            const uint8_t byte = bytes[i++];
            value |= static_cast<int32_t>(byte & 0x7F) << shift;
            shift += 7;
            consumed++;

            if ((byte & 0x80) == 0)
              {
                break;
              }

            if (consumed >= 5)
              {
                throw std::runtime_error(
                  "BlockData invalide: varint trop long.");
              }
          }

        values.push_back(value);
      }

    return values;
  }

  // hi johan
  // if you are reading this, i am very sorry for the code you are about to read
  // i had to map each minecraft block name to a texture, and there are a lot of blocks in minecraft, so i had to write a lot of if statements, and i am very sorry for that
  // that way when you load a schematic, if the block type is recognized, it will use the correct texture
  Block makeBlockFromName(const std::string& palette_name,
                          const Point3& position,
                          bool replace_unknown_with_dirt)
  {
    const std::string normalized = normalizeBlockName(palette_name);

    if (normalized == "grass_block")
      {
        return Block::grass(position);
      }
    if (normalized == "dirt" || normalized == "coarse_dirt"
        || normalized == "podzol")
      {
        return Block::dirt(position);
      }
    if (normalized == "sand" || normalized == "red_sand")
      {
        return Block::sand(position);
      }
    if (normalized == "stone" || normalized == "cobblestone"
        || normalized == "andesite" || normalized == "diorite"
        || normalized == "granite")
      {
        return Block::stone(position);
      }
    if (normalized == "water")
      {
        return Block::water(position);
      }
    if (normalized == "snow" || normalized == "snow_block")
      {
        return Block::snow(position);
      }
    if (normalized == "oak_log" || normalized == "spruce_log"
        || normalized == "birch_log" || normalized == "jungle_log"
        || normalized == "acacia_log" || normalized == "dark_oak_log"
        || normalized == "mangrove_log" || normalized == "cherry_log"
        || normalized == "oak_wood")
      {
        return Block::log(position);
      }
    if (normalized == "oak_leaves" || normalized == "spruce_leaves"
        || normalized == "birch_leaves" || normalized == "jungle_leaves"
        || normalized == "acacia_leaves" || normalized == "dark_oak_leaves"
        || normalized == "mangrove_leaves" || normalized == "cherry_leaves")
      {
        return Block::leaves(position);
      }

    if (Block::texture_exists(normalized))
      {
        return Block::block_from_name(normalized, position);
      }

    if (replace_unknown_with_dirt)
      {
        return Block::dirt(position);
      }

    throw std::runtime_error("Bloc inconnu dans la palette: " + palette_name);
  }

  std::vector<uint8_t> SchematicaParser::load(const std::string& filepath)
  {
    // we use zlib to read the compressed .schem file
    gzFile file = gzopen(filepath.c_str(), "rb");
    if (!file)
      {
        std::cerr << "Erreur: Impossible d'ouvrir le fichier " << filepath
                  << std::endl;
        return {};
      }

    std::vector<uint8_t> buffer;
    uint8_t tempChunk[8192];
    int bytesRead = 0;

    while ((bytesRead = gzread(file, tempChunk, sizeof(tempChunk))) > 0)
      {
        buffer.insert(buffer.end(), tempChunk, tempChunk + bytesRead);
      }

    gzclose(file);
    return buffer;
  }

  uint16_t SchematicaParser::readU16_BigEndian(const uint8_t* data,
                                               size_t& cursor)
  {
    uint16_t val = (data[cursor] << 8) | data[cursor + 1];
    cursor += 2;
    return val;
  }

  uint32_t SchematicaParser::readU32_BigEndian(const uint8_t* data,
                                               size_t& cursor)
  {
    uint32_t val = (data[cursor] << 24) | (data[cursor + 1] << 16)
      | (data[cursor + 2] << 8) | data[cursor + 3];
    cursor += 4;
    return val;
  }

  std::string SchematicaParser::readString()
  {
    if (cursor + 2 > data.size())
      {
        std::cerr << "Erreur: lecture String hors limites (longueur manquante)."
                  << std::endl;
        cursor = data.size();
        return {};
      }

    uint16_t string_length = readU16_BigEndian(data.data(), cursor);

    if (cursor + string_length > data.size())
      {
        std::cerr << "Erreur: lecture String hors limites (payload tronque)."
                  << std::endl;
        cursor = data.size();
        return {};
      }

    std::string str(reinterpret_cast<const char*>(&data[cursor]),
                    string_length);
    cursor += string_length;
    return str;
  }

  void SchematicaParser::parseCompound(const std::string& parent_compound_name)
  {
    while (cursor < data.size())
      {
        NBTType type = static_cast<NBTType>(data[cursor++]);

        if (type == NBTType::End)
          {
            return;
          }

        std::string name = readString();

        readTagPayload(type, name, parent_compound_name);
      }
  }

  void SchematicaParser::readTagPayload(NBTType type,
                                        const std::string& name,
                                        const std::string& parent_compound_name)
  {
    auto canRead = [&](size_t bytes) -> bool {
      if (cursor + bytes > data.size())
        {
          std::cerr << "Erreur: lecture NBT hors limites pour le tag [" << name
                    << "]" << std::endl;
          cursor = data.size();
          return false;
        }
      return true;
    };

    auto readU64_BigEndian = [&]() -> uint64_t {
      if (!canRead(8))
        {
          return 0;
        }

      uint64_t val = (static_cast<uint64_t>(data[cursor]) << 56)
        | (static_cast<uint64_t>(data[cursor + 1]) << 48)
        | (static_cast<uint64_t>(data[cursor + 2]) << 40)
        | (static_cast<uint64_t>(data[cursor + 3]) << 32)
        | (static_cast<uint64_t>(data[cursor + 4]) << 24)
        | (static_cast<uint64_t>(data[cursor + 5]) << 16)
        | (static_cast<uint64_t>(data[cursor + 6]) << 8)
        | static_cast<uint64_t>(data[cursor + 7]);
      cursor += 8;
      return val;
    };

    std::function<void(NBTType)> readUnnamedPayload;
    std::function<void()> readUnnamedCompound;

    readUnnamedCompound = [&]() {
      while (cursor < data.size())
        {
          if (!canRead(1))
            {
              return;
            }

          NBTType childType = static_cast<NBTType>(data[cursor++]);
          if (childType == NBTType::End)
            {
              return;
            }

          readString();
          readUnnamedPayload(childType);
        }
    };

    readUnnamedPayload = [&](NBTType elementType) {
      switch (elementType)
        {
        case NBTType::Byte:
          if (canRead(1))
            {
              cursor += 1;
            }
          break;
        case NBTType::Short:
          if (canRead(2))
            {
              cursor += 2;
            }
          break;
        case NBTType::Int:
        case NBTType::Float:
          if (canRead(4))
            {
              cursor += 4;
            }
          break;
        case NBTType::Long:
        case NBTType::Double:
          if (canRead(8))
            {
              cursor += 8;
            }
          break;
        case NBTType::ByteArray:
          {
            int32_t length =
              static_cast<int32_t>(readU32_BigEndian(data.data(), cursor));
            if (length < 0)
              {
                std::cerr << "Erreur: longueur negative dans ByteArray."
                          << std::endl;
                cursor = data.size();
                return;
              }
            if (canRead(static_cast<size_t>(length)))
              {
                cursor += static_cast<size_t>(length);
              }
            break;
          }
        case NBTType::String:
          readString();
          break;
        case NBTType::List:
          {
            if (!canRead(1))
              {
                return;
              }
            NBTType nestedType = static_cast<NBTType>(data[cursor++]);
            int32_t length =
              static_cast<int32_t>(readU32_BigEndian(data.data(), cursor));
            if (length < 0)
              {
                std::cerr << "Erreur: longueur negative dans List."
                          << std::endl;
                cursor = data.size();
                return;
              }
            for (int32_t i = 0; i < length && cursor < data.size(); ++i)
              {
                readUnnamedPayload(nestedType);
              }
            break;
          }
        case NBTType::Compound:
          readUnnamedCompound();
          break;
        case NBTType::IntArray:
          {
            int32_t length =
              static_cast<int32_t>(readU32_BigEndian(data.data(), cursor));
            if (length < 0)
              {
                std::cerr << "Erreur: longueur negative dans IntArray."
                          << std::endl;
                cursor = data.size();
                return;
              }
            size_t bytes = static_cast<size_t>(length) * 4;
            if (canRead(bytes))
              {
                cursor += bytes;
              }
            break;
          }
        case NBTType::LongArray:
          {
            int32_t length =
              static_cast<int32_t>(readU32_BigEndian(data.data(), cursor));
            if (length < 0)
              {
                std::cerr << "Erreur: longueur negative dans LongArray."
                          << std::endl;
                cursor = data.size();
                return;
              }
            size_t bytes = static_cast<size_t>(length) * 8;
            if (canRead(bytes))
              {
                cursor += bytes;
              }
            break;
          }
        case NBTType::End:
        default:
          break;
        }
    };

    switch (type)
      {
      case NBTType::Byte:
        {
          if (!canRead(1))
            {
              return;
            }
          int8_t val = data[cursor++];
          std::cout << "Byte [" << name << "] : " << (int)val << std::endl;
          break;
        }
      case NBTType::Short:
        {
          if (!canRead(2))
            {
              return;
            }
          int16_t val =
            static_cast<int16_t>(readU16_BigEndian(data.data(), cursor));

          if (name == "Width")
            {
              width = val;
            }
          else if (name == "Height")
            {
              height = val;
            }
          else if (name == "Length")
            {
              length = val;
            }

          std::cout << "Short [" << name << "] : " << val << std::endl;
          break;
        }
      case NBTType::Int:
        {
          if (!canRead(4))
            {
              return;
            }
          int32_t val =
            static_cast<int32_t>(readU32_BigEndian(data.data(), cursor));

          if (parent_compound_name == "Palette")
            {
              palette_by_id[val] = name;
            }
          else if (name == "Width")
            {
              width = val;
            }
          else if (name == "Height")
            {
              height = val;
            }
          else if (name == "Length")
            {
              length = val;
            }

          std::cout << "Int [" << name << "] : " << val << std::endl;
          break;
        }
      case NBTType::Long:
        {
          int64_t val = static_cast<int64_t>(readU64_BigEndian());
          std::cout << "Long [" << name << "] : " << val << std::endl;
          break;
        }
      case NBTType::Float:
        {
          if (!canRead(4))
            {
              return;
            }
          uint32_t raw = readU32_BigEndian(data.data(), cursor);
          float val = std::bit_cast<float>(raw);
          std::cout << "Float [" << name << "] : " << val << std::endl;
          break;
        }
      case NBTType::Double:
        {
          uint64_t raw = readU64_BigEndian();
          double val = std::bit_cast<double>(raw);
          std::cout << "Double [" << name << "] : " << val << std::endl;
          break;
        }
      case NBTType::String:
        {
          std::string val = readString();
          std::cout << "String [" << name << "] : " << val << std::endl;
          break;
        }
      case NBTType::Compound:
        {
          std::cout << "Compound [" << name << "] {" << std::endl;
          parseCompound(name);
          std::cout << "}" << std::endl;
          break;
        }
      case NBTType::ByteArray:
        {
          if (!canRead(4))
            {
              return;
            }
          int32_t length =
            static_cast<int32_t>(readU32_BigEndian(data.data(), cursor));
          if (length < 0)
            {
              std::cerr << "Erreur: ByteArray [" << name
                        << "] avec une longueur negative." << std::endl;
              cursor = data.size();
              return;
            }
          if (!canRead(static_cast<size_t>(length)))
            {
              return;
            }

          if (name == "BlockData")
            {
              const size_t start = cursor;
              block_palette_ids = decodeVarIntBlockData(
                &data[start], static_cast<size_t>(length));
            }

          cursor += static_cast<size_t>(length);
          break;
        }
      case NBTType::IntArray:
        {
          if (!canRead(4))
            {
              return;
            }
          int32_t length =
            static_cast<int32_t>(readU32_BigEndian(data.data(), cursor));
          if (length < 0)
            {
              std::cerr << "Erreur: IntArray [" << name
                        << "] avec une longueur negative." << std::endl;
              cursor = data.size();
              return;
            }
          size_t bytes = static_cast<size_t>(length) * 4;
          if (!canRead(bytes))
            {
              return;
            }
          cursor += bytes;
          break;
        }
      case NBTType::LongArray:
        {
          if (!canRead(4))
            {
              return;
            }
          int32_t length =
            static_cast<int32_t>(readU32_BigEndian(data.data(), cursor));
          if (length < 0)
            {
              std::cerr << "Erreur: LongArray [" << name
                        << "] avec une longueur negative." << std::endl;
              cursor = data.size();
              return;
            }
          size_t bytes = static_cast<size_t>(length) * 8;
          if (!canRead(bytes))
            {
              return;
            }
          cursor += bytes;
          break;
        }
      case NBTType::List:
        {
          if (!canRead(1 + 4))
            {
              return;
            }
          NBTType elementType = static_cast<NBTType>(data[cursor++]);
          int32_t list_length =
            static_cast<int32_t>(readU32_BigEndian(data.data(), cursor));
          if (list_length < 0)
            {
              std::cerr << "Erreur: List [" << name
                        << "] avec une longueur negative." << std::endl;
              cursor = data.size();
              return;
            }

          std::cout << "List [" << name << "] of type " << (int)elementType
                    << " with " << list_length << " elements" << std::endl;

          for (int32_t i = 0; i < list_length && cursor < data.size(); ++i)
            {
              if (name == "size")
                {
                  if (i == 0)
                    {
                      this->width = elementType == NBTType::Int
                        ? static_cast<int32_t>(
                            readU32_BigEndian(data.data(), cursor))
                        : static_cast<int32_t>(data[cursor++]);
                      std::cout << "Width set to " << width << std::endl;
                    }
                  else if (i == 1)
                    {
                      this->height = elementType == NBTType::Int
                        ? static_cast<int32_t>(
                            readU32_BigEndian(data.data(), cursor))
                        : static_cast<int32_t>(data[cursor++]);
                      std::cout << "Height set to " << height << std::endl;
                    }
                  else if (i == 2)
                    {
                      this->length = elementType == NBTType::Int
                        ? static_cast<int32_t>(
                            readU32_BigEndian(data.data(), cursor))
                        : static_cast<int32_t>(data[cursor++]);
                      std::cout << "Length set to " << this->length
                                << std::endl;
                    }
                }
              else
                readUnnamedPayload(elementType);
            }

          break;
        }

      default:
        std::cerr << "Type NBT non géré : " << (int)type << std::endl;
        break;
      }
  }

  void SchematicaParser::buildBlocksFromParsedData()
  {
    parsed_blocks.clear();

    std::cout << "Construction des blocs à partir des données parsées..."
              << std::endl;

    std::cout << "Dimensions: " << width << " x " << height << " x " << length
              << std::endl;

    if (width <= 0 || height <= 0 || length <= 0)
      {
        throw std::runtime_error(
          "Dimensions invalides ou absentes (Width/Height/Length). ");
      }

    const int64_t expected_count =
      static_cast<int64_t>(width) * height * length;
    if (expected_count <= 0)
      {
        throw std::runtime_error("Le nombre de blocs attendu est invalide.");
      }

    std::cout << "Nombre de blocs attendu: " << expected_count << std::endl;
    std::cout << "Nombre de blocs dans BlockData: " << block_palette_ids.size()
              << std::endl;

    if (static_cast<int64_t>(block_palette_ids.size()) < expected_count)
      {
        throw std::runtime_error(
          "BlockData incomplet: moins de blocs que prevu.");
      }

    parsed_blocks.reserve(static_cast<size_t>(expected_count));

    for (int y = 0; y < height; ++y)
      {
        for (int z = 0; z < length; ++z)
          {
            for (int x = 0; x < width; ++x)
              {
                const int64_t index =
                  (static_cast<int64_t>(y) * length + z) * width + x;
                const int32_t palette_id =
                  block_palette_ids[static_cast<size_t>(index)];

                auto palette_it = palette_by_id.find(palette_id);
                if (palette_it == palette_by_id.end())
                  {
                    if (replace_unknown_blocks_with_dirt)
                      {
                        parsed_blocks.push_back(Block::dirt(Point3(x, y, z)));
                        continue;
                      }

                    throw std::runtime_error("Palette ID introuvable: "
                                             + std::to_string(palette_id));
                  }

                const std::string normalized =
                  normalizeBlockName(palette_it->second);
                if (normalized == "air" || normalized == "cave_air"
                    || normalized == "void_air")
                  {
                    continue;
                  }

                parsed_blocks.push_back(
                  makeBlockFromName(palette_it->second, Point3(x, y, z),
                                    replace_unknown_blocks_with_dirt));
              }
          }
      }
  }

  SchematicaParser::Schematic
  SchematicaParser::parse(bool replace_unknown_with_dirt)
  {
    cursor = 0;
    width = 0;
    height = 0;
    length = 0;
    palette_by_id.clear();
    block_palette_ids.clear();
    parsed_blocks.clear();
    replace_unknown_blocks_with_dirt = replace_unknown_with_dirt;

    data = load(filename);
    if (data.empty())
      {
        std::cerr << "Erreur: Impossible de charger les données du fichier "
                  << filename << std::endl;
        return {0, 0, 0, {}};
      }

    std::cout << "Données chargées avec succès, taille: " << data.size()
              << " octets" << std::endl;

    if (cursor >= data.size())
      {
        std::cerr << "Format invalide: fichier NBT vide." << std::endl;
        return {0, 0, 0, {}};
      }

    NBTType rootType = static_cast<NBTType>(data[cursor++]);
    if (rootType != NBTType::Compound)
      {
        std::cerr << "Format invalide, la racine n'est pas un Compound."
                  << std::endl;
        return {0, 0, 0, {}};
      }

    std::string rootName = readString();

    parseCompound(rootName);

    buildBlocksFromParsedData();
    return {width, height, length, parsed_blocks};
  }
} // namespace isim