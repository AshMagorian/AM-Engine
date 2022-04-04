#include <AM_Engine/AM_Engine.h>
#include "SaveManager.h"
#include "MasterRenderer.h"
#include "WaterRenderer.h"
#include "WaterBuffers.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

void DebugUIManager::Init(GLFWwindow* _window, std::weak_ptr<Application> _app)
{
	m_app = _app;
	m_sceneManager = m_app.lock()->GetSceneManager();
	// Setup ImGui 
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfw_InitForOpenGL(_window, true);
	ImGui_ImplOpenGL3_Init((char*)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));
	ImGui::StyleColorsDark();
}

void DebugUIManager::NewFrame()
{
	//imgui new Frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void DebugUIManager::Tick(std::list<std::shared_ptr<Entity>> _entities, int _width, int _height)
{
	std::shared_ptr<Scene> currentScene = m_sceneManager->GetCurrentScene();
	//ImGui::ShowDemoWindow();
	if (m_newScene)
		ImGui::SetNextWindowSize(ImVec2(300.0f, 600.0f));
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	std::string coreTitle = "Core (" + currentScene->sceneName + ")";
	ImGui::Begin(&coreTitle[0], NULL, ImGuiWindowFlags_MenuBar);
	m_newScene = false;
	std::string menu_popup_action = "";
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			if (ImGui::BeginMenu("Change Scene")) 
			{
				for (size_t i = 0; i < m_sceneManager->m_scenes.size(); ++i)
				{
					std::string tmp = m_sceneManager->m_scenes.at(i)->sceneName;
					if (ImGui::MenuItem(&tmp[0]))
					{
						m_sceneManager->SetCurrentScene(tmp);
						m_newScene = true;
					}
				}

				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Save Scene")) menu_popup_action = "Save";
			if (ImGui::BeginMenu("Load Scene"))
			{
				std::vector<std::string> names = m_app.lock()->GetSaveManager()->GetScenes();
				for (size_t i = 0; i < names.size(); ++i)
				{
					std::string tmp = names.at(i);
					if (ImGui::MenuItem(&tmp[0]))
					{
						m_app.lock()->GetSaveManager()->LoadScene(tmp);
						m_newScene = true;
					}
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if (menu_popup_action == "Save") ImGui::OpenPopup("Save");
	if (ImGui::BeginPopup("Save"))
	{
		ImGui::Text("Enter the name of your Scene");
		ImGui::Separator();
		static char sceneName[64] = ""; ImGui::InputText("", sceneName, 64);
		if (ImGui::Button("save_confirm") && sceneName[0] != NULL)
		{
			currentScene->sceneName = sceneName;
			m_app.lock()->GetSaveManager()->SaveScene();
			m_newScene = true;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	//Core window with scene selection, entities tab and skybox

	if (ImGui::CollapsingHeader("Entities"))
	{
		for (std::list<std::shared_ptr<Entity>>::iterator it = _entities.begin(); it != _entities.end(); ++it)
		{
			std::string tmp = (*it)->GetName();
			char* label = &tmp[0];
			if (ImGui::Selectable(label, m_currentEntity == (*it) && m_entityWindow == true))
			{
				m_currentEntity = (*it);
				m_entityWindow = true;
				m_resetEntityWindowSize = true;
			}
		}
		if (ImGui::Button("Add Entity"))
			ImGui::OpenPopup("entity_popup");
		if (ImGui::BeginPopup("entity_popup"))
		{
			ImGui::Text("New Entity Name");
			ImGui::Separator();
			static char entityName[64] = ""; ImGui::InputText("", entityName, 64);
			if (ImGui::Button("Add") && entityName[0] != NULL)
			{
				currentScene->AddEntity(entityName);
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

	}
	// Directional light
	if (ImGui::CollapsingHeader("Directional Light"))
	{
		ImGui::Text("    x        y         z");
		ImGui::DragFloat3("Direction", &(currentScene->lightManager->GetDirectionalLight()->direction.x), 0.005f);
		ImGui::ColorPicker3("Colour", &(currentScene->lightManager->GetDirectionalLight()->colour.x));
		ImGui::DragFloat("Intensity", &(currentScene->lightManager->GetDirectionalLight()->intensity), 0.05f);
	}
	// Skybox selector
	if (ImGui::CollapsingHeader("Skybox"))
	{
		const int numCubeMaps = Skybox::m_cubemaps.size();
		std::vector<char*> maps;
		for (int i = 0; i < numCubeMaps; ++i)
			maps.push_back(&Skybox::m_cubemaps.at(i).name[0]);
		const char* mapsArr[100];
		std::copy(maps.begin(), maps.end(), mapsArr);
		static int item_current = 0;
		ImGui::Combo("skybox", &item_current, mapsArr, maps.size());
		if (ImGui::Button("Apply"))
		{
			currentScene->cubemapName = maps.at(item_current);
			m_app.lock()->GetSkybox()->SetSkybox(maps.at(item_current));
		}
		GLuint id = Skybox::m_cubemaps[item_current].preview_id;

		ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
		ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
		ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
		ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white
		
		ImGui::Image((void*)(intptr_t)id, ImVec2(200, 200), uv_min, uv_max, tint_col, border_col);
		
	}
	// Terrain editor
	if (ImGui::CollapsingHeader("Terrain"))
	{
		ImGui::Text("Parallax Settings");
		ImGui::DragFloat("Height", &(currentScene->terrain->m_parallax_Height), 0.005f);

		// Grid selector
		static char selected = 12;
		for (int y = 0; y < 5; y++)
		{
			for (int x = 0; x < 5; x++)
			{
				if (x > 0)
					ImGui::SameLine();
				char name[32];
				sprintf(name, "(%i,%i)", x - 2, 2 - y);


				ImGui::PushID(y * 5 + x);
				if (ImGui::Selectable(name, selected == y * 5 + x, 0, ImVec2(50, 50)))
				{
					// Toggle clicked cell 
					selected = y * 5 + x;
				}
				ImGui::PopID();
			}
		}
		int blockX = (selected % 5) - 2;
		int blockY = (2 - (selected / 5));
		ImGui::Text("The block currently selected is (%i,%i)", blockX, blockY);
		bool blockFound = false;
		for (std::list<std::shared_ptr<TerrainBlock>>::iterator i = currentScene->terrain->m_terrainBlocks.begin(); i != currentScene->terrain->m_terrainBlocks.end(); ++i)
		{
			// If the selected grid tile matches the one in the list
			if ((*i)->m_gridPosX == blockX && (*i)->m_gridPosZ == -blockY)
			{
				blockFound = true;

				//Display the blend map here
				if ((*i)->m_blendMap != nullptr)
				{
					ImVec2 uv_min = ImVec2(0.0f, 1.0f);                 // Top-left
					ImVec2 uv_max = ImVec2(1.0f, 0.0f);                 // Lower-right
					ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
					ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white
					ImGui::Text("Blend Map");
					ImGui::Image((void*)(intptr_t)(*i)->m_blendMap->getId(), ImVec2(200, 200), uv_min, uv_max, tint_col, border_col);
				}

				ImGui::DragFloat("TexCoord Scale", &m_app.lock()->GetSceneManager()->GetCurrentScene()->terrain->m_texCoordScale, 0.01f);

				char* colours[] = { "black", "red", "green", "blue" };

				for (int blendIndex = 0; blendIndex < 4; ++blendIndex)
				{
					char name[32];
					sprintf(name, "Material %i (%s)", blendIndex ,colours[blendIndex]);
					if (ImGui::TreeNode(name))
					{
						ImGui::DragFloat("Height adjustment", &((*i)->m_material[blendIndex]->m_parallax_adjustment), 0.005f);
						ImGui::DragFloat("Height scale adjustment", &((*i)->m_material[blendIndex]->m_parallax_heightScaleAdjustment), 0.001f);

						if (ImGui::TreeNode("PBR Material"))
						{
							//ImGui::DragFloat("TexCoord Scale", &m_texCoordScale, 0.01f);
							(*i)->m_material[blendIndex]->ShowUI();
							ImGui::TreePop();
						}
						else
						{
							ImGui::SameLine(150);
							if (ImGui::SmallButton("Select PBR Material##b"))
								ImGui::OpenPopup("pbr_popup");
						}
						if (ImGui::BeginPopup("pbr_popup"))
						{
							ImGui::Text("PBR Materials");
							ImGui::Separator();

							std::list<std::shared_ptr<PBR_Material>> list;
							m_app.lock()->GetResourceManager()->GetAll(&list);
							for (std::list<std::shared_ptr<PBR_Material>>::iterator j = list.begin(); j != list.end(); ++j)
							{
								if (ImGui::Selectable((*j)->GetName().c_str()))
									(*i)->m_material[blendIndex] = m_app.lock()->GetResourceManager()->LoadFromResources<PBR_Material>((*j)->GetName());
							}
							ImGui::EndPopup();
						}
						ImGui::TreePop();
					}

				}

			}
			
		}
		if (blockFound == false)
		{
			if (ImGui::SmallButton("add terrain block"))
			{
				m_app.lock()->GetSceneManager()->GetCurrentScene()->terrain->AddTerrainBlock(blockX, -blockY,
					m_app.lock()->GetResourceManager()->LoadFromResources<PBR_Material>("grass"));
			}
		}
	}
	if (ImGui::CollapsingHeader("DebugTextures"))
	{
		ImVec2 uv_min = ImVec2(0.0f, 1.0f);                 // Top-left
		ImVec2 uv_max = ImVec2(1.0f, 0.0f);                 // Lower-right
		ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
		ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white
		ImGui::Text("Position");
		ImGui::Image((void*)(intptr_t)MasterRenderer::Get().gPosition, ImVec2(200, 200), uv_min, uv_max, tint_col, border_col);
		ImGui::Text("Normal");
		ImGui::Image((void*)(intptr_t)MasterRenderer::Get().gNormal, ImVec2(200, 200), uv_min, uv_max, tint_col, border_col);
		ImGui::Text("Albedo");
		ImGui::Image((void*)(intptr_t)MasterRenderer::Get().gAlbedo, ImVec2(200, 200), uv_min, uv_max, tint_col, border_col);
		ImGui::Text("MRA");
		ImGui::Image((void*)(intptr_t)MasterRenderer::Get().gMRA, ImVec2(200, 200), uv_min, uv_max, tint_col, border_col);
		ImGui::Text("Reflection texture");
		ImGui::Image((void*)(intptr_t)WaterRenderer::Get().m_waterBuffers->GetReflectionDepthTexture(), ImVec2(200, 200), uv_min, uv_max, tint_col, border_col);
		ImGui::Text("Refraction texture");
		ImGui::Image((void*)(intptr_t)WaterRenderer::Get().m_waterBuffers->GetRefractionDepthTexture(), ImVec2(200, 200), uv_min, uv_max, tint_col, border_col);


	}
	ImGui::End();
	
	// Entity Window
	if (m_entityWindow == true)
	{
		bool deleteFlag = false;
		for (std::list<std::shared_ptr<Entity>>::iterator i = _entities.begin(); i != _entities.end(); ++i)
		{
			if ((*i) == m_currentEntity)
			{
				ImVec2 size{ 300.0f, 600.0f };
				ImVec2 pos{(float)_width - size.x, 0.0f};
				if (m_resetEntityWindowSize)
				{
					ImGui::SetNextWindowSize(size);
					ImGui::SetNextWindowPos(pos);
					m_resetEntityWindowSize = false;
				}

				ImGui::Begin((*i)->GetName().c_str(), &m_entityWindow, ImGuiWindowFlags_MenuBar);
				if (ImGui::BeginMenuBar())
				{
					if (ImGui::BeginMenu("Menu"))
					{
						if (ImGui::MenuItem("Delete Entity")) deleteFlag = true;
						if (ImGui::MenuItem("Rename Entity")) menu_popup_action = "rename_popup";

						ImGui::EndMenu();
					}
					ImGui::EndMenuBar();
				}

				std::list<std::shared_ptr<Component>> components = (*i)->GetComponents();
				std::shared_ptr<Component> componentToDelete = nullptr;
				for (std::list<std::shared_ptr<Component>>::iterator j = components.begin(); j != components.end(); ++j)
				{
					std::string componentName = typeid(*(*j)).name();
					ImGui::PushID(componentName.c_str());
					componentName = componentName.substr(componentName.find_first_of(" \t")+1);
					if (ImGui::CollapsingHeader(componentName.c_str()))
					{
						(*j)->OnShowUI();
						if (ImGui::SmallButton("Remove Component"))
							componentToDelete = (*j);
					}
					if(componentName == "Renderer")
						(*i)->GetComponent<Renderer>()->DrawOutline();

					ImGui::Separator();
					ImGui::PopID();
				}
				if (componentToDelete!= nullptr)
					(*i)->RemoveComponent(componentToDelete);

				if (ImGui::Button("Add Component"))
					ImGui::OpenPopup("components_popup");
				if (ImGui::BeginPopup("components_popup"))
				{
					ImGui::Text("Components");
					ImGui::Separator();
					// Prints a list of all of the component names
					for (auto it = Component::protoTable().cbegin(); it != Component::protoTable().cend(); ++it)
					{
						if (ImGui::Selectable((it->first).c_str()))
						{
							(*i)->AddComponentPrototype((it->first));
						}
					}
					ImGui::EndPopup();
				}

				if (menu_popup_action == "rename_popup") ImGui::OpenPopup("rename_popup");
				if (ImGui::BeginPopup("rename_popup"))
				{
					ImGui::Text("New Name");
					ImGui::Separator();
					static char newName[64] = ""; ImGui::InputText("", newName, 64);
					if (ImGui::Button("confirm") && newName[0] != NULL)
					{
						currentScene->NameEntity(newName, (*i));
						m_resetEntityWindowSize = true;
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
				}
				ImGui::End();
			}
		}
		if (deleteFlag == true)
			currentScene->entities.remove(m_currentEntity);
	}
}

void DebugUIManager::Display()
{
	//Render imgui
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DebugUIManager::End()
{
	//Shutdown imgui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}