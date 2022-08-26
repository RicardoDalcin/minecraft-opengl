#ifndef _BLOCKDATABASE_H
#define _BLOCKDATABASE_H

#include <array>
#include <string>

#include <glm/glm.hpp>

const int STONE_SIDE = 0;
const int ANDESITE_SIDE = 1;
const int DIRT_SIDE = 2;

const int GRASS_SIDE = 3;
const int GRASS_TOP = 4;

const int SNOW_SIDE = 5;

const int PODZIL_TOP = 6;
const int PODZIL_SIDE = 7;

const int OAK_PLANK_SIDE = 8;
const int SPRUCE_PLANK_SIDE = 9;
const int BIRCH_PLANK_SIDE = 10;
const int JUNGLE_PLANK_SIDE = 11;
const int ACACIA_PLANK_SIDE = 12;
const int DARK_OAK_PLANK_SIDE = 13;

const int OAK_SAPPLING = 14;
const int SPRUCE_SAPPLING = 15;
const int BIRCH_SAPPLING = 16;
const int JUNGLE_SAPPLING = 17;
const int ACACIA_SAPPLING = 18;
const int DARK_OAK_SAPPLING = 19;

const int OAK_LOG_TOP = 20;
const int OAK_LOG_SIDE = 21;

const int SPRUCE_LOG_TOP = 22;
const int SPRUCE_LOG_SIDE = 23;

const int BIRCH_LOG_TOP = 24;
const int BIRCH_LOG_SIDE = 25;

const int JUNGLE_LOG_TOP = 26;
const int JUNGLE_LOG_SIDE = 27;

const int ACACIA_LOG_TOP = 28;
const int ACACIA_LOG_SIDE = 29;

const int DARK_OAK_LOG_TOP = 30;
const int DARK_OAK_LOG_SIDE = 31;

const int COBBLESTONE_SIDE = 32;
const int MOSSY_COBBLESTONE_SIDE = 33;

const int SAND_SIDE = 34;
const int RED_SAND_SIDE = 35;
const int GRAVEL_SIDE = 36;

const int COAL_ORE_SIDE = 37;
const int IRON_ORE_SIDE = 38;
const int LAPIS_ORE_SIDE = 39;
const int EMERALD_ORE_SIDE = 40;
const int GOLD_ORE_SIDE = 41;
const int REDSTONE_ORE_SIDE = 42;
const int DIAMOND_ORE_SIDE = 43;
const int QUARTZ_ORE_SIDE = 44;

const int COAL_BLOCK_SIDE = 45;
const int IRON_BLOCK_SIDE = 46;
const int LAPIS_BLOCK_SIDE = 47;
const int EMERALD_BLOCK_SIDE = 48;
const int GOLD_BLOCK_SIDE = 49;
const int REDSTONE_BLOCK_SIDE = 50;
const int DIAMOND_BLOCK_SIDE = 51;

const int OAK_LEAVES_SIDE = 52;
const int OAK_LEAVES_TOP = 53;
const int SPRUCE_LEAVES_SIDE = 54;
const int SPRUCE_LEAVES_TOP = 55;
const int BIRCH_LEAVES_SIDE = 56;
const int BIRCH_LEAVES_TOP = 57;
const int JUNGLE_LEAVES_SIDE = 58;
const int JUNGLE_LEAVES_TOP = 59;
const int ACACIA_LEAVES_SIDE = 60;
const int ACACIA_LEAVES_TOP = 61;
const int DARK_OAK_LEAVES_SIDE = 62;
const int DARK_OAK_LEAVES_TOP = 63;

const int BEDROCK_SIDE = 64;
const int OBSIDIAN_SIDE = 65;

const int STONE_BRICK_SIDE = 70;
const int MOSSY_STONE_BRICK_SIDE = 71;
const int CRACKED_STONE_BRICK_SIDE = 72;
const int CHISELED_STONE_BRICK_SIDE = 73;

const int CRAFTING_TABLE_TOP = 160;
const int CRAFTING_TABLE_FACE = 161;
const int CRAFTING_TABLE_SIDE = 162;

const int FURNACE_TOP = 163;
const int FURNACE_SIDE = 164;
const int FURNACE_FACE = 165;

const int WATER_TOP = 97;

struct TextureInfo
{
  int face;
  int side;
  int top;
  int bottom;
};

const TextureInfo AIR_TEXTURE = {-1, -1, -1, -1};
const TextureInfo STONE_TEXTURE = {STONE_SIDE, STONE_SIDE, STONE_SIDE, STONE_SIDE};
const TextureInfo ANDESITE_TEXTURE = {ANDESITE_SIDE, ANDESITE_SIDE, ANDESITE_SIDE, ANDESITE_SIDE};
const TextureInfo DIRT_TEXTURE = {DIRT_SIDE, DIRT_SIDE, DIRT_SIDE, DIRT_SIDE};

const TextureInfo GRASS_TEXTURE = {GRASS_SIDE, GRASS_SIDE, GRASS_TOP, DIRT_SIDE};
const TextureInfo SNOW_TEXTURE = {SNOW_SIDE, SNOW_SIDE, SNOW_SIDE, DIRT_SIDE};
const TextureInfo PODZIL_TEXTURE = {PODZIL_SIDE, PODZIL_SIDE, PODZIL_TOP, DIRT_SIDE};

const TextureInfo OAK_PLANK_TEXTURE = {OAK_PLANK_SIDE, OAK_PLANK_SIDE, OAK_PLANK_SIDE, OAK_PLANK_SIDE};
const TextureInfo SPRUCE_PLANK_TEXTURE = {SPRUCE_PLANK_SIDE, SPRUCE_PLANK_SIDE, SPRUCE_PLANK_SIDE, SPRUCE_PLANK_SIDE};
const TextureInfo BIRCH_PLANK_TEXTURE = {BIRCH_PLANK_SIDE, BIRCH_PLANK_SIDE, BIRCH_PLANK_SIDE, BIRCH_PLANK_SIDE};
const TextureInfo JUNGLE_PLANK_TEXTURE = {JUNGLE_PLANK_SIDE, JUNGLE_PLANK_SIDE, JUNGLE_PLANK_SIDE, JUNGLE_PLANK_SIDE};
const TextureInfo ACACIA_PLANK_TEXTURE = {ACACIA_PLANK_SIDE, ACACIA_PLANK_SIDE, ACACIA_PLANK_SIDE, ACACIA_PLANK_SIDE};
const TextureInfo DARK_OAK_PLANK_TEXTURE = {DARK_OAK_PLANK_SIDE, DARK_OAK_PLANK_SIDE, DARK_OAK_PLANK_SIDE, DARK_OAK_PLANK_SIDE};

const TextureInfo OAK_LOG_TEXTURE = {OAK_LOG_SIDE, OAK_LOG_SIDE, OAK_LOG_TOP, OAK_LOG_TOP};
const TextureInfo SPRUCE_LOG_TEXTURE = {SPRUCE_LOG_SIDE, SPRUCE_LOG_SIDE, SPRUCE_LOG_TOP, SPRUCE_LOG_TOP};
const TextureInfo BIRCH_LOG_TEXTURE = {BIRCH_LOG_SIDE, BIRCH_LOG_SIDE, BIRCH_LOG_TOP, BIRCH_LOG_TOP};
const TextureInfo JUNGLE_LOG_TEXTURE = {JUNGLE_LOG_SIDE, JUNGLE_LOG_SIDE, JUNGLE_LOG_TOP, JUNGLE_LOG_TOP};
const TextureInfo ACACIA_LOG_TEXTURE = {ACACIA_LOG_SIDE, ACACIA_LOG_SIDE, ACACIA_LOG_TOP, ACACIA_LOG_TOP};
const TextureInfo DARK_OAK_LOG_TEXTURE = {DARK_OAK_LOG_SIDE, DARK_OAK_LOG_SIDE, DARK_OAK_LOG_TOP, DARK_OAK_LOG_TOP};

const TextureInfo COBBLESTONE_TEXTURE = {COBBLESTONE_SIDE, COBBLESTONE_SIDE, COBBLESTONE_SIDE, COBBLESTONE_SIDE};
const TextureInfo MOSSY_COBBLESTONE_TEXUTRE = {MOSSY_COBBLESTONE_SIDE, MOSSY_COBBLESTONE_SIDE, MOSSY_COBBLESTONE_SIDE, MOSSY_COBBLESTONE_SIDE};

const TextureInfo SAND_TEXTURE = {SAND_SIDE, SAND_SIDE, SAND_SIDE, SAND_SIDE};
const TextureInfo RED_SAND_TEXTURE = {RED_SAND_SIDE, RED_SAND_SIDE, RED_SAND_SIDE, RED_SAND_SIDE};
const TextureInfo GRAVEL_TEXTURE = {GRAVEL_SIDE, GRAVEL_SIDE, GRAVEL_SIDE, GRAVEL_SIDE};

const TextureInfo COAL_ORE_TEXTURE = {COAL_ORE_SIDE, COAL_ORE_SIDE, COAL_ORE_SIDE, COAL_ORE_SIDE};
const TextureInfo IRON_ORE_TEXTURE = {IRON_ORE_SIDE, IRON_ORE_SIDE, IRON_ORE_SIDE, IRON_ORE_SIDE};
const TextureInfo LAPIS_ORE_TEXTURE = {LAPIS_ORE_SIDE, LAPIS_ORE_SIDE, LAPIS_ORE_SIDE, LAPIS_ORE_SIDE};
const TextureInfo EMERALD_ORE_TEXTURE = {EMERALD_ORE_SIDE, EMERALD_ORE_SIDE, EMERALD_ORE_SIDE, EMERALD_ORE_SIDE};
const TextureInfo GOLD_ORE_TEXTURE = {GOLD_ORE_SIDE, GOLD_ORE_SIDE, GOLD_ORE_SIDE, GOLD_ORE_SIDE};
const TextureInfo REDSTONE_ORE_TEXTURE = {REDSTONE_ORE_SIDE, REDSTONE_ORE_SIDE, REDSTONE_ORE_SIDE, REDSTONE_ORE_SIDE};
const TextureInfo DIAMOND_ORE_TEXTURE = {DIAMOND_ORE_SIDE, DIAMOND_ORE_SIDE, DIAMOND_ORE_SIDE, DIAMOND_ORE_SIDE};
const TextureInfo QUARTZ_ORE_TEXTURE = {QUARTZ_ORE_SIDE, QUARTZ_ORE_SIDE, QUARTZ_ORE_SIDE, QUARTZ_ORE_SIDE};

const TextureInfo COAL_BLOCK_TEXTURE = {COAL_BLOCK_SIDE, COAL_BLOCK_SIDE, COAL_BLOCK_SIDE, COAL_BLOCK_SIDE};
const TextureInfo IRON_BLOCK_TEXTURE = {IRON_BLOCK_SIDE, IRON_BLOCK_SIDE, IRON_BLOCK_SIDE, IRON_BLOCK_SIDE};
const TextureInfo LAPIS_BLOCK_TEXTURE = {LAPIS_BLOCK_SIDE, LAPIS_BLOCK_SIDE, LAPIS_BLOCK_SIDE, LAPIS_BLOCK_SIDE};
const TextureInfo EMERALD_BLOCK_TEXTURE = {EMERALD_BLOCK_SIDE, EMERALD_BLOCK_SIDE, EMERALD_BLOCK_SIDE, EMERALD_BLOCK_SIDE};
const TextureInfo GOLD_BLOCK_TEXTURE = {GOLD_BLOCK_SIDE, GOLD_BLOCK_SIDE, GOLD_BLOCK_SIDE, GOLD_BLOCK_SIDE};
const TextureInfo REDSTONE_BLOCK_TEXTURE = {REDSTONE_BLOCK_SIDE, REDSTONE_BLOCK_SIDE, REDSTONE_BLOCK_SIDE, REDSTONE_BLOCK_SIDE};
const TextureInfo DIAMOND_BLOCK_TEXTURE = {DIAMOND_BLOCK_SIDE, DIAMOND_BLOCK_SIDE, DIAMOND_BLOCK_SIDE, DIAMOND_BLOCK_SIDE};

const TextureInfo OAK_LEAVES_TEXTURE = {OAK_LEAVES_SIDE, OAK_LEAVES_SIDE, OAK_LEAVES_SIDE, OAK_LEAVES_SIDE};
const TextureInfo SPRUCE_LEAVES_TEXTURE = {SPRUCE_LEAVES_SIDE, SPRUCE_LEAVES_SIDE, SPRUCE_LEAVES_SIDE, SPRUCE_LEAVES_SIDE};
const TextureInfo BIRCH_LEAVES_TEXTURE = {BIRCH_LEAVES_SIDE, BIRCH_LEAVES_SIDE, BIRCH_LEAVES_SIDE, BIRCH_LEAVES_SIDE};
const TextureInfo JUNGLE_LEAVES_TEXTURE = {JUNGLE_LEAVES_SIDE, JUNGLE_LEAVES_SIDE, JUNGLE_LEAVES_SIDE, JUNGLE_LEAVES_SIDE};
const TextureInfo ACACIA_LEAVES_TEXTURE = {ACACIA_LEAVES_SIDE, ACACIA_LEAVES_SIDE, ACACIA_LEAVES_SIDE, ACACIA_LEAVES_SIDE};
const TextureInfo DARK_OAK_LEAVES_TEXTURE = {DARK_OAK_LEAVES_SIDE, DARK_OAK_LEAVES_SIDE, DARK_OAK_LEAVES_SIDE, DARK_OAK_LEAVES_SIDE};

const TextureInfo BEDROCK_TEXTURE = {BEDROCK_SIDE, BEDROCK_SIDE, BEDROCK_SIDE, BEDROCK_SIDE};
const TextureInfo OBSIDIAN_TEXTURE = {OBSIDIAN_SIDE, OBSIDIAN_SIDE, OBSIDIAN_SIDE, OBSIDIAN_SIDE};

const TextureInfo STONE_BRICK_TEXTURE = {STONE_BRICK_SIDE, STONE_BRICK_SIDE, STONE_BRICK_SIDE, STONE_BRICK_SIDE};
const TextureInfo MOSSY_STONE_BRICK_TEXTURE = {MOSSY_STONE_BRICK_SIDE, MOSSY_STONE_BRICK_SIDE, MOSSY_STONE_BRICK_SIDE, MOSSY_STONE_BRICK_SIDE};
const TextureInfo CRACKED_STONE_BRICK_TEXTURE = {CRACKED_STONE_BRICK_SIDE, CRACKED_STONE_BRICK_SIDE, CRACKED_STONE_BRICK_SIDE, CRACKED_STONE_BRICK_SIDE};
const TextureInfo CHISELED_STONE_BRICK_TEXTURE = {CHISELED_STONE_BRICK_SIDE, CHISELED_STONE_BRICK_SIDE, CHISELED_STONE_BRICK_SIDE, CHISELED_STONE_BRICK_SIDE};

const TextureInfo CRAFTING_TABLE_TEXTURE = {CRAFTING_TABLE_FACE, CRAFTING_TABLE_SIDE, CRAFTING_TABLE_TOP, OAK_PLANK_SIDE};
const TextureInfo FURNACE_TEXTURE = {FURNACE_FACE, FURNACE_SIDE, FURNACE_TOP, FURNACE_TOP};

const TextureInfo WATER_TEXTURE = {-1, -1, WATER_TOP, -1};

struct BlockBaseInformation
{
  std::string blockId;
  TextureInfo textureReference;
  bool isOpaque;
  bool isSolid;
};

struct BlockInformation
{
  std::string blockId;
  TextureInfo textureReference;
  std::array<glm::vec2, 36> textureCoordinates;
};

const int BLOCK_COUNT = 54;

const BlockBaseInformation BLOCK_ATLAS[BLOCK_COUNT] = {
    {"air", AIR_TEXTURE, false, false},
    {"stone", STONE_TEXTURE, true, true},
    {"andesite", ANDESITE_TEXTURE, true, true},
    {"dirt", DIRT_TEXTURE, true, true},
    {"grass", GRASS_TEXTURE, true, true},
    {"snow", SNOW_TEXTURE, true, true},
    {"podzil", PODZIL_TEXTURE, true, true},
    {"oak_plank", OAK_PLANK_TEXTURE, true, true},
    {"spruce_plank", SPRUCE_PLANK_TEXTURE, true, true},
    {"birch_plank", BIRCH_PLANK_TEXTURE, true, true},
    {"jungle_plank", JUNGLE_PLANK_TEXTURE, true, true},
    {"acacia_plank", ACACIA_PLANK_TEXTURE, true, true},
    {"dark_oak_plank", DARK_OAK_PLANK_TEXTURE, true, true},
    {"oak_log", OAK_LOG_TEXTURE, true, true},
    {"spruce_log", SPRUCE_LOG_TEXTURE, true, true},
    {"birch_log", BIRCH_LOG_TEXTURE, true, true},
    {"jungle_log", JUNGLE_LOG_TEXTURE, true, true},
    {"acacia_log", ACACIA_LOG_TEXTURE, true, true},
    {"dark_oak_log", DARK_OAK_LOG_TEXTURE, true, true},
    {"cobblestone", COBBLESTONE_TEXTURE, true, true},
    {"mossy_cobblestone", MOSSY_COBBLESTONE_TEXUTRE, true, true},
    {"sand", SAND_TEXTURE, true, true},
    {"red_sand", RED_SAND_TEXTURE, true, true},
    {"gravel", GRAVEL_TEXTURE, true, true},
    {"coal_ore", COAL_ORE_TEXTURE, true, true},
    {"iron_ore", IRON_ORE_TEXTURE, true, true},
    {"lapis_ore", LAPIS_ORE_TEXTURE, true, true},
    {"emerald_ore", EMERALD_ORE_TEXTURE, true, true},
    {"gold_ore", GOLD_ORE_TEXTURE, true, true},
    {"redstone_ore", REDSTONE_ORE_TEXTURE, true, true},
    {"diamond_ore", DIAMOND_ORE_TEXTURE, true, true},
    {"quartz_ore", QUARTZ_ORE_TEXTURE, true, true},
    {"coal_block", COAL_BLOCK_TEXTURE, true, true},
    {"iron_block", IRON_BLOCK_TEXTURE, true, true},
    {"lapis_block", LAPIS_BLOCK_TEXTURE, true, true},
    {"emerald_block", EMERALD_BLOCK_TEXTURE, true, true},
    {"gold_block", GOLD_BLOCK_TEXTURE, true, true},
    {"redstone_block", REDSTONE_BLOCK_TEXTURE, true, true},
    {"diamond_block", DIAMOND_BLOCK_TEXTURE, true, true},
    {"oak_leaves", OAK_LEAVES_TEXTURE, true, true},
    {"spruce_leaves", SPRUCE_LEAVES_TEXTURE, true, true},
    {"birch_leaves", BIRCH_LEAVES_TEXTURE, true, true},
    {"jungle_leaves", JUNGLE_LEAVES_TEXTURE, true, true},
    {"acacia_leaves", ACACIA_LEAVES_TEXTURE, true, true},
    {"dark_oak_leaves", DARK_OAK_LEAVES_TEXTURE, true, true},
    {"bedrock", BEDROCK_TEXTURE, true, true},
    {"obsidian", OBSIDIAN_TEXTURE, true, true},
    {"stone_brick", STONE_BRICK_TEXTURE, true, true},
    {"mossy_stone_brick", MOSSY_STONE_BRICK_TEXTURE, true, true},
    {"cracked_stone_brick", CRACKED_STONE_BRICK_TEXTURE, true, true},
    {"chiseled_stone_brick", CHISELED_STONE_BRICK_TEXTURE, true, true},
    {"crafting_table", CRAFTING_TABLE_TEXTURE, true, true},
    {"furnace", FURNACE_TEXTURE, true, true},
    {"water", WATER_TEXTURE, false, false},
};

const int AIR = 0;
const int STONE = 1;
const int ANDESITE = 2;
const int DIRT = 3;
const int GRASS = 4;
const int SNOW = 5;
const int PODZIL = 6;
const int OAK_PLANK = 7;
const int SPRUCE_PLANK = 8;
const int BIRCH_PLANK = 9;
const int JUNGLE_PLANK = 10;
const int ACACIA_PLANK = 11;
const int DARK_OAK_PLANK = 12;
const int OAK_LOG = 13;
const int SPRUCE_LOG = 14;
const int BIRCH_LOG = 15;
const int JUNGLE_LOG = 16;
const int ACACIA_LOG = 17;
const int DARK_OAK_LOG = 18;
const int COBBLESTONE = 19;
const int MOSSY_COBBLESTONE = 20;
const int SAND = 21;
const int RED_SAND = 22;
const int GRAVEL = 23;
const int COAL_ORE = 24;
const int IRON_ORE = 25;
const int LAPIS_ORE = 26;
const int EMERALD_ORE = 27;
const int GOLD_ORE = 28;
const int REDSTONE_ORE = 29;
const int DIAMOND_ORE = 30;
const int QUARTZ_ORE = 31;
const int COAL_BLOCK = 32;
const int IRON_BLOCK = 33;
const int LAPIS_BLOCK = 34;
const int EMERALD_BLOCK = 35;
const int GOLD_BLOCK = 36;
const int REDSTONE_BLOCK = 37;
const int DIAMOND_BLOCK = 38;
const int OAK_LEAVES = 39;
const int SPRUCE_LEAVES = 40;
const int BIRCH_LEAVES = 41;
const int JUNGLE_LEAVES = 42;
const int ACACIA_LEAVES = 43;
const int DARK_OAK_LEAVES = 44;
const int BEDROCK = 45;
const int OBSIDIAN = 46;
const int STONE_BRICK = 47;
const int MOSSY_STONE_BRICK = 48;
const int CRACKED_STONE_BRICK = 49;
const int CHISELED_STONE_BRICK = 50;
const int CRAFTING_TABLE = 51;
const int FURNACE = 52;
const int WATER = 53;

class BlockDatabase
{
private:
  BlockDatabase() {}

  static const int HORIZONTAL_TEXTURE_COUNT = 32;
  static const int VERTICAL_TEXTURE_COUNT = 11;

  static BlockInformation blockInformation[BLOCK_COUNT];

public:
  static void Initialize();

  static BlockInformation GetBlockInformation(std::string blockId);
  static BlockInformation GetBlockInformationIndex(int index);
};

#endif