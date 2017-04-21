#include "pch.h"

#include "LightComponent.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

#include "Graphics/Material.h"
#include "Graphics/Rendering/GraphicsDevice.h"

/*
 *
 *Light Component
 *
 **/

#pragma region Light Component

void LightComponent::Initialize()
{
	BaseComponent::Initialize();

	auto strongPtr = this->shared_from_this();
	WeakLightPtr weakPtr(strongPtr);
	StrongEventDataPtr eventPtr(DBG_NEW Event_NewLight(weakPtr, _pOwner->GetId()));
	EventManager::GetPtr()->QueueEvent(eventPtr);
}

void LightComponent::RegisterProperties()
{
	RegisterProperty("Strength", PropertyType::Float, &_Strength);
	RegisterProperty("DiffuseColor", PropertyType::Color, &_DiffuseColor);
}

TiXmlElement* LightComponent::ToXML() const
{
	auto element = BaseComponent::ToXML();
	
	TiXmlComment* comment = DBG_NEW TiXmlComment("LightType : Directional = 0, Point = 1, Spot = 2");
	TiXmlElement* lightTypeE = XmlHelper::ToXml("LightType", static_cast<int>(_LightType));
	TiXmlElement* strengthE = XmlHelper::ToXml("Strength", _Strength);
	TiXmlElement* diffuseColE = XmlHelper::ToXml("DiffuseColor", _DiffuseColor);

	element->LinkEndChild(comment);
	element->LinkEndChild(lightTypeE);
	element->LinkEndChild(diffuseColE);
	element->LinkEndChild(strengthE);

	return element;
}

void LightComponent::Initialize(TiXmlElement* xmlData)
{
	BaseComponent::Initialize(xmlData);

	int lightType;
	XmlHelper::FromXml(xmlData, "LightType", lightType);
	_LightType = static_cast<LightType>(lightType);

	XmlHelper::FromXml(xmlData, "Strength", _Strength);
	XmlHelper::FromXml(xmlData, "DiffuseColor", _DiffuseColor);
}

#pragma endregion

void DirectionalLight::Initialize(TiXmlElement* xmlData)
{
	LightComponent::Initialize(xmlData);

	XmlHelper::FromXml(xmlData, "ShadowStrength", _ShadowStrength);
}

void DirectionalLight::RegisterProperties()
{
	LightComponent::RegisterProperties();
	RegisterProperty("Shadow Strength", PropertyType::Float, &_ShadowStrength);
}

TiXmlElement* DirectionalLight::ToXML() const
{
	auto element = LightComponent::ToXML();
	element->LinkEndChild(XmlHelper::ToXml("ShadowStrength", _ShadowStrength));

	return element;
}

void PointLight::Initialize(TiXmlElement* xmlData)
{
	LightComponent::Initialize(xmlData);

	XmlHelper::FromXml(xmlData, "Falloff", _Falloff);
	XmlHelper::FromXml(xmlData, "Range", _Range);
}

void PointLight::RegisterProperties()
{
	LightComponent::RegisterProperties();

	RegisterProperty("Falloff", PropertyType::Float, &_Falloff);
	RegisterProperty("Range", PropertyType::Float, &_Range);
}

TiXmlElement* PointLight::ToXML() const
{
	auto element = LightComponent::ToXML();

	element->LinkEndChild(XmlHelper::ToXml("Falloff", _Falloff));
	element->LinkEndChild(XmlHelper::ToXml("Range", _Range));

	return element;
}

void SpotLight::Initialize(TiXmlElement* xmlData)
{
	LightComponent::Initialize(xmlData);

	XmlHelper::FromXml(xmlData, "ConeAngle", _ConeAngleDegrees);
	XmlHelper::FromXml(xmlData, "Falloff", _Falloff);
}

void SpotLight::RegisterProperties()
{
	LightComponent::RegisterProperties();

	RegisterProperty("Cone Angle", PropertyType::Float, &_ConeAngleDegrees);
	RegisterProperty("Falloff", PropertyType::Float, &_Falloff);
}

TiXmlElement* SpotLight::ToXML() const
{
	auto element = LightComponent::ToXML();

	element->LinkEndChild(XmlHelper::ToXml("Falloff", _Falloff));
	element->LinkEndChild(XmlHelper::ToXml("ConeAngle", _ConeAngleDegrees));

	return element;
}
