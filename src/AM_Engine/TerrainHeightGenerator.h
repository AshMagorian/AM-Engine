#pragma once
#include <random>
#include <vector>
class TerrainHeightGenerator
{
private:
	TerrainHeightGenerator() {}

	float m_amplitude = 4.0f;
	int m_octaves = 3;
	float m_roughness = 0.3f;

	int m_seed = 0;

	std::vector<float> m_heights;
	int m_multithreading_numberOfThreads = 0;
	int m_multithreading_blockHeight = 0;

	void Init_Impl();
	float GenerateHeight_Impl(const int& x, const int& z, const int& vertexCount, const int& gridX, const int& gridZ);
	void GenerateHeights_Impl(std::vector<float> &heights, const int& vertexCount, const int& gridX, const int& gridZ);
	void GenerateHeightsBlock(int blockNumber, const int& vertexCount, const int& gridX, const int& gridZ);
	float GetInterpolatedNoise(float x, float z);
	float Interpolate(float a, float b, float blend);
	float GetSmoothNoise(int x, int z);
	float GetNoise(int x, int z);

public:
	TerrainHeightGenerator(const TerrainHeightGenerator&) = delete;
	static TerrainHeightGenerator& Get()
	{
		static TerrainHeightGenerator instance;
		return instance;
	}

	static void Init() { return Get().Init_Impl(); }
	static float GenerateHeight(const int& x, const int& z, const int& vertexCount, const int& gridX, const int& gridZ) { return Get().GenerateHeight_Impl(x, z, vertexCount, gridX, gridZ); }
	static void GenerateHeights(std::vector<float> &heights, const int& vertexCount, const int& gridX, const int& gridZ) { return Get().GenerateHeights_Impl(heights, vertexCount, gridX, gridZ); }
	static float GetAmplitude() { return Get().m_amplitude; }
};