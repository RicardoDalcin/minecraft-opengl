#include "world/Noise.hpp"

#include "world/WorldConstants.hpp"

// Inicializa a noise sem parâmetros
Noise::Noise()
    : m_Seed(0),
      m_Octaves(1),
      m_Amplitude(1.0f),
      m_Smoothness(1.0f),
      m_Roughness(1.0f),
      m_Offset(0.0f)
{
}

// Inicializa a noise com os parâmetros passados
Noise::Noise(int seed, int octaves, float amplitude, float smoothness, float roughness, float offset)
    : m_Seed(seed),
      m_Octaves(octaves),
      m_Amplitude(amplitude),
      m_Smoothness(smoothness),
      m_Roughness(roughness),
      m_Offset(offset)
{
}

Noise::~Noise()
{
}

void Noise::SetSeed(int seed)
{
  m_Seed = seed;
}

void Noise::SetOctaves(int octaves)
{
  m_Octaves = octaves;
}

void Noise::SetAmplitude(float amplitude)
{
  m_Amplitude = amplitude;
}

void Noise::SetSmoothness(float smoothness)
{
  m_Smoothness = smoothness;
}

void Noise::SetRoughness(float roughness)
{
  m_Roughness = roughness;
}

void Noise::SetOffset(float offset)
{
  m_Offset = offset;
}

// Calcula a noise em uma posição 2D
float Noise::GetNoise(glm::vec2 position)
{
  float value = 0;
  float accumulatedAmplitude = 0;

  for (int i = 0; i < m_Octaves; i++)
  {
    float frequency = glm::pow(2.0f, i);
    float amplitude = glm::pow(m_Roughness, i);

    // Calcula as posições com base na frequência e suavização
    float x = position.x * frequency / m_Smoothness;
    float y = position.y * frequency / m_Smoothness;

    // Gera a noise simplex para a posição
    float noise = glm::simplex(glm::vec3(m_Seed + x, m_Seed + y, m_Seed));

    // Calcula o valor da noise com a amplitude
    noise = (noise + 1.0f) / 2.0f;
    value += noise * amplitude;

    accumulatedAmplitude += amplitude;
  }

  return value / accumulatedAmplitude;
}

// Calcula a noise em uma posição do mundo
float Noise::GetNoise(glm::vec2 blockPosition, glm::vec2 chunkPosition)
{
  glm::vec2 computedChunkPosition = glm::vec2(
      chunkPosition.x * WorldConstants::CHUNK_SIZE,
      chunkPosition.y * WorldConstants::CHUNK_SIZE);

  return GetNoise(blockPosition + computedChunkPosition);
}