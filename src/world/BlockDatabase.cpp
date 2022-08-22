#include <glad/glad.h>

#include "world/BlockDatabase.hpp"

BlockInformation BlockDatabase::GetBlockInformation(std::string blockId)
{
  for (const auto &block : BLOCK_ATLAS)
  {
    if (blockId == block.blockId)
    {
      return block;
    }
  }
}

BlockInformation BlockDatabase::GetBlockInformationIndex(int index)
{
  return BLOCK_ATLAS[index];
}
