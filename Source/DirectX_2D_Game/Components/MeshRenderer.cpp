#include "pch.h"

#include "MeshRenderer.h"

#include "Resources/ResCache.h"
#include "Graphics/Material.h"
#include "Graphics/Mesh.h"
#include "Graphics/Rendering/RenderRequest.h"

MeshRenderer::MeshRenderer(const std::string& meshName): BaseComponent(STRING(MeshRenderer)), _MeshFilePath(meshName)
{
	_MeshFilePath.reserve(255);
}

void MeshRenderer::Initialize()
{
	BaseComponent::Initialize();

	if (_MeshFilePath.empty())
		LOG_M("Mesh renderer does not have any mesh to render");

	_RenderRequest.reset(DBG_NEW RenderRequest);
	_RenderRequest->SetMaterials(_Materials);
	_RenderRequest->SetMeshName(_MeshFilePath);
	_RenderRequest->SetOwner(this->shared_from_this());
	_RenderRequest->SetTransform(this->GetTransform());

	EventManager::GetPtr()->QueueEvent(StrongEventDataPtr(DBG_NEW Event_NewRenderRequest(_RenderRequest)));
}

void MeshRenderer::Initialize(TiXmlElement* xmlData)
{
	BaseComponent::Initialize(xmlData);

	XmlHelper::FromXml(xmlData, "MeshName", _MeshFilePath);

	int materialsCount = -1;
	const TiXmlElement* materialsE = nullptr;
	if (XmlHelper::FindChild(xmlData, "Materials", &materialsE))
	{
		materialsE->QueryIntAttribute("Count", &materialsCount);
		if (materialsCount != -1)
		{
			_Materials.reserve(materialsCount);
			for (auto matE = materialsE->FirstChildElement(); matE; matE = matE->NextSiblingElement())
			{
				auto matPtr = Material::CreateFromXML(const_cast<TiXmlElement*>(matE));
				_Materials.emplace_back(matPtr);
			}
		}
	}
}

void MeshRenderer::RegisterProperties()
{
	BaseComponent::RegisterProperties();
	
	_MeshFilePath.reserve(255);
	RegisterProperty("Mesh Name", Core::PropertyType::String, (void*)&_MeshFilePath);
}

void MeshRenderer::RemoveMaterial(ObjectId materialId)
{
	auto itr = std::find_if(_Materials.begin(), _Materials.end(),
		[&](std::shared_ptr<Material> material) { return material->GetId() == materialId; });

	if (itr != _Materials.end())
		_Materials.erase(itr);
}

TiXmlElement* MeshRenderer::ToXML() const
{
	auto element = BaseComponent::ToXML();

	// handle if material is a prefab or not
	element->LinkEndChild(XmlHelper::ToXml("MeshName", _MeshFilePath));

	TiXmlElement* materialsE = DBG_NEW TiXmlElement("Materials");
	materialsE->SetDoubleAttribute("Count", _Materials.size());
	for (auto mat : _Materials)
	{
		materialsE->LinkEndChild(mat->ToXML());
	}
	element->LinkEndChild(materialsE);

	return element;
}

void MeshRenderer::SetMeshFilePath(const std::string & meshFilePath)
{
	_MeshFilePath = meshFilePath;
	_RenderRequest->SetMeshName(meshFilePath);
}

void MeshRenderer::AddMaterial(std::shared_ptr<Material> material)
{
	if (material)
	{
		_Materials.push_back(material);
	}
	else
		LOG_W("Trying to add a null material");
}
