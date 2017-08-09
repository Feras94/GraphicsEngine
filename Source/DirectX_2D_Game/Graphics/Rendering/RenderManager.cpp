#include "pch.h"

#include "RenderManager.h"

#include <DirectXCollision.h>
using DirectX::BoundingFrustum;
using DirectX::BoundingBox;
using DirectX::BoundingSphere;
using DirectX::ContainmentType;

#include "GraphicsDevice.h"
#include "Effects/Effect.h"
#include "ShadowMap.h"
#include "RenderRequest.h"

#include "Graphics/Material.h"
#include "Graphics/Mesh.h"

#include "Components/Camera.h"

#include "Level/Level.h"

#include "Engine/SettingsManager.h"
#include "Resources/ResCache.h"

#include <3rd Party/imgui/imgui.h>

RenderManager::RenderManager() : _pGraphicsDevice(nullptr), _pEffect(nullptr)
{
	HandleRegistering(true);
}

RenderManager::~RenderManager()
{
	HandleRegistering(false);
}

bool RenderManager::Initialize(HWND hWnd)
{
	_pGraphicsDevice.reset(DBG_NEW GraphicsDevice);
	auto br = _pGraphicsDevice->Initialize(hWnd);
	if (!br)
	{
		LOG_E("Failed to initialize the graphics device", 0);
		return false;
	}

	_pRenderSettings.reset(DBG_NEW RenderSettings);
	_pEffect.reset(DBG_NEW Effect(_pRenderSettings));
	br = _pEffect->Init("Assets\\Effects\\MainEffect.fx", _pGraphicsDevice.get());
	if (!br)
	{
		LOG_E("Failed to initialize the effect", 0);
		return false;
	}

	_pShadowMap.reset(DBG_NEW ShadowMap(2048));

	// TODO load settings

	return true;
}

void RenderManager::OnRender(bool renderGui)
{
	FUNC_PROFILE();

	// we cant render if we don't have an active camera
	if (_cameras.size() == 0)
		return;

	// we first must get a reference to the current level
	auto currLevel = Level::GetCurrent();
	Assert(currLevel != nullptr, "Current Level Can't Be Null");
	
	_pGraphicsDevice->Clear();

	for (auto weakCamera : _cameras)
	{
		// converting the camera weak pointer to a strong pointer
		auto camera = weakCamera.lock();
		// if the camera is not enabled we don't need to render the scene
		if (!camera->IsEnabled())
			return;

		// now would be a good time to sort the list, but we are not doing that yet
		// TODO sort rendering requests into different sets for fast rendering
		_pEffect->PrepareFrame(_lightsMap, camera, currLevel->AmbientColor());

		_pEffect->SetTechnique(RenderTechnique::ShadowTech);
		RenderToShadowMap(camera);

		_pEffect->SetTechnique(RenderTechnique::MainTech);
		RenderToBackBuffer(camera);
	}

	if (renderGui)
		ImGui::Render();
	
	_pGraphicsDevice->Present();
}

void RenderManager::RenderToShadowMap(std::shared_ptr<Camera> camera)
{
	FUNC_PROFILE();

	if (!_pRenderSettings->ShadowsEnabled)
		return;

	_pShadowMap->SetAsActiveTarget();

	Render([](std::shared_ptr<Material> mat, std::shared_ptr<SubMesh> subMesh)
	{ 
		return mat->GetIsLit() && mat->GetCastShadows();
	}, camera);
}

void RenderManager::RenderToBackBuffer(std::shared_ptr<Camera> camera)
{
	FUNC_PROFILE();

	_pGraphicsDevice->SetDefaultTargets();
	_pGraphicsDevice->SetDefaultStates();
	_pGraphicsDevice->SetCullState(_pRenderSettings->WireframeEnabled ? CullState::Wireframe : CullState::CounterClockWise);
	_pGraphicsDevice->SetViewPort(camera->GetViewPort());

	_pEffect->SetShadowMap(_pShadowMap);
	Render([](std::shared_ptr<Material> mat, std::shared_ptr<SubMesh> subMesh) { return true; }, camera);
	_pEffect->RemoveShadowMap();
}

void RenderManager::Render(std::function< bool(std::shared_ptr<Material>, std::shared_ptr<SubMesh>) > validationCallback, std::shared_ptr<Camera> camera)
{
	for (auto it = _renderRequests.cbegin(); it != _renderRequests.cend(); ++it)
	{
		if (it->expired())
		{
			it = _renderRequests.erase(it);
			if (it == _renderRequests.cend())
				return;

			continue;
		}

		auto renderRequestPtr = it->lock();
		assert(renderRequestPtr);

		if (ValidateRenderRequest(renderRequestPtr, camera))
		{
			auto meshHandle = ResCache::GetPtr()->GetHandle(renderRequestPtr->GetMeshName());
			assert(meshHandle);
			std::shared_ptr<Mesh> meshPtr = std::static_pointer_cast<Mesh>(meshHandle->GetResource().lock());
			assert(meshPtr);
			auto transform = renderRequestPtr->GetTransform().lock();
			assert(transform);

			for (size_t i = 0; i < renderRequestPtr->GetMaterialsCount() && i < meshPtr->GetSubMeshesCount(); ++i)
			{
				auto mat = renderRequestPtr->GetMaterial(i);
				auto subMesh = meshPtr->GetSubMesh(i);

				if (!IsVisible(subMesh, transform, camera))
					continue;

				if (!validationCallback(mat, subMesh))
					continue;

				ProcessDrawItem(mat, subMesh, transform);
			}

			if (meshPtr->GetSubMeshesCount() > renderRequestPtr->GetMaterialsCount())
			{
				for (size_t i = renderRequestPtr->GetMaterialsCount(); i < meshPtr->GetSubMeshesCount(); ++i)
				{
					auto mat = Material::GetDefault();
					auto subMesh = meshPtr->GetSubMesh(i);

					if (!IsVisible(subMesh, transform, camera))
						continue;

					if (!validationCallback(mat, subMesh))
						continue;

					ProcessDrawItem(mat, subMesh, transform);
				}
			}

			if (meshPtr->GetSubMeshesCount() < renderRequestPtr->GetMaterialsCount())
				LOG_M("too much materials for mesh : " + renderRequestPtr->GetMeshName());
		}
	}
}

void RenderManager::ProcessDrawItem(std::shared_ptr<Material> mat, std::shared_ptr<SubMesh> subMesh, std::shared_ptr<Transform> transform)
{
	FUNC_PROFILE();

	_pEffect->UpdateMaterial(mat);
	_pEffect->UpdateMatrices(transform);
	if (_pEffect->Apply())
		_pGraphicsDevice->Draw(subMesh);
	else
		LOG_E("failed to upload data to the GPU", 0);
}

bool RenderManager::ValidateRenderRequest(StrongRenderRequestPtr renderRequest, std::shared_ptr<Camera> camera) const
{
	FUNC_PROFILE();

	if (renderRequest->GetMeshName().empty() || renderRequest->GetTransform().expired() || renderRequest->GetOwner().expired())
	{
		LOG_M("Detected a render request with missing parameters");
		return false;
	}

	auto ownerComponent = renderRequest->GetOwner().lock();
	assert(ownerComponent);

	if (!ownerComponent->IsEnabled() || !ownerComponent->GetOwner().lock()->IsEnabled())
	{
		return false;
	}

	return true;
}

bool RenderManager::IsVisible(std::shared_ptr<SubMesh> subMesh, std::shared_ptr<Transform> transform, std::shared_ptr<Camera> camera)
{
	FUNC_PROFILE();

	// frustum creation
	BoundingFrustum camViewFrustum = camera->GetViewFrustum();

	camViewFrustum.Transform(camViewFrustum, camera->GetViewMatrix().Invert());

	// culling
	if (subMesh->GetBoundingShapeType() == BoundingShapeType::Box)
	{
		BoundingBox box;
		subMesh->GetBoundingBox()->Transform(box, transform->GetWorldMat());

		auto containmentType = camViewFrustum.Contains(box);
		return containmentType == ContainmentType::CONTAINS || containmentType == ContainmentType::INTERSECTS;
	}
	if (subMesh->GetBoundingShapeType() == BoundingShapeType::Sphere)
	{
		BoundingSphere sphere;
		subMesh->GetBoundingSphere()->Transform(sphere, transform->GetWorldMat());

		auto containmentType = camViewFrustum.Contains(sphere);
		return containmentType == ContainmentType::CONTAINS || containmentType == ContainmentType::INTERSECTS;
	}

	LOG_M("Found a strange bounding shape");
	return true;
}


bool RenderManager::HandleEvent(StrongEventDataPtr eventData)
{
	if (eventData->GetID() == Event_NewRenderRequest::kEventID)
	{
		auto casted = std::dynamic_pointer_cast<Event_NewRenderRequest>(eventData);
		// this should never fire
		assert(casted != nullptr);

		_renderRequests.emplace_back(casted->GetRenderRequest());
	}
	else if (eventData->GetID() == Event_NewCamera::kEventID)
	{
		std::shared_ptr<Event_NewCamera> casted = std::dynamic_pointer_cast<Event_NewCamera>(eventData);
		assert(casted != nullptr);

		_cameras.push_back(casted->GetCamera());
	}
	else if (eventData->GetID() == Event_NewLight::kEventID)
	{
		std::shared_ptr<Event_NewLight> casted = std::dynamic_pointer_cast<Event_NewLight>(eventData);
		assert(casted != nullptr);

		_lightsMap.emplace_back(casted->LightPtr());
	}
	else if (eventData->GetID() == Event_SavingSettings::kEventID)
	{
		auto casted = std::dynamic_pointer_cast<Event_SavingSettings>(eventData);
		assert(casted);

		auto settingsManager = casted->GetSettingsManager().lock();
		assert(settingsManager);
		// TODO save settings here
		//settingsManager->SaveSetting("Graphics", "ShadowsEnabled", _bUsingShadows);
	}
	else
		// we should not receive any other events
		assert(false);

	return false;
}

void RenderManager::HandleRegistering(bool isRegistering)
{
	ToggleRegisteration(Event_NewRenderRequest::kEventID, isRegistering);
	ToggleRegisteration(Event_NewCamera::kEventID, isRegistering);
	ToggleRegisteration(Event_NewLight::kEventID, isRegistering);
	ToggleRegisteration(Event_SavingSettings::kEventID, isRegistering);
}
