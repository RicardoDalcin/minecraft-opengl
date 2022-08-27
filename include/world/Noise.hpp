#ifndef _NOISE_H
#define _NOISE_H

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>

class Noise
{
private:
  int m_Seed;
  int m_Octaves;
  float m_Amplitude;
  float m_Smoothness;
  float m_Roughness;
  float m_Offset;

public:
  Noise();
  Noise(int seed, int octaves, float amplitude, float smoothness, float roughness, float offset);
  ~Noise();

  void SetSeed(int seed);
  void SetOctaves(int octaves);
  void SetAmplitude(float amplitude);
  void SetSmoothness(float smoothness);
  void SetRoughness(float roughness);
  void SetOffset(float offset);

  int GetSeed() const { return m_Seed; };
  int GetOctaves() const { return m_Octaves; };
  float GetAmplitude() const { return m_Amplitude; };
  float GetSmoothness() const { return m_Smoothness; };
  float GetRoughness() const { return m_Roughness; };
  float GetOffset() const { return m_Offset; };

  float GetNoise(glm::vec2 position);
  float GetNoise(glm::vec2 blockPosition, glm::vec2 chunkPosition);
};

#endif