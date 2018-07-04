#pragma once

#include "Device.h"

class Instance
{
public:

	~Instance();

	VkInstance instance;
	VkDebugReportCallbackEXT callback;

	void createInstance();
	bool checkValidationLayerSupport();
	static void DestroyDebugReportCallback(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator);
	std::vector<const char*> getRequiredExtensions();
	void setupDebugCallback();
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugReportFlagsEXT flags,
		VkDebugReportObjectTypeEXT objType,
		uint64_t obj,
		size_t location,
		int32_t code,
		const char* layerPrefix,
		const char* msg,
		void* userData
	);
	VkResult CreateDebugReportCallback(
		VkInstance instance,
		const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugReportCallbackEXT* pCallback
	);


};