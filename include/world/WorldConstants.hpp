#ifndef _WORLDCONSTANTS_H
#define _WORLDCONSTANTS_H

namespace WorldConstants
{
  const int CHUNK_SIZE = 16;
  const int CHUNK_HEIGHT = 256;

  const int WATER_LEVEL = CHUNK_SIZE;

  const int CHUNKS_PER_AXIS = 16;

  const float WORLD_SIZE = static_cast<float>(WorldConstants::CHUNKS_PER_AXIS) * WorldConstants::CHUNK_SIZE;

  const int MIN_HEIGHT = 2;
}

#endif