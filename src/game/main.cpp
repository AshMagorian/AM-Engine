#include <AM_Engine/AM_Engine.h>

#include "FirstPersonCameraControls.h"

//#define WINDOW_WIDTH 1920 
//#define WINDOW_HEIGHT 1080 
#define WINDOW_WIDTH 1280 
#define WINDOW_HEIGHT 720 

int main(int argc, char *argv[])
{
	std::shared_ptr<Application> application = Application::Init(WINDOW_WIDTH, WINDOW_HEIGHT, DEBUG_ON);

	application->GetResourceManager()->CreateResource<ShaderProgram>("pbrShader.txt", "pbr_shader");
	application->GetResourceManager()->CreateResource<ShaderProgram>("test.txt", "test_shader");
	application->GetResourceManager()->CreateResource<ShaderProgram>("NormalDebug.txt", "normal_debug_shader");
	application->GetResourceManager()->CreateResource<ShaderProgram>("TangentDebug.txt", "tangent_debug_shader");

	application->GetResourceManager()->CreateResource<VertexArray>("cube", "cube");
	application->GetResourceManager()->CreateResource<VertexArray>("plane", "plane");
	application->GetResourceManager()->CreateResource<VertexArray>("sphere", "sphere");

	application->GetResourceManager()->CreateResource<PBR_Material>("../src/resources/Textures/grass/grass.txt", "grass");
	application->GetResourceManager()->CreateResource<PBR_Material>("../src/resources/Textures/Cobblestone/cobblestone.txt", "cobble_mat"); 
	application->GetResourceManager()->CreateResource<PBR_Material>("../src/resources/Textures/forest_ground/forest_ground.txt", "forest_ground");
	application->GetResourceManager()->CreateResource<PBR_Material>("../src/resources/Textures/metal_plate/metal_plate.txt", "metal_plate_mat");

	std::shared_ptr<AssimpModel> backpack_obj = std::make_shared<AssimpModel>("../src/resources/backpack/backpack.obj", FLIP_TEXTURES);
	application->GetResourceManager()->CreateResource<AssimpModel>(backpack_obj, "backpack_obj");
	std::shared_ptr<AssimpModel> globe_obj = std::make_shared<AssimpModel>("../src/resources/Old_Antique_Standing_Globe_OBJ/Old_Antique_Standing_Globe_.obj");
	application->GetResourceManager()->CreateResource<AssimpModel>(globe_obj, "globe_obj");

	application->GetSkybox()->CreateSkybox("bg", "../src/resources/Textures/MonValley_A_LookoutPoint_2k.hdr");
	application->GetSkybox()->CreateSkybox("bg2", "../src/resources/Textures/Mt-Washington-Gold-Room_Ref.hdr");
	//application->GetSkybox()->CreateSkybox("bg3", "../src/resources/Textures/cape_hill_4k.hdr");


	std::shared_ptr<PBR_Material> dielectric = std::make_shared<PBR_Material>();
	dielectric->SetAlbedo(glm::vec3(0.2f, 0.0f, 0.0f));
	dielectric->SetMetallic(0.0f);
	dielectric->SetRoughness(0.3f);
	application->GetResourceManager()->CreateResource<PBR_Material>(dielectric, "dielectric");

	std::shared_ptr<PBR_Material> metal = std::make_shared<PBR_Material>();
	metal->SetAlbedo(glm::vec3(0.2f, 0.3f, 0.5f));
	metal->SetMetallic(1.0f);
	metal->SetRoughness(0.1f);
	application->GetResourceManager()->CreateResource<PBR_Material>(metal, "metal");

	//std::shared_ptr<Entity> light = application->AddEntity();
	//light->AddComponent<PointLight>();
	//light->GetTransform()->SetPos(glm::vec3(2.0f, 3.0f, 1.0f));

	//*********************************************************************************************************************************

	std::shared_ptr<Scene> scene1 = application->GetSceneManager()->CreateScene("Scene1");
	scene1->camera->AddComponent<FirstPersonCameraControls>();
	scene1->camera->GetTransform()->SetPos(glm::vec3(0.0f, 2.0f, 5.0f));
	scene1->cubemapName = "bg";

	std::shared_ptr<Entity> sphere = scene1->AddEntity("sphere");
	sphere->AddComponent<Renderer>(application->GetResourceManager()->LoadFromResources<ShaderProgram>("pbr_shader"),
		application->GetResourceManager()->LoadFromResources<VertexArray>("cube"),
		application->GetResourceManager()->LoadFromResources<PBR_Material>("cobble_mat"));
	sphere->GetTransform()->SetPos(glm::vec3(0.0f, 2.0f, 0.0f));
	sphere->GetComponent<Renderer>()->SetTexCoordScale(2.0f);
	sphere->GetComponent<Renderer>()->SetParallaxClipBorders(false);
	std::shared_ptr<Entity> sphere2 = scene1->AddEntity("sphere2");
	sphere2->AddComponent<Renderer>(application->GetResourceManager()->LoadFromResources<ShaderProgram>("pbr_shader"),
		application->GetResourceManager()->LoadFromResources<VertexArray>("sphere"),
		dielectric);
	sphere2->GetTransform()->SetPos(glm::vec3(2.0f, 2.0f, 0.0f));
	std::shared_ptr<Entity> sphere3 = scene1->AddEntity("sphere3");
	sphere3->AddComponent<Renderer>(application->GetResourceManager()->LoadFromResources<ShaderProgram>("pbr_shader"),
		application->GetResourceManager()->LoadFromResources<VertexArray>("sphere"),
		metal);
	sphere3->GetTransform()->SetPos(glm::vec3(-2.0f, 2.0f, 0.0f));
	
	//**********************************************************************************************************************************

	std::shared_ptr<Scene> scene2 = application->GetSceneManager()->CreateScene("Scene2");
	scene2->camera->AddComponent<FirstPersonCameraControls>();
	scene2->camera->GetTransform()->SetPos(glm::vec3(0.0f, 0.0f, 5.0f));
	scene2->cubemapName = "bg2";

	// Test Object from assimp
	std::shared_ptr<Entity> assimpTest = scene2->AddEntity("assimpTest");
	//assimpTest->AddComponent<Renderer>("../src/resources/backpack/backpack.obj",
	//	application->GetResourceManager()->LoadFromResources<ShaderProgram>("pbr_shader"));
	//assimpTest->AddComponent<Renderer>("../src/resources/Survival_BackPack_2/Survival_BackPack_2.fbx",
	//	application->GetResourceManager()->LoadFromResources<ShaderProgram>("pbr_shader"));
	//assimpTest->AddComponent<Renderer>("../src/resources/Old_Antique_Standing_Globe_FBX/Old_Antique_Standing_Globe_.fbx",
	//	application->GetResourceManager()->LoadFromResources<ShaderProgram>("pbr_shader"));
	assimpTest->AddComponent<Renderer>(application->GetResourceManager()->LoadFromResources<AssimpModel>("globe_obj") ,
		application->GetResourceManager()->LoadFromResources<ShaderProgram>("pbr_shader"));

	assimpTest->GetTransform()->SetScale(glm::vec3(0.02f, 0.02f, 0.02f));

	application->GetSceneManager()->SetStartupScene("Scene1");

	// Terrain stuff

	application->GetSceneManager()->GetCurrentScene()->terrain->AddTerrainBlock(0, 0, 
		application->GetResourceManager()->LoadFromResources<PBR_Material>("grass"));
	application->GetSceneManager()->GetCurrentScene()->terrain->SetTerrainBlockMaterial_r(0, 0,
		application->GetResourceManager()->LoadFromResources<PBR_Material>("cobble_mat"));
	application->GetSceneManager()->GetCurrentScene()->terrain->SetTerrainBlockMaterial_g(0, 0,
		application->GetResourceManager()->LoadFromResources<PBR_Material>("forest_ground"));
	application->GetSceneManager()->GetCurrentScene()->terrain->SetTerrainBlockMaterial_b(0, 0,
		application->GetResourceManager()->LoadFromResources<PBR_Material>("cobble_mat"));

	application->GetResourceManager()->CreateResource<Texture>("../src/resources/Textures/blendMap.png", "blendMap");
	application->GetSceneManager()->GetCurrentScene()->terrain->SetTerrainBlockBlendMap(0, 0, 
		application->GetResourceManager()->LoadFromResources<Texture>("blendMap"));


	/**
	*Runs the game loop from application
	*/
	application->Run();
	return 0;
}