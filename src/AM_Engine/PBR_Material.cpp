#include <AM_Engine.h>
#include <glm/ext.hpp>


GLuint PBR_Material::m_brdfID = 0;
bool PBR_Material::brdfCheck = false;
std::vector<IBL_data> PBR_Material::m_envMaps;

PBR_Material::PBR_Material(std::string _path)
{
	std::ifstream file(_path);
	std::string line;
	std::vector<std::string> splitLine;
	if (!file.is_open())
	{
		throw Exception("PBR_Material not found, '" + _path + "' cannot be loaded");
	}
	else
	{
		std::getline(file, line);
		std::string matName = line;
		while (!file.eof())
		{
			std::getline(file, line);
			if (line.length() < 1) continue;
			SplitStringWhitespace(line, splitLine);
			if (splitLine.size() < 1) continue;
			
			if (splitLine.at(0) == "diff")
			{
				m_albedo_rgb.x = (float)atof(splitLine.at(1).c_str());
				m_albedo_rgb.y = (float)atof(splitLine.at(2).c_str());
				m_albedo_rgb.z = (float)atof(splitLine.at(3).c_str());
			}
			else if (splitLine.at(0) == "diff_map")
			{
				m_albedo = std::make_shared<Texture>("../src/resources/Textures/" + matName + "/" + splitLine.at(1));
			}
			else if (splitLine.at(0) == "n_map")
			{
				m_normal = std::make_shared<Texture>("../src/resources/Textures/" + matName + "/" + splitLine.at(1));
			}
			else if (splitLine.at(0) == "m")
			{
				m_metallic_value = (float)atof(splitLine.at(1).c_str());
			}
			else if (splitLine.at(0) == "m_map")
			{
				m_metallic = std::make_shared<Texture>("../src/resources/Textures/" + matName + "/" + splitLine.at(1));
			}
			else if (splitLine.at(0) == "r")
			{
				m_roughness_value = (float)atof(splitLine.at(1).c_str());
			}
			else if (splitLine.at(0) == "r_map")
			{
				m_roughness = std::make_shared<Texture>("../src/resources/Textures/" + matName + "/" + splitLine.at(1));
			}
			else if (splitLine.at(0) == "ao_map")
			{
				m_ao = std::make_shared<Texture>("../src/resources/Textures/" + matName + "/" + splitLine.at(1));
			}
			else if (splitLine.at(0) == "disp_map")
			{
				m_displacement = std::make_shared<Texture>("../src/resources/Textures/" + matName + "/" + splitLine.at(1));
			}
		}
	}
	file.close();
}

void PBR_Material::SplitStringWhitespace(std::string& input, std::vector<std::string>& output)
{
	std::string curr;
	output.clear();
	for (size_t i = 0; i < input.length(); i++)
	{
		if (input.at(i) == ' ' ||
			input.at(i) == '\r' ||
			input.at(i) == '\n' ||
			input.at(i) == '\t')
		{
			if (curr.length() > 0)
			{
				output.push_back(curr);
				curr = "";
			}
		}
		else { curr += input.at(i); }
	}
	if (curr.length() > 0)	{ output.push_back(curr); }
}

void PBR_Material::SetTextures(std::shared_ptr<Texture> _albedo,
								std::shared_ptr<Texture> _normal,
								std::shared_ptr<Texture> _metallic,
								std::shared_ptr<Texture> _roughness,
								std::shared_ptr<Texture> _ao)
{
	m_albedo = _albedo;
	m_normal = _normal;
	m_metallic = _metallic;
	m_roughness = _roughness;
	m_ao = _ao;
}

void PBR_Material::SetIBLData(std::string _name, GLuint _irradianceId, GLuint _prefilterId)
{
	for (size_t i = 0; i < m_envMaps.size(); i++)
	{
		if (_name == m_envMaps.at(i).envName)
		{
			m_envMaps.at(i).irradianceMapID = _irradianceId;
			m_envMaps.at(i).prefilterMapID = _prefilterId;
		}
	}
	// Make a new IBL_data struct to push back
	IBL_data data;
	data.envName = _name;
	data.irradianceMapID = _irradianceId;
	data.prefilterMapID = _prefilterId;
	m_envMaps.push_back(data);

	if (brdfCheck == false)
	{
		MakeBRDFTex();
		brdfCheck = true;
	}
}

void PBR_Material::MakeBRDFTex()
{
	std::shared_ptr<ShaderProgram> brdfShader = std::make_shared<ShaderProgram>("brdf.vert", "brdf.frag");
	unsigned int captureFBO, captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glGenTextures(1, &m_brdfID);
	// pre-allocate enough memory for the LUT texture.
	glBindTexture(GL_TEXTURE_2D, m_brdfID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_brdfID, 0);
	glViewport(0, 0, 512, 512);

	glUseProgram(brdfShader->GetId());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//render quad here
	unsigned int emptyVA;
	glGenVertexArrays(1, &emptyVA);
	glBindVertexArray(emptyVA);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint PBR_Material::GetIrradiance(std::string _name)
{
	for (size_t i = 0; i < m_envMaps.size(); i++)
	{
		if (_name == m_envMaps.at(i).envName)
		{
			return  m_envMaps.at(i).irradianceMapID;
		}
	}
	return NULL;
}

GLuint PBR_Material::GetPrefilter(std::string _name)
{
	for (size_t i = 0; i < m_envMaps.size(); i++)
	{
		if (_name == m_envMaps.at(i).envName)
		{
			return  m_envMaps.at(i).prefilterMapID;
		}
	}
	return NULL;
}

void PBR_Material::ShowUI()
{
	if (ImGui::TreeNode("Albedo"))
	{
		if (m_albedo != nullptr) {
			ImGui::Text(("Albedo texture: " + m_albedo->GetPath()).c_str());
			ImGui::Image((void*)(intptr_t)m_albedo->getId(), ImVec2(100, 100));
			if (ImGui::SmallButton("Remove texture##albedo"))
				m_albedo = nullptr;
		}
		else
			ImGui::ColorPicker3("Colour", &(m_albedo_rgb.x));
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Normal"))
	{
		if (m_normal != nullptr) {
			ImGui::Text(("Normal texture: " + m_normal->GetPath()).c_str());
			ImGui::Image((void*)(intptr_t)m_normal->getId(), ImVec2(100, 100));
			if (ImGui::SmallButton("Remove texture##normal"))
				m_normal = nullptr;
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Metallic"))
	{
		if (m_metallic != nullptr) {
			ImGui::Text(("Metallic texture: " + m_metallic->GetPath()).c_str());
			ImGui::Image((void*)(intptr_t)m_metallic->getId(), ImVec2(100, 100));
			if (ImGui::SmallButton("Remove texture##met"))
				m_metallic = nullptr;
		}
		else
			ImGui::DragFloat("Metallic", &m_metallic_value, 0.005f, 0.0f, 1.0f);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Roughness"))
	{
		if (m_roughness != nullptr) {
			ImGui::Text(("Roughness texture: " + m_roughness->GetPath()).c_str());
			ImGui::Image((void*)(intptr_t)m_roughness->getId(), ImVec2(100, 100));
			if (ImGui::SmallButton("Remove texture##rough"))
				m_roughness = nullptr;
			ImGui::DragFloat("Roughness Adjustment", &m_roughnessAdjustment, 0.005f, -1.0f, 1.0f);
		}
		else
			ImGui::DragFloat("Roughness", &m_roughness_value, 0.005f, 0.0f, 1.0f);
		ImGui::TreePop();
	}
	
	if (ImGui::TreeNode("Ambient Occlusion"))
	{
		if (m_ao != nullptr) {
			ImGui::Text(("ao texture: " + m_ao->GetPath()).c_str());
			ImGui::Image((void*)(intptr_t)m_ao->getId(), ImVec2(100, 100));
			if (ImGui::SmallButton("Remove texture##ao"))
				m_ao = nullptr;
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Displacement"))
	{
		if (m_displacement != nullptr)
		{
			ImGui::Text(("displacement texture: " + m_displacement->GetPath()).c_str());
			ImGui::Image((void*)(intptr_t)m_displacement->getId(), ImVec2(100, 100));
			if (ImGui::SmallButton("Remove texture##disp"))
				m_displacement = nullptr;

			if (ImGui::RadioButton("Parallax clipBorders", m_parallax_clipBorders))
				m_parallax_clipBorders = !m_parallax_clipBorders;
			ImGui::DragFloat("Parallax height", &m_parallax_height, 0.001f);
			ImGui::InputInt("Parallax min layers", &m_parallax_minLayers);
			ImGui::InputInt("Parallax max layers", &m_parallax_maxLayers);
		}
		ImGui::TreePop();
	}
}