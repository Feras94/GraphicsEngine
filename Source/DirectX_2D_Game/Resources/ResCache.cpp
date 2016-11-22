#include "ResCache.h"

#include "IResourceLoader.h"
#include "ResHandle.h"
#include "Resource.h"

#include "Debugging/Debug.h"
#include "Utilities/Utils.h"

std::shared_ptr<ResHandle> ResCache::GetHandle(const string& resourceName)
{
	auto handle = FindHandle(resourceName);
	if (handle)
		return handle;

	handle = LoadResource(resourceName);
	if (!handle)
		LOG_E("Failed to load the resource : " + resourceName, 0);

	return handle;
}

void ResCache::RegisterLoader(const string& pattern, std::shared_ptr<IResourceLoader> loader)
{
	auto findIt = _loadersMap.find(pattern);
	if (findIt != _loadersMap.end())
		LOG_M("The Resource loader with the pattern: " + pattern + "Already exists");

	_loadersMap.emplace(std::make_pair(pattern, loader));
}

std::shared_ptr<ResHandle> ResCache::FindHandle(const string& resourceName)
{
	auto findIt = _handlesMap.find(resourceName);
	if (findIt == _handlesMap.end())
		return nullptr;
	else
		return findIt->second;
}

std::shared_ptr<ResHandle> ResCache::LoadResource(const string& resourceName)
{
	LOG_M("Loading resource : " + resourceName);

	
	auto loader = FindLoader(resourceName);
	if (!loader)
		LOG_E("Could not file a loader for resource : " + resourceName, 0);

	auto resource = loader->LoadResource(resourceName);
	if (!resource)
		LOG_E("Could not load the resource : " + resourceName, 0);

	std::shared_ptr<ResHandle> handle(DBG_NEW ResHandle);
	handle->_pOwnerCache = this;
	handle->_pResource = resource;

	_handlesMap.emplace(std::make_pair(resourceName, handle));

	return handle;
}

std::shared_ptr<IResourceLoader> ResCache::FindLoader(const string& resourceName)
{
	std::shared_ptr<IResourceLoader> loader;

	for (auto it = _loadersMap.begin(); it != _loadersMap.end(); ++it)
	{
		if (Utils::WildcardMatch(it->first, resourceName))
		{
			loader = it->second;
			break;
		}
	}

	return loader;
}