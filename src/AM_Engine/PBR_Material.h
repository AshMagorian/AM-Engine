#pragma once

#include "Resource.h"

#include <memory>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Texture;
class ShaderProgram;

struct IBL_data
{
	std::string envName;
	GLuint irradianceMapID;
	GLuint prefilterMapID;
};

class PBR_Material : public Resource
{
	friend class Renderer;
	friend class TerrainGrid;
	friend class DebugUIManager;
private:
	std::shared_ptr<Texture> m_albedo;
	std::shared_ptr<Texture> m_normal;
	std::shared_ptr<Texture> m_metallic;
	std::shared_ptr<Texture> m_roughness;
	std::shared_ptr<Texture> m_ao;
	std::shared_ptr<Texture> m_displacement;

	glm::vec3 m_albedo_rgb = glm::vec3(1.0f, 1.0f, 1.0f);
	float m_metallic_value = 0.0f;
	float m_roughness_value = 0.5f;
	float m_roughnessAdjustment = 0.0f;

	static std::vector<IBL_data> m_envMaps;
	static bool brdfCheck;
	static GLuint m_brdfID;

	//Parallax data
	bool m_parallax_clipBorders = true;
	float m_parallax_height = 0.1f;
	int m_parallax_minLayers = 8;
	int m_parallax_maxLayers = 32;
	float m_parallax_adjustment = 0;
	float m_parallax_heightScaleAdjustment = 1;

	void SplitStringWhitespace(std::string& input, std::vector<std::string>& output);
	static void MakeBRDFTex();
public:
	PBR_Material() {}
	PBR_Material(std::string _path);

	void ShowUI();

	static void SetIBLData(std::string _name, GLuint _irradianceId, GLuint _prefilterId);
	static GLuint GetIrradiance(std::string _name);
	static GLuint GetPrefilter(std::string _name);
	static GLuint GetBRDF() { return m_brdfID; }

	std::shared_ptr<Texture> GetAlbedo() { return m_albedo; }
	std::shared_ptr<Texture> GetNormal() { return m_normal; }
	std::shared_ptr<Texture> GetMetallic() { return m_metallic; }
	std::shared_ptr<Texture> GetRoughness() { return m_roughness; }
	std::shared_ptr<Texture> GetAO() { return m_ao; }
	std::shared_ptr<Texture> GetDisplacement() { return m_displacement; }

	glm::vec3 GetAlbedoValue() { return m_albedo_rgb; }
	float GetMetallicValue() { return m_metallic_value; }
	float GetRoughnessValue() { return m_roughness_value; }

	void SetTextures(std::shared_ptr<Texture> _albedo,
					std::shared_ptr<Texture> _normal,
					std::shared_ptr<Texture> _metallic,
					std::shared_ptr<Texture> _roughness,
					std::shared_ptr<Texture> _ao);
	void SetAlbedoTex(std::shared_ptr<Texture> tex) { m_albedo = tex; }
	void SetNormalTex(std::shared_ptr<Texture> tex) { m_normal = tex; }
	void SetMetallicTex(std::shared_ptr<Texture> tex) { m_metallic = tex; }
	void SetRoughnessTex(std::shared_ptr<Texture> tex) { m_roughness = tex; }
	void SetAOTex(std::shared_ptr<Texture> tex) { m_ao = tex; }
	void SetDisplacementTex(std::shared_ptr<Texture> tex) { m_displacement = tex; }

	void SetAlbedo(glm::vec3 value) { m_albedo_rgb = value; }
	void SetMetallic(float value) { m_metallic_value = value; }
	void SetRoughness(float value) { m_roughness_value = value; }

	void SetParallaxClipBorders(bool value) { m_parallax_clipBorders = value; }
};