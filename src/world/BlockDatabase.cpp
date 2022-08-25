#include <glad/glad.h>

#include "world/BlockDatabase.hpp"

BlockInformation BlockDatabase::blockInformation[BLOCK_COUNT] = {};

void BlockDatabase::Initialize()
{
  for (int i = 0; i < BLOCK_COUNT; i++)
  {
    BlockBaseInformation blockBase = BLOCK_ATLAS[i];
    BlockInformation block;

    block.blockId = blockBase.blockId;
    block.textureReference = blockBase.textureReference;
    block.textureCoordinates = {};

    int textureIndexX = blockBase.textureReference.face % HORIZONTAL_TEXTURE_COUNT;
    int textureIndexY = ceil(blockBase.textureReference.face / HORIZONTAL_TEXTURE_COUNT);

    float unitX = 1.0f / HORIZONTAL_TEXTURE_COUNT;
    float unitY = 1.0f / VERTICAL_TEXTURE_COUNT;

    float textureZeroX = unitX * textureIndexX;
    float textureZeroY = unitY * ((VERTICAL_TEXTURE_COUNT - 1) - textureIndexY);

    float textureOneX = textureZeroX + unitX;
    float textureOneY = textureZeroY + unitY;

    std::array<glm::vec2, 36> textureCoords;
    block.textureCoordinates[0] = glm::vec2(textureOneX, textureOneY);
    block.textureCoordinates[1] = glm::vec2(textureZeroX, textureOneY);
    block.textureCoordinates[2] = glm::vec2(textureZeroX, textureZeroY);

    block.textureCoordinates[4] = glm::vec2(textureOneX, textureOneY);
    block.textureCoordinates[3] = glm::vec2(textureOneX, textureZeroY);
    block.textureCoordinates[5] = glm::vec2(textureZeroX, textureZeroY);

    textureIndexX = blockBase.textureReference.side % HORIZONTAL_TEXTURE_COUNT;
    textureIndexY = ceil(blockBase.textureReference.side / HORIZONTAL_TEXTURE_COUNT);

    textureZeroX = unitX * textureIndexX;
    textureZeroY = unitY * ((VERTICAL_TEXTURE_COUNT - 1) - textureIndexY);

    textureOneX = textureZeroX + unitX;
    textureOneY = textureZeroY + unitY;

    block.textureCoordinates[6] = glm::vec2(textureOneX, textureOneY);
    block.textureCoordinates[7] = glm::vec2(textureZeroX, textureOneY);
    block.textureCoordinates[8] = glm::vec2(textureZeroX, textureZeroY);

    block.textureCoordinates[10] = glm::vec2(textureOneX, textureOneY);
    block.textureCoordinates[9] = glm::vec2(textureOneX, textureZeroY);
    block.textureCoordinates[11] = glm::vec2(textureZeroX, textureZeroY);

    // back face must flip the texture
    block.textureCoordinates[14] = glm::vec2(textureOneX, textureOneY);
    block.textureCoordinates[15] = glm::vec2(textureZeroX, textureOneY);
    block.textureCoordinates[12] = glm::vec2(textureZeroX, textureZeroY);

    block.textureCoordinates[17] = glm::vec2(textureOneX, textureOneY);
    block.textureCoordinates[13] = glm::vec2(textureOneX, textureZeroY);
    block.textureCoordinates[16] = glm::vec2(textureZeroX, textureZeroY);

    block.textureCoordinates[18] = glm::vec2(textureOneX, textureOneY);
    block.textureCoordinates[19] = glm::vec2(textureZeroX, textureOneY);
    block.textureCoordinates[20] = glm::vec2(textureZeroX, textureZeroY);

    block.textureCoordinates[22] = glm::vec2(textureOneX, textureOneY);
    block.textureCoordinates[21] = glm::vec2(textureOneX, textureZeroY);
    block.textureCoordinates[23] = glm::vec2(textureZeroX, textureZeroY);

    textureIndexX = blockBase.textureReference.top % HORIZONTAL_TEXTURE_COUNT;
    textureIndexY = ceil(blockBase.textureReference.top / HORIZONTAL_TEXTURE_COUNT);

    textureZeroX = unitX * textureIndexX;
    textureZeroY = unitY * ((VERTICAL_TEXTURE_COUNT - 1) - textureIndexY);

    textureOneX = textureZeroX + unitX;
    textureOneY = textureZeroY + unitY;

    block.textureCoordinates[24] = glm::vec2(textureOneX, textureOneY);
    block.textureCoordinates[25] = glm::vec2(textureZeroX, textureOneY);
    block.textureCoordinates[26] = glm::vec2(textureZeroX, textureZeroY);

    block.textureCoordinates[28] = glm::vec2(textureOneX, textureOneY);
    block.textureCoordinates[27] = glm::vec2(textureOneX, textureZeroY);
    block.textureCoordinates[29] = glm::vec2(textureZeroX, textureZeroY);

    textureIndexX = blockBase.textureReference.bottom % HORIZONTAL_TEXTURE_COUNT;
    textureIndexY = ceil(blockBase.textureReference.bottom / HORIZONTAL_TEXTURE_COUNT);

    textureZeroX = unitX * textureIndexX;
    textureZeroY = unitY * ((VERTICAL_TEXTURE_COUNT - 1) - textureIndexY);

    textureOneX = textureZeroX + unitX;
    textureOneY = textureZeroY + unitY;

    block.textureCoordinates[30] = glm::vec2(textureOneX, textureOneY);
    block.textureCoordinates[31] = glm::vec2(textureZeroX, textureOneY);
    block.textureCoordinates[32] = glm::vec2(textureZeroX, textureZeroY);

    block.textureCoordinates[34] = glm::vec2(textureOneX, textureOneY);
    block.textureCoordinates[33] = glm::vec2(textureOneX, textureZeroY);
    block.textureCoordinates[35] = glm::vec2(textureZeroX, textureZeroY);

    blockInformation[i] = block;
  }
}

BlockInformation BlockDatabase::GetBlockInformation(std::string blockId)
{
  for (const auto &block : blockInformation)
  {
    if (blockId == block.blockId)
    {
      return block;
    }
  }
}

BlockInformation BlockDatabase::GetBlockInformationIndex(int index)
{
  return blockInformation[index];
}
