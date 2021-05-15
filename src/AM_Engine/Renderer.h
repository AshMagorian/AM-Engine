#pragma once

class ShaderProgram;
class VertexArray;
class Texture;
class PBR_Material;
class AssimpModel;

class Renderer : public Component
{
	IMPLEMENT_CLONE(Renderer)	
	friend class OutlineRenderer;
private:
	std::shared_ptr<ShaderProgram> m_shaderProgram;
	std::shared_ptr<VertexArray> m_va;
	std::shared_ptr<PBR_Material> m_pbrMat;
	std::shared_ptr<Camera> m_cam;
	std::shared_ptr<AssimpModel> m_assimpModel;
	float m_texCoordScale = 1.0f;

	void BindPBRValues();

public:
	Renderer();
	~Renderer();

	void OnInit();
	void OnInit(std::shared_ptr<ShaderProgram> _shader, std::shared_ptr<VertexArray> _va, std::shared_ptr<PBR_Material> _mat);
	void OnInit(std::string _path, std::shared_ptr<ShaderProgram> _shader);
	void OnInit(std::shared_ptr<AssimpModel> _assimp, std::shared_ptr<ShaderProgram> _shader);
	void OnTick();
	void OnDisplay();
	void OnShowUI();
	void OnSave(Json::Value& val);
	void OnLoad(Json::Value& val);

	void SetMesh(std::shared_ptr<VertexArray> _va) { m_va = _va; }
	void SetPBRMaterial(std::shared_ptr<PBR_Material> _mat) { m_pbrMat = _mat; }
	void SetShader(std::shared_ptr<ShaderProgram> _shader) { m_shaderProgram = _shader; }
	void SetCamera(std::shared_ptr<Camera> _cam) { m_cam = _cam; }
	void SetTexCoordScale(float _value) { m_texCoordScale = _value; }
	void DrawOutline();

	void SetParallaxClipBorders(bool value);

};