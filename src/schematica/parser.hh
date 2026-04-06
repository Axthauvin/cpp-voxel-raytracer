#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include "objects/minecraft.hh"

namespace isim
{

  class SchematicaParser
  {
    enum class NBTType : uint8_t
    {
      End = 0,
      Byte = 1,
      Short = 2,
      Int = 3,
      Long = 4,
      Float = 5,
      Double = 6,
      ByteArray = 7,
      String = 8,
      List = 9,
      Compound = 10,
      IntArray = 11,
      LongArray = 12
    };

  public:
    struct Schematic
    {
      int32_t width;
      int32_t height;
      int32_t length;
      std::vector<isim::Block> blocks;
    };

    SchematicaParser(const std::string& filename)
      : filename(filename)
    {}

    // Parses the .schem file and returns a vector of blocks with their positions and types.
    Schematic parse(bool replace_unknown_with_dirt = false);

  private:
    std::string readString();
    uint16_t readU16_BigEndian(const uint8_t* data, size_t& cursor);
    uint32_t readU32_BigEndian(const uint8_t* data, size_t& cursor);
    std::vector<uint8_t> load(const std::string& filepath);
    void parseCompound(const std::string& parent_compound_name);
    void readTagPayload(NBTType type,
                        const std::string& name,
                        const std::string& parent_compound_name);
    void buildBlocksFromParsedData();

    std::string filename;
    std::vector<uint8_t> data;
    size_t cursor = 0;

    std::vector<isim::Block> parsed_blocks;
    std::unordered_map<int32_t, std::string> palette_by_id;
    std::vector<int32_t> block_palette_ids;
    int32_t width = 0;
    int32_t height = 0;
    int32_t length = 0;
    bool replace_unknown_blocks_with_dirt = false;
  };
} // namespace isim