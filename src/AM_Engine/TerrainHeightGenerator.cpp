#include "TerrainHeightGenerator.h"

#include <random>
#include <iostream>
#include <thread>
#include <list>
#include <math.h>   
#define PI  3.14159265

void TerrainHeightGenerator::Init_Impl()
{
	m_seed = 12345;
}

void TerrainHeightGenerator::GenerateHeights_Impl(std::vector<float> &heights, const int& vertexCount, const int& gridX, const int& gridZ)
{
	m_multithreading_numberOfThreads = std::thread::hardware_concurrency();
	m_multithreading_blockHeight = vertexCount / m_multithreading_numberOfThreads;
	m_heights = heights;
	std::list<std::thread> threads;

	// Assigns a thread to each group of rows
	for (int blockNumber = 0; blockNumber < m_multithreading_numberOfThreads; blockNumber++)
	{
		threads.push_back(std::thread(&TerrainHeightGenerator::GenerateHeightsBlock, this, blockNumber, vertexCount, gridX, gridZ));
	}
	for (std::list<std::thread>::iterator i = threads.begin(); i != threads.end(); ++i)
	{
		i->join();
	}
	heights = m_heights;

	/*
	for (int i = 0; i < vertexCount; ++i)
	{
		for (int j = 0; j < vertexCount; ++j)
		{
			int n = (i * vertexCount) + j;
			// Finds the true vertex placement on the terrain grid in relation to the other terrains, then adds an arbitrary number to make sure the number isn't negative
			int x = j + (gridX * (vertexCount - 1)) + (vertexCount * 5); 
			int z = i + (gridZ * (vertexCount - 1)) + (vertexCount * 5);

			float total = 0;
			float amplitude = m_amplitude;
			float smoothValue = 8.0f;
			for (int k = 0; k < m_octaves; k++)
			{
				total += GetInterpolatedNoise(x / smoothValue, z / smoothValue) * amplitude ;
				amplitude *= m_roughness;
				smoothValue /= 2;
			}
			
			//float total = GetInterpolatedNoise(x / 8.0f, z / 8.0f) * m_amplitude;
			//total += GetInterpolatedNoise(x / 4.0f, z / 4.0f) * m_amplitude / 3;
			//total += GetInterpolatedNoise(x / 2.0f, z / 4.0f) * m_amplitude / 9;

			heights[n] = total;

			//std::cout << "local x: " << j << ", local z: " << i << ", noiseX: " << x << ", noiseZ: " << z << ", height: " << heights[n] << std::endl;
		}
	}
	*/
}

void TerrainHeightGenerator::GenerateHeightsBlock(int blockNumber, const int& vertexCount, const int& gridX, const int& gridZ)
{
	for (int i = blockNumber; i < vertexCount; i+= m_multithreading_numberOfThreads)
	{
		for (int j = 0; j < vertexCount; ++j)
		{
			int n = (i * vertexCount) + j;
			// Finds the true vertex placement on the terrain grid in relation to the other terrains, then adds an arbitrary number to make sure the number isn't negative
			int x = j + (gridX * (vertexCount - 1)) + (vertexCount * 5);
			int z = i + (gridZ * (vertexCount - 1)) + (vertexCount * 5);

			float total = 0;
			float amplitude = m_amplitude;
			float smoothValue = 8.0f;
			for (int k = 0; k < m_octaves; k++)
			{
				total += GetInterpolatedNoise(x / smoothValue, z / smoothValue) * amplitude;
				amplitude *= m_roughness;
				smoothValue /= 2;
			}

			m_heights[n] = total;
		}
	}
}

float TerrainHeightGenerator::GenerateHeight_Impl(const int& x, const int& z, const int& vertexCount, const int& gridX, const int& gridZ)
{
	int noiseX = x + (gridX * (vertexCount - 1)) + (vertexCount * 5);
	int noiseZ = z + (gridZ * (vertexCount - 1)) + (vertexCount * 5);

	return GetNoise(noiseX, noiseZ) * m_amplitude;
}

float TerrainHeightGenerator::GetInterpolatedNoise(float x, float z)
{
	int intX = (int)x;
	int intZ = (int)z;
	float fracX = x - (float)intX;
	float fracZ = z - (float)intZ;

	float v1 = GetSmoothNoise(intX, intZ);
	float v2 = GetSmoothNoise(intX + 1, intZ);
	float v3 = GetSmoothNoise(intX, intZ + 1);
	float v4 = GetSmoothNoise(intX + 1, intZ + 1);
	float i1 = Interpolate(v1, v2, fracX);
	float i2 = Interpolate(v3, v4, fracX);
	return Interpolate(i1, i2, fracZ);
}

float TerrainHeightGenerator::Interpolate(float a, float b, float blend)
{
	double theta = blend * PI;
	float f = (float)(1.0f - cos(theta)) * 0.5f; // gives a value between 0 and 1
	return a * (1.0f - f) + b * f;
}

float TerrainHeightGenerator::GetSmoothNoise(int x, int z)
{
	float corners = (GetNoise(x-1 , z-1) + GetNoise(x-1 , z+1) + GetNoise(x+1, z-1) + GetNoise(x+1, z+1)) / 16.0f;
	float sides = (GetNoise(x, z + 1) + GetNoise(x, z - 1) + GetNoise(x + 1, z) + GetNoise(x - 1, z)) / 8.0f;
	float center = GetNoise(x, z) / 4.0f;
	return corners + sides + center;
}

float TerrainHeightGenerator::GetNoise(int x, int z)
{
	int seed = (x * 78955) + (z * 58438) + m_seed;
	std::mt19937 gen(seed);
	std::uniform_real_distribution<> dis(-1.0, 1.0);
	return dis(gen);

}