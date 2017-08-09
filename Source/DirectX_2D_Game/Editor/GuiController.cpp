﻿#include "pch.h"

#include "GuiController.h"

#include <3rd Party/imgui/ImGuizmo.h>
#include <Components/Transform.h>
#include <Components/Camera.h>
#include <Actors/ActorFactory.h>
#include <Components/ComponentFactory.h>
#include <Components/MeshRenderer.h>
#include <Graphics/Material.h>

#include "3rd Party/imgui/imgui_impl_dx11.h"
#include "Graphics/Rendering/GraphicsDevice.h"

#include <Level/Level.h>

#include <Debugging/Logger.h>

#include <Input/Input.h>
#include "AssetManager.h"
#include "Components/ScriptComponent.h"
#include <Components/AudioComponent.h>

namespace ImGuizmo {
	struct matrix_t;
}

inline void SetupImGuiStyle(bool bStyleDark_, float alpha_)
{
	ImGuiStyle& style = ImGui::GetStyle();

	// light style from Pacôme Danhiez (user itamago) https://github.com/ocornut/imgui/pull/511#issuecomment-175719267
	style.Alpha = 1.0f;
	style.FrameRounding = 3.0f;
	style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.86f, 0.86f, 0.86f, 0.99f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(0.59f, 0.59f, 0.59f, 0.50f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

	if (bStyleDark_)
	{
		for (int i = 0; i < ImGuiCol_COUNT; i++)
		{
			ImVec4& col = style.Colors[i];
			float H, S, V;
			ImGui::ColorConvertRGBtoHSV(col.x, col.y, col.z, H, S, V);

			if (S < 0.1f)
			{
				V = 1.0f - V;
			}
			ImGui::ColorConvertHSVtoRGB(H, S, V, col.x, col.y, col.z);
			if (col.w < 1.00f)
			{
				col.w *= alpha_;
			}
		}
	}
	else
	{
		for (int i = 0; i < ImGuiCol_COUNT; i++)
		{
			ImVec4& col = style.Colors[i];
			if (col.w < 1.00f)
			{
				col.x *= alpha_;
				col.y *= alpha_;
				col.z *= alpha_;
				col.w *= alpha_;
			}
		}
	}
}

Core::GuiController::GuiController() : _isDarkTheme(true), _windowsAlpha(0.9f), _bShowAboutWindow(false), _selectedActorId(-1), _windowWidth(-1), _windowHeight(-1)
{
	HandleRegistering(true);
}

Core::GuiController::~GuiController()
{
	HandleRegistering(false);
}

bool Core::GuiController::Initialize(std::weak_ptr<IWindow> window, std::weak_ptr<Level> level, std::weak_ptr<ActorFactory> actorFactory)
{
	_pWindow = window;
	_pLevel = level;
	_pActorFactory = actorFactory;

	_pAssetManager.reset(DBG_NEW AssetManager);
	_pAssetManager->LoadAssets();

	auto win = window.lock();
	auto device = GraphicsDevice::GetPtr();

	auto result = ImGui_ImplDX11_Init(win->GetHandle(), device->GetDevice().Get(), device->GetContext().Get());

	auto windowInformation = win->GetInformation();
	_windowWidth = windowInformation.Width;
	_windowHeight = windowInformation.Heigt;

	SetupImGuiStyle(_isDarkTheme, _windowsAlpha);

	ImGuiIO& io = ImGui::GetIO();
	//io.Fonts->AddFontDefault();
	io.Fonts->AddFontFromFileTTF("Assets/Fonts/Roboto-Regular.ttf", 14.f);

	return result;
}

void Core::GuiController::Shutdown()
{
	ImGui_ImplDX11_Shutdown();
}

void Core::GuiController::Render()
{
	FUNC_PROFILE();

	SetupImGuiStyle(_isDarkTheme, _windowsAlpha);

	ImGui_ImplDX11_NewFrame();
	ImGuizmo::BeginFrame();

	ImGui::ShowTestWindow();
	
	DrawMenuBar();
	DrawActorsWindow();
	DrawAssetsWindow();
	DrawLogWindow();
	DrawPropertiesWindow();
	
	DrawExtraWindows();
}

bool Core::GuiController::HandleEvent(StrongEventDataPtr eventData)
{
	auto eventId = eventData->GetID();
	if (eventId == Event_WindowResized::kEventID)
	{
		auto casted = std::dynamic_pointer_cast<Event_WindowResized>(eventData);

		_windowWidth = casted->Width();
		_windowHeight = casted->Height();
	}
	else if (eventId == Event_NewCamera::kEventID)
	{
		auto casted = std::dynamic_pointer_cast<Event_NewCamera>(eventData);

		_pCamera = casted->GetCamera();
	}

	return true;
}

void Core::GuiController::HandleRegistering(bool isRegistering)
{
	ToggleRegisteration(Event_WindowResized::kEventID, isRegistering);
	ToggleRegisteration(Event_NewCamera::kEventID, isRegistering);
}

void Core::GuiController::DrawMenuBar()
{
	FUNC_PROFILE();

	if (ImGui::BeginMainMenuBar())
	{
		DrawFileMenu();
		DrawViewMenu();
		DrawActorsMenu();
		DrawComponentsMenu();
		DrawHelpMenu();		

		ImGui::EndMainMenuBar();
	}
}

void Core::GuiController::DrawFileMenu() const
{
	FUNC_PROFILE();
	
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("New", "CTRL+N"))
		{
			LOG_M("New Pressed");
		}

		if (ImGui::MenuItem("Save", "CTRL+S"))
		{
			LOG_M("Save Pressed");
			auto level = _pLevel.lock();
			level->SaveLevel();
		}

		ImGui::Separator();
		if (ImGui::MenuItem("Exit", "ALT+F4"))
		{
			auto window = _pWindow.lock();
			PostMessage(window->GetHandle(), WM_CLOSE, 0, 0);
			LOG_M("Exit Pressed");
		}

		ImGui::EndMenu();
	}
}

void Core::GuiController::DrawViewMenu()
{
	FUNC_PROFILE();
	
	if (ImGui::BeginMenu("View"))
	{
		ImGui::MenuItem("Use Dark Theme", nullptr, &_isDarkTheme);
		ImGui::SliderFloat("Windows Alpha", &_windowsAlpha, 0.f, 1.f);

		ImGui::EndMenu();
	}
}

void Core::GuiController::DrawActorsMenu()
{
	FUNC_PROFILE();
	
	auto actor = _pSelectedActor.lock();
	if (ImGui::BeginMenu("Actors"))
	{
		if (ImGui::BeginMenu("Create New"))
		{
			auto factory = _pActorFactory.lock();
			auto level = _pLevel.lock();

			auto actor = std::shared_ptr<Actor>();

			if (ImGui::MenuItem("Empty Actor"))
				actor = factory->CreateEmptyActor();

			ImGui::Separator();

			if (ImGui::MenuItem("Box Actor"))
				actor = factory->CreateBoxActor();
			if (ImGui::MenuItem("Sphere Actor"))
				actor = factory->CreateSphereActor();
			if (ImGui::MenuItem("Cylinder Actor"))
				actor = factory->CreateCylinderActor();
			if (ImGui::MenuItem("Plane Actor"))
				actor = factory->CreatePlaneActor();

			ImGui::Separator();

			if (ImGui::MenuItem("Directional Light"))
				actor = factory->CreateDirectionalLight();
			if (ImGui::MenuItem("Spot Light"))
				actor = factory->CreateSpotLight();
			if (ImGui::MenuItem("Point Light"))
				actor = factory->CreatePointLight();

			if (actor)
			{
				level->AddActor(actor);
				_pSelectedActor = actor;
				_selectedActorId = actor->GetId().GetUnderlyingValue();
			}

			ImGui::EndMenu();
		}

		if (actor)
		{
			if (ImGui::MenuItem("Delete", "DELETE"))
			{
				auto level = _pLevel.lock();
				level->DestroyActor(actor->GetId());

				_pSelectedActor.reset();
				_selectedActorId = -1;
			}
		}

		ImGui::EndMenu();
	}
}

void Core::GuiController::DrawComponentsMenu() const
{
	FUNC_PROFILE();
	
	auto actor = _pSelectedActor.lock();
	if (actor)
	{
		if (ImGui::BeginMenu("Components"))
		{
			if (ImGui::BeginMenu("Add New"))
			{
				auto componentFactory = _pActorFactory.lock()->GetComponentFactory();
				auto& components = componentFactory->GetRegisteredComponents();

				for (auto& component : components)
				{
					if (ImGui::MenuItem(component.c_str()))
					{
						auto com = componentFactory->CreateComponent(component);
						actor->AddComponent(com);
					}
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}
	}
}

void Core::GuiController::DrawHelpMenu()
{
	FUNC_PROFILE();

	if (ImGui::BeginMenu("Help"))
	{
		if (ImGui::MenuItem("About", "CTRL+I"))
		{
			_bShowAboutWindow = true;
		}
		

		ImGui::EndMenu();
	}
}

void Core::GuiController::DrawActorsWindow()
{
	FUNC_PROFILE();

	auto level = _pLevel.lock();
	if (!level) return;

	const auto& actors = level->GetActors();


	ImGui::SetNextWindowPos(ImVec2(0, 25));
	ImGui::SetNextWindowSize(ImVec2(_windowWidth * 0.15f, _windowHeight - 30));

	ImGuiWindowFlags windowFlags = 0;
	windowFlags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("Actors", nullptr, windowFlags);

	for (auto actor : actors)
	{
		const auto& name = actor->GetName();

		// gizmo testing

		if (actor->GetId().GetUnderlyingValue() == _selectedActorId)
		{
			auto camera = _pCamera.lock();
			if (!camera) continue;;

			static ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
			static ImGuizmo::MODE mode = ImGuizmo::WORLD;

			if (!Input::IsMouseButtonDown(MouseKeys::RightButton))
			{
				if (Input::IsKeyPressed(KeyCode::Q))
					operation = ImGuizmo::TRANSLATE;
				else if (Input::IsKeyPressed(KeyCode::W))
					operation = ImGuizmo::ROTATE;
				else if (Input::IsKeyPressed(KeyCode::E))
					operation = ImGuizmo::SCALE;

				if (Input::IsKeyPressed(KeyCode::Z))
					mode = ImGuizmo::WORLD;
				else if (Input::IsKeyPressed(KeyCode::X))
					mode = ImGuizmo::LOCAL;
			}
			auto transform = actor->GetTransform().lock();

			auto position = transform->GetPosition();
			auto rotation = transform->GetRotation();
			auto scale = transform->GetScale();


			auto matrix = transform->GetWorldMat();
			auto mat = (float*)matrix.m;

			auto& io = ImGui::GetIO();

			const auto& viewMatrix = camera->GetViewMatrix();
			const auto& projectionMatrix = camera->GetProjectionMatrix();

			auto view = (float*)viewMatrix.m;
			auto projection = (float*)projectionMatrix.m;

			ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
			ImGuizmo::Manipulate(view, projection, operation, mode, mat);

			matrix.Decompose(scale, rotation, position);

			if (operation == ImGuizmo::TRANSLATE)
				transform->SetPosition(position);
			else if (operation == ImGuizmo::ROTATE)
				transform->SetRotation(rotation);
			else
				transform->SetScale(scale);
		}


		// end gizmo testing

		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | (_selectedActorId == actor->GetId().GetUnderlyingValue() ? ImGuiTreeNodeFlags_Selected : 0);
		ImGui::TreeNodeEx(actor.get(), node_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen, name.c_str());
		if (ImGui::IsItemClicked())
		{
			_selectedActorId = actor->GetId().GetUnderlyingValue();
			_pSelectedActor = actor;
		}
	}

	ImGui::End();
}

void Core::GuiController::DrawAssetsWindow()
{
}

void Core::GuiController::DrawLogWindow()
{
}

static void toEulerianAngle(const Quaternion& q, float& roll, float& pitch, float& yaw)
{
	double ysqr = q.y * q.y;

	// roll (x-axis rotation)
	double t0 = +2.0 * (q.w * q.x + q.y * q.z);
	double t1 = +1.0 - 2.0 * (q.x * q.x + ysqr);
	roll = std::atan2(t0, t1);

	// pitch (y-axis rotation)
	double t2 = +2.0 * (q.w * q.y - q.z * q.x);
	t2 = t2 > 1.0 ? 1.0 : t2;
	t2 = t2 < -1.0 ? -1.0 : t2;
	pitch = std::asin(t2);

	// yaw (z-axis rotation)
	double t3 = +2.0 * (q.w * q.z + q.x * q.y);
	double t4 = +1.0 - 2.0 * (ysqr + q.z * q.z);
	yaw = std::atan2(t3, t4);
}

// callback to get the combo list items (to work with vector<string>)
bool get_combo_item(void* data, int index, const char** outdata)
{
	auto list = static_cast<std::vector<string>*>(data);
	*outdata = (*list)[index].c_str();
	return true;
}

void Core::GuiController::DrawPropertiesWindow()
{
	FUNC_PROFILE();

	auto actor = _pSelectedActor.lock();
	if (!actor) return;

	auto width = _windowWidth * 0.35f;
	auto height = _windowHeight;

	ImGui::SetNextWindowPos(ImVec2(_windowWidth - width, 25));
	ImGui::SetNextWindowSize(ImVec2(width, height - 30));

	ImGuiWindowFlags windowFlags = 0;
	windowFlags |= ImGuiWindowFlags_NoResize;

	ImGui::Begin("Properties", nullptr, windowFlags);

	auto name = actor->GetName();

	if (ImGui::CollapsingHeader("Actor", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Name");
		ImGui::SameLine(100);

		auto name = actor->GetName();
		name.reserve(255);
		ImGui::InputText("##name", &name[0], 255);
		actor->SetName(name);

		ImGui::Text("Enabled");
		ImGui::SameLine(100);
		auto enabled = actor->IsEnabled();
		ImGui::Checkbox("##enabled", &enabled);
		actor->SetEnabled(enabled);
	}

	auto& components = actor->GetComponents();
	std::vector<ObjectId> componentsToRemove;
	for (auto component : components)
	{
		bool componentOpen = true;
		if (ImGui::CollapsingHeader(component->GetName().c_str(), &componentOpen, ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (component->GetTypeID() == MeshRenderer::kComponentID)
			{
				std::shared_ptr<MeshRenderer> mesh_renderer = std::dynamic_pointer_cast<MeshRenderer>(component);
				DrawMeshRendererProperties(mesh_renderer);
			}
			if (component->GetTypeID() == ScriptComponent::kComponentID)
			{
				auto scriptComponent = std::dynamic_pointer_cast<ScriptComponent>(component);
				DrawScriptProperties(scriptComponent);
			}
			else
			{
				auto& props = component->GetProperties();
				for (auto& prop : props)
				{
					ImGui::Text(prop.name);
					ImGui::SameLine(100);

					// TEMP DATA
					string* str;
					char* buff;

					float roll, pitch, yaw;
					float vec[3];


					// END TEMP DATA

					ImGui::PushID(prop.value);
					switch (prop.type)
					{
					case PropertyType::Float:
						ImGui::DragFloat("", (float*)prop.value, prop.minValue, prop.maxValue);
						break;

					case PropertyType::Int:
						ImGui::DragInt("", (int*)prop.value, prop.minValue, prop.maxValue);
						break;

					case PropertyType::Bool:
						ImGui::Checkbox("", (bool*)prop.value);
						break;

					case PropertyType::Color:
						ImGui::ColorEdit4("", (float*)prop.value, false);
						break;

					case PropertyType::String:
						str = (string*)prop.value;
						buff = &((*str)[0]);
						ImGui::InputText("", buff, 255);
						break;

					case PropertyType::Quaternion:

						toEulerianAngle(*(Quaternion*)prop.value, roll, pitch, yaw);

						vec[0] = DirectX::XMConvertToDegrees(roll);
						vec[1] = DirectX::XMConvertToDegrees(pitch);
						vec[2] = DirectX::XMConvertToDegrees(yaw);

						ImGui::DragFloat3("", vec);

						roll = DirectX::XMConvertToRadians(vec[0]);
						pitch = DirectX::XMConvertToRadians(vec[1]);
						yaw = DirectX::XMConvertToRadians(vec[2]);

						//*(Quaternion*)prop.value = Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll);
						//actor->GetTransform().lock()->SetRotation(Quaternion::CreateFromYawPitchRoll(vec[2], vec[1], vec[0]));

						break;

					case PropertyType::Vector2:
						ImGui::DragFloat2("", (float*)prop.value, 0.1, prop.minValue, prop.maxValue);
						break;

					case PropertyType::Vector3:
						ImGui::DragFloat3("", (float*)prop.value, 0.1, prop.minValue, prop.maxValue);
						break;

					case PropertyType::Vector4:
						ImGui::DragFloat4("", (float*)prop.value, 0.1, prop.minValue, prop.maxValue);
						break;

					default:
						LOG_W("Unknown property type");
					}
					ImGui::PopID();
				}
			}

			if (component->GetTypeID() == AudioComponent::kComponentID)
			{
				auto audioComponent = std::dynamic_pointer_cast<AudioComponent>(component);
				auto& sounds = _pAssetManager->GetSounds();

				ImGui::Text("Audio File");
				ImGui::SameLine(100);
				
				auto index = std::find(sounds.begin(), sounds.end(), audioComponent->GetAudioFilePath());
				auto selected = std::distance(sounds.begin(), index);
				
				ImGui::PushID(component.get());
				
				if (ImGui::Combo("", &selected, get_combo_item, (void*)&sounds, sounds.size()))
					if (selected < sounds.size()) audioComponent->SetAudioFilePath(sounds[selected]);
				
				ImGui::PopID();

				if (ImGui::Button("Play", ImVec2(100, 25)))
					audioComponent->Play();
				ImGui::SameLine(120);
				
				if (ImGui::Button("Pause", ImVec2(100, 25)))
					audioComponent->Pause();
				ImGui::SameLine(240);

				if (ImGui::Button("Stop", ImVec2(100, 25)))
					audioComponent->Stop();
			}
		}

		if (component->GetTypeID() != Transform::kComponentID && !componentOpen) 
			componentsToRemove.push_back(component->GetId());
	}

	for (auto componentId : componentsToRemove)
		actor->RemoveComponent(componentId);


	ImGui::End();
}

void Core::GuiController::DrawScriptProperties(std::shared_ptr<ScriptComponent> script_component)
{
	auto& scripts = _pAssetManager->GetScripts();

	ImGui::Text("Enabled");
	ImGui::SameLine(100);
	auto enabled = script_component->IsEnabled();
	ImGui::PushID(&enabled);
	if (ImGui::Checkbox("", &enabled))
		script_component->SetEnabled(enabled);

	ImGui::PopID();

	ImGui::Text("Script");
	ImGui::SameLine(100);
	auto scriptFile = script_component->GetScriptFilePath();
	auto index = std::find(scripts.begin(), scripts.end(), scriptFile);
	auto selected = std::distance(scripts.begin(), index);
	ImGui::PushID(script_component.get());
	if (ImGui::Combo("", &selected, get_combo_item, (void*)&scripts, scripts.size()))
	{
		if (selected < scripts.size())
		{
			script_component->SetScriptFilePath(scripts[selected]);
			script_component->ReloadScript();
		}
	}

	ImGui::PopID();

	// TODO add id for this button
	if (ImGui::Button("Reload Script"))
	{
		script_component->ReloadScript();
	}
}

void Core::GuiController::DrawMeshRendererProperties(std::shared_ptr<MeshRenderer> mesh_renderer)
{
	auto& meshes = _pAssetManager->GetMeshes();
	
	ImGui::Text("Enabled");
	ImGui::SameLine(100);
	auto isEnabled = mesh_renderer->IsEnabled();
	if (ImGui::Checkbox("##mesh_renderer_enabled", &isEnabled))
		mesh_renderer->SetEnabled(isEnabled);
	
	ImGui::Text("Mesh File");
	ImGui::SameLine(100);
	auto index = std::find(meshes.begin(), meshes.end(), mesh_renderer->GetMeshFilePath());
	auto selected = std::distance(meshes.begin(), index);
	if (ImGui::Combo("##mesh_renderer_file_path", &selected, get_combo_item, (void*)&meshes, meshes.size()))
		if (selected < meshes.size()) mesh_renderer->SetMeshFilePath(meshes[selected]);
	

	ImGui::Separator();
	ImGui::Text("Materials");
	ImGui::SameLine(100);
	if (ImGui::Button("Add New Material"))
	{
		mesh_renderer->AddMaterial(std::shared_ptr<Material>(DBG_NEW Material));
	}

	const auto& materials = mesh_renderer->GetMaterials();
	std::vector<ObjectId> materialsToRemove;
	for (auto itr = materials.begin(); itr < materials.end(); ++itr)
	{
		auto material = *itr;

		ImGui::PushID(material.get());
		bool open = true;
		if (ImGui::CollapsingHeader(material->GetName().c_str(), &open, ImGuiTreeNodeFlags_DefaultOpen))
		{
			DrawMaterialProperties(material);
		}
		if (!open) materialsToRemove.push_back(material->GetId());
		ImGui::PopID();
	}

	for (auto materialId : materialsToRemove)
	{
		mesh_renderer->RemoveMaterial(materialId);
	}
}

void Core::GuiController::DrawMaterialProperties(std::shared_ptr<Material> material) const
{
	auto& textures = _pAssetManager->GetTextures();

	ImGui::Text("Diffuse Color");
	ImGui::SameLine(125);
	ImGui::PushID(&material->_DiffuseColor);
	ImGui::ColorEdit4("", (float*)&material->_DiffuseColor, false);
	ImGui::PopID();

	ImGui::Text("Specular Color");
	ImGui::SameLine(125);
	ImGui::PushID(&material->_SpecularColor);
	ImGui::ColorEdit4("", (float*)&material->_SpecularColor, false);
	ImGui::PopID();

	ImGui::Text("Specular Power");
	ImGui::SameLine(125);
	ImGui::PushID(&material->_SpecularPower);
	ImGui::DragFloat("", &material->_SpecularPower, 1, 0, 1000);
	ImGui::PopID();

	ImGui::Text("Ambient Color");
	ImGui::SameLine(125);
	ImGui::PushID(&material->_AmbientColor);
	ImGui::ColorEdit4("", (float*)&material->_AmbientColor, false);
	ImGui::PopID();

	ImGui::Text("Is Lit");
	ImGui::SameLine(125);
	ImGui::PushID(&material->_IsLit);
	ImGui::Checkbox("", &material->_IsLit);
	ImGui::PopID();

	ImGui::Text("Is Textured");
	ImGui::SameLine(125);
	ImGui::PushID(&material->_IsTextured);
	ImGui::Checkbox("", &material->_IsTextured);
	ImGui::PopID();

	ImGui::Text("Texture Scale");
	ImGui::SameLine(125);
	ImGui::PushID(&material->_TextureScale);
	ImGui::DragFloat2("", (float*)&material->_TextureScale, 1, 1, 1000);
	ImGui::PopID();

	ImGui::Text("Texture Offset");
	ImGui::SameLine(125);
	ImGui::PushID(&material->_TextureOffset);
	ImGui::DragFloat2("", (float*)&material->_TextureOffset, 0.1, -1, 1);
	ImGui::PopID();

	ImGui::Text("Texture Rotation");
	ImGui::SameLine(125);
	ImGui::PushID(&material->_TextureRotation);
	ImGui::DragFloat("", &material->_TextureRotation, 1, -360, 360);
	ImGui::PopID();

	ImGui::Text("Texture Path");
	ImGui::SameLine(125);
	ImGui::PushID(&material->_TexturePath);
	auto index = std::find(textures.begin(), textures.end(), material->_TexturePath);
	auto selected = std::distance(textures.begin(), index);
	if (ImGui::Combo("", &selected, get_combo_item, (void*)&textures, textures.size()))
		if (selected < textures.size()) material->SetTexturePath(textures[selected]);
	ImGui::PopID();

	ImGui::Text("Use Specular Texture");
	ImGui::SameLine(125);
	ImGui::PushID(&material->_UseSpecularTexture);
	ImGui::Checkbox("", &material->_UseSpecularTexture);
	ImGui::PopID();

	ImGui::Text("Specular Texture");
	ImGui::SameLine(125);
	ImGui::PushID(&material->_SpecularTexturePath);
	index = std::find(textures.begin(), textures.end(), material->_SpecularTexturePath);
	selected = std::distance(textures.begin(), index);
	if (ImGui::Combo("", &selected, get_combo_item, (void*)&textures, textures.size()))
		if (selected < textures.size()) material->SetSpecularTexturePath(textures[selected]);
	ImGui::PopID();

	ImGui::Text("Use Normal Map");
	ImGui::SameLine(125);
	ImGui::PushID(&material->_UseNormalMap);
	ImGui::Checkbox("", &material->_UseNormalMap);
	ImGui::PopID();

	ImGui::Text("Normal Map Texture");
	ImGui::SameLine(125);
	ImGui::PushID(&material->_NormalMapTexturePath);
	index = std::find(textures.begin(), textures.end(), material->_NormalMapTexturePath);
	selected = std::distance(textures.begin(), index);
	if (ImGui::Combo("", &selected, get_combo_item, (void*)&textures, textures.size()))
		if (selected < textures.size()) material->SetNormalMapTexturePath(textures[selected]);
	ImGui::PopID();

	ImGui::Text("Cast Shadows");
	ImGui::SameLine(125);
	ImGui::PushID(&material->_CastShadows);
	ImGui::Checkbox("", &material->_CastShadows);
	ImGui::PopID();

	ImGui::Text("Receive Shadows");
	ImGui::SameLine(125);
	ImGui::PushID(&material->_ReceiveShadows);
	ImGui::Checkbox("", &material->_ReceiveShadows);
	ImGui::PopID();
}

void Core::GuiController::DrawExtraWindows()
{
	if (_bShowAboutWindow)
	{
		////ImGui::SetNextWindowPos(ImVec2(200, 200));
		//ImGuiWindowFlags windowFlags = ImGuiWindowFlags_Modal | ImGuiWindowFlags_AlwaysAutoResize;
		//ImGui::Begin("About", &_bShowAboutWindow, windowFlags);
		//ImGui::Text("Hello and welcome to our great editor");
		//ImGui::End();

		ImGui::OpenPopup("About");
		if (ImGui::BeginPopupModal("About", &_bShowAboutWindow, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Welcome to the great Graphics Engine");
			ImGui::Text("Version: 1.0.0");
			ImGui::Separator();


			if (ImGui::Button("OK", ImVec2(150, 20)))
			{
				_bShowAboutWindow = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}	
}
