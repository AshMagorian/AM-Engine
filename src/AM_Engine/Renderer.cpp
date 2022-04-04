#include "AM_Engine.h"
#include "AssimpModel.h"
#include "MasterRenderer.h"

MAKE_PROTOTYPE(Renderer)

Renderer::Renderer() {}
Renderer::~Renderer() 
{
}

void Renderer::OnInit()
{
	m_shaderProgram = GetApplication()->GetResourceManager()->LoadFromResources<ShaderProgram>("pbr_shader");
	m_va = GetApplication()->GetResourceManager()->LoadFromResources<VertexArray>("sphere");
	m_pbrMat = std::make_shared<PBR_Material>();
	m_cam = GetApplication()->GetCamera();
}
void Renderer::OnInit(std::shared_ptr<ShaderProgram> _shader, std::shared_ptr<VertexArray> _va, std::shared_ptr<PBR_Material> _mat)
{
	m_shaderProgram = _shader;
	m_va = _va;
	m_pbrMat = _mat;
	m_cam = GetApplication()->GetCamera();
}
void Renderer::OnInit(std::string _path, std::shared_ptr<ShaderProgram> _shader)
{
	//Convert the path to char*
	char* p = &_path[0];
	m_assimpModel = std::make_shared<AssimpModel>(p);
	m_shaderProgram = _shader;
	m_cam = GetApplication()->GetCamera();
}
void Renderer::OnInit(std::shared_ptr<AssimpModel> _assimp, std::shared_ptr<ShaderProgram> _shader)
{
	m_assimpModel =_assimp;
	m_shaderProgram = _shader;
	m_cam = GetApplication()->GetCamera();
}

void Renderer::OnTick()
{
	
}
void Renderer::OnDisplay()
{

	m_shaderProgram = MasterRenderer::GetGeoShader();

	m_shaderProgram->SetUniform("in_Projection", m_cam->GetProjectionMatrix());
	m_shaderProgram->SetUniform("in_View", m_cam->GetViewMatrix());

	m_shaderProgram->SetUniform("in_Model", GetEntity()->GetTransform()->GetModelMatrix());
	m_shaderProgram->SetUniform("in_NormalMtx", GetEntity()->GetTransform()->GetNormalMatrix());
	if (m_assimpModel)
	{
		//Render using assimp
		 m_assimpModel->Draw(m_shaderProgram);
	}
	else if (m_va)
	{
		if (m_pbrMat)
			BindPBRValues();
		m_va->Draw(m_shaderProgram);
	}

}

void Renderer::BindPBRValues()
{
	int matBinary = 0;

	if (m_pbrMat->GetAlbedo() != nullptr) {
		m_shaderProgram->SetUniform("in_Material.texture_diffuse1", m_pbrMat->GetAlbedo());
		matBinary += 16;
	}
	else
		m_shaderProgram->SetUniform("in_pxlVal.albedo", m_pbrMat->GetAlbedoValue());

	if (m_pbrMat->GetNormal() != nullptr) {
		m_shaderProgram->SetUniform("in_Material.texture_normal1", m_pbrMat->GetNormal());
		matBinary += 8;
	}
	//else
	//	m_shaderProgram->SetUniform("in_pxlVal.normal", glm::vec3(0.0f, 0.0f, 1.0f));

	if (m_pbrMat->GetMetallic() != nullptr) {
		m_shaderProgram->SetUniform("in_Material.texture_metallic1", m_pbrMat->GetMetallic());
		matBinary += 4;
	}
	else
		m_shaderProgram->SetUniform("in_pxlVal.metallic", m_pbrMat->GetMetallicValue());

	if (m_pbrMat->GetRoughness() != nullptr) {
		m_shaderProgram->SetUniform("in_Material.texture_roughness1", m_pbrMat->GetRoughness());

		matBinary += 2;
	}
	else
		m_shaderProgram->SetUniform("in_pxlVal.roughness", m_pbrMat->GetRoughnessValue());

	if (m_pbrMat->GetAO() != nullptr) {
		m_shaderProgram->SetUniform("in_Material.texture_ao1", m_pbrMat->GetAO());
		matBinary += 1;
	}
	else
		m_shaderProgram->SetUniform("in_pxlVal.ao", 1.0f);

	if (MasterRenderer::IsSimpleRendering() == false)
	{
		if (m_pbrMat->GetDisplacement() != nullptr) {
			m_shaderProgram->SetUniform("in_Material.texture_displacement1", m_pbrMat->GetDisplacement());

			m_shaderProgram->SetUniform("in_Parallax_ClipBorders", (int)m_pbrMat->m_parallax_clipBorders);
			m_shaderProgram->SetUniform("in_Parallax_Height", m_pbrMat->m_parallax_height);
			m_shaderProgram->SetUniform("in_Parallax_MinLayers", m_pbrMat->m_parallax_minLayers);
			m_shaderProgram->SetUniform("in_Parallax_MaxLayers", m_pbrMat->m_parallax_maxLayers);
			m_shaderProgram->SetUniform("in_Parallax_adjustment", m_pbrMat->m_parallax_adjustment);
			matBinary += 32;
		}
	}

	m_shaderProgram->SetUniform("in_MatValue", matBinary);
	m_shaderProgram->SetUniform("in_TexCoordScale", m_texCoordScale);
}

void Renderer::DrawOutline()
{ 
	GetApplication()->GetOutlineRenderer()->AddToList(GetEntity()); 
}

void Renderer::SetParallaxClipBorders(bool value) 
{ 
	m_pbrMat->SetParallaxClipBorders(value); 
}


void Renderer::OnShowUI()
{
	if (m_shaderProgram)
	{
		ImGui::Text(("Shader: " + m_shaderProgram->GetName()).c_str());
	}
	ImGui::SameLine(150); 
	if (ImGui::SmallButton("Select Shader"))
		ImGui::OpenPopup("shader_popup");
	if (ImGui::BeginPopup("shader_popup"))
	{
		ImGui::Text("Shaders");
		ImGui::Separator();

		std::list<std::shared_ptr<ShaderProgram>> list;
		GetApplication()->GetResourceManager()->GetAll(&list);
		for (std::list<std::shared_ptr<ShaderProgram>>::iterator i = list.begin(); i != list.end(); ++i)
		{
			if (ImGui::Selectable((*i)->GetName().c_str()))
				m_shaderProgram = (*i);
		}
		ImGui::EndPopup();
	}

	if (m_assimpModel)
	{
		ImGui::Text(("Assimp Mesh: " + m_assimpModel->GetName()).c_str());
	}
	else if (m_va)
	{
		ImGui::Text(("Mesh: " + m_va->GetName()).c_str());

	}
	ImGui::SameLine(150);
	if (ImGui::SmallButton("Select Mesh"))
		ImGui::OpenPopup("mesh_popup");
	if (ImGui::BeginPopup("mesh_popup"))
	{
		ImGui::Text("Meshes");
		ImGui::Separator();

		std::list<std::shared_ptr<VertexArray>> list;
		GetApplication()->GetResourceManager()->GetAll(&list);
		for (std::list<std::shared_ptr<VertexArray>>::iterator i = list.begin(); i != list.end(); ++i)
		{
			if (ImGui::Selectable((*i)->GetName().c_str()))
			{
				m_assimpModel = nullptr;
				m_va = std::make_shared<VertexArray>(*(*i));
			}
		}
		ImGui::Separator();
		ImGui::Text("Assimp Meshes");
		ImGui::Separator();

		std::list<std::shared_ptr<AssimpModel>> list2;
		GetApplication()->GetResourceManager()->GetAll(&list2);
		for (std::list<std::shared_ptr<AssimpModel>>::iterator i = list2.begin(); i != list2.end(); ++i)
		{
			if (ImGui::Selectable((*i)->GetName().c_str()))
			{
				m_va = nullptr;
				m_assimpModel = std::make_shared<AssimpModel>(*(*i));
			}
		}
		ImGui::EndPopup();
	}

	if (m_pbrMat && !m_assimpModel)
	{
		if (ImGui::TreeNode("PBR Material"))
		{
			ImGui::DragFloat("TexCoord Scale", &m_texCoordScale, 0.01f);
			m_pbrMat->ShowUI();
			ImGui::TreePop();
		}
		else
		{
			ImGui::SameLine(150);
			if (ImGui::SmallButton("Select PBR Material##b"))
				ImGui::OpenPopup("pbr_popup");
		}
	}
	if (ImGui::BeginPopup("pbr_popup"))
	{
		ImGui::Text("PBR Materials");
		ImGui::Separator();

		std::list<std::shared_ptr<PBR_Material>> list;
		GetApplication()->GetResourceManager()->GetAll(&list);
		for (std::list<std::shared_ptr<PBR_Material>>::iterator i = list.begin(); i != list.end(); ++i)
		{
			if (ImGui::Selectable((*i)->GetName().c_str()))
				m_pbrMat = GetApplication()->GetResourceManager()->LoadFromResources<PBR_Material>((*i)->GetName());
		}
		ImGui::EndPopup();
	}
}

void Renderer::OnSave(Json::Value& val)
{
	SAVE_TYPE(Renderer);
	SAVE_VALUE("shaderProgram", m_shaderProgram->GetName());
	if (m_va != nullptr)
		SAVE_VALUE("va", m_va->GetName());
	else
		SAVE_VALUE("va",0);
	if (m_assimpModel != nullptr)
		SAVE_VALUE("assimp", m_assimpModel->GetName());
	else
		SAVE_VALUE("assimp", 0);

	// PBR
	Json::Value json_pbrMat;
	json_pbrMat["path"] = m_pbrMat->GetName();
	if (m_pbrMat->GetAlbedo() != nullptr)
		json_pbrMat["albedo"] = m_pbrMat->GetAlbedo()->GetName();
	else
	{
		json_pbrMat["albedo"][0] = m_pbrMat->GetAlbedoValue().x;
		json_pbrMat["albedo"][1] = m_pbrMat->GetAlbedoValue().y;
		json_pbrMat["albedo"][2] = m_pbrMat->GetAlbedoValue().z;
	}
	if (m_pbrMat->GetNormal() != nullptr)
		json_pbrMat["normal"] = m_pbrMat->GetNormal()->GetName();
	else
		json_pbrMat["normal"] = 0;
	if (m_pbrMat->GetMetallic() != nullptr)
		json_pbrMat["metallic"] = m_pbrMat->GetMetallic()->GetName();
	else
		json_pbrMat["metallic"] = m_pbrMat->GetMetallicValue();
	if (m_pbrMat->GetRoughness() != nullptr)
		json_pbrMat["roughness"] = m_pbrMat->GetRoughness()->GetName();
	else
		json_pbrMat["roughness"] = m_pbrMat->GetRoughnessValue();
	if (m_pbrMat->GetAO() != nullptr)
		json_pbrMat["ao"] = m_pbrMat->GetAO()->GetName();
	else
		json_pbrMat["ao"] = 0;
	if (m_pbrMat->GetDisplacement() != nullptr)
		json_pbrMat["displacement"] = m_pbrMat->GetDisplacement()->GetName();
	else
		json_pbrMat["displacement"] = 0;
	SAVE_VALUE("pbrMat", json_pbrMat);

	SAVE_VALUE("texCoordScale", m_texCoordScale);
	SAVE_VALUE("parallaxClipBorders", m_pbrMat->m_parallax_clipBorders);
	SAVE_VALUE("parallaxHeight", m_pbrMat->m_parallax_height);
	SAVE_VALUE("parallaxMin", m_pbrMat->m_parallax_minLayers);
	SAVE_VALUE("parallaxMax", m_pbrMat->m_parallax_maxLayers);
}

void Renderer::OnLoad(Json::Value& val)
{
	std::shared_ptr<Resources> resourceManager = GetApplication()->GetResourceManager();
	m_shaderProgram = resourceManager->LoadFromResources<ShaderProgram>(val["shaderProgram"].asString());
	if (val["va"].isString())
		m_va = resourceManager->LoadFromResources<VertexArray>(val["va"].asString());
	if (val["assimp"].isString())
		m_assimpModel = resourceManager->LoadFromResources<AssimpModel>(val["assimp"].asString());

	// PBR
	Json::Value json_pbrMat = val["pbrMat"];
	if (!json_pbrMat["path"].asString().empty())
		m_pbrMat = resourceManager->LoadFromResources<PBR_Material>(json_pbrMat["path"].asString());

	if (json_pbrMat["albedo"].isString())
	{
		if (!json_pbrMat["albedo"].asString().empty())
		m_pbrMat->SetAlbedoTex(resourceManager->LoadFromResources<Texture>(json_pbrMat["albedo"].asString()));
	}
	else
	{
		glm::vec3 tmp; 
		tmp.x = json_pbrMat["albedo"][0].asFloat();
		tmp.y = json_pbrMat["albedo"][1].asFloat();
		tmp.z = json_pbrMat["albedo"][2].asFloat();
		m_pbrMat->SetAlbedo(tmp);
	}

	if (json_pbrMat["normal"].isString())
	{
		if (!json_pbrMat["normal"].asString().empty())
			m_pbrMat->SetNormalTex(resourceManager->LoadFromResources<Texture>(json_pbrMat["normal"].asString()));
	}
		

	if (json_pbrMat["metallic"].isString())
	{
		if (!json_pbrMat["metallic"].asString().empty())
			m_pbrMat->SetMetallicTex(resourceManager->LoadFromResources<Texture>(json_pbrMat["metallic"].asString()));
	}
	else
		m_pbrMat->SetMetallic(json_pbrMat["metallic"].asFloat());

	if (json_pbrMat["roughness"].isString())
	{
		if (!json_pbrMat["roughness"].asString().empty())
			m_pbrMat->SetRoughnessTex(resourceManager->LoadFromResources<Texture>(json_pbrMat["roughness"].asString()));
	}
	else
		m_pbrMat->SetRoughness(json_pbrMat["roughness"].asFloat());

	if (json_pbrMat["ao"].isString())
	{
		if (!json_pbrMat["ao"].asString().empty())
			m_pbrMat->SetAOTex(resourceManager->LoadFromResources<Texture>(json_pbrMat["ao"].asString()));
	}

	if (json_pbrMat["displacement"].isString())
	{
		if (!json_pbrMat["displacement"].asString().empty())
			m_pbrMat->SetDisplacementTex(resourceManager->LoadFromResources<Texture>(json_pbrMat["displacement"].asString()));
	}
		
	LOAD_FLOAT("texCoordScale", m_texCoordScale);
	LOAD_BOOL("parallaxClipBorders", m_pbrMat->m_parallax_clipBorders);
	LOAD_FLOAT("parallaxHeight", m_pbrMat->m_parallax_height);
	LOAD_INT("parallaxMin", m_pbrMat->m_parallax_minLayers);
	LOAD_INT("parallaxMax", m_pbrMat->m_parallax_maxLayers);
}