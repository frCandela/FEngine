#pragma once

#include "AllInclude.h"

class Instance {
public:
	Instance() {
		Create();
	}

	VkInstance vkInstance = VK_NULL_HANDLE;

private:
	std::vector< VkExtensionProperties > m_availableExtensions;
	VkDebugReportCallbackEXT m_callback;

	bool Create() {

		GetAvailableExtensions();

		// Get desired extensions
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		std::vector< const char * > desiredExtensions = { VK_EXT_DEBUG_REPORT_EXTENSION_NAME };
		for (unsigned glfwExtensionIndex = 0; glfwExtensionIndex < glfwExtensionCount; glfwExtensionIndex++) {
			desiredExtensions.push_back(glfwExtensions[glfwExtensionIndex]);
		}

		std::vector< const char * > existingExtensions;
		existingExtensions.reserve(desiredExtensions.size());
		for (int extensionIndex = 0; extensionIndex < desiredExtensions.size(); extensionIndex++) {
			if (IsExtensionAvailable(desiredExtensions[extensionIndex])) {
				existingExtensions.push_back(desiredExtensions[extensionIndex]);
			}
		}

		VkApplicationInfo appInfo;
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "FEngine Editor";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "FEngine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

		VkInstanceCreateInfo instanceCreateInfo;
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pNext = nullptr;
		instanceCreateInfo.flags = 0;
		instanceCreateInfo.pApplicationInfo = &appInfo;
		instanceCreateInfo.enabledLayerCount = 0;
		instanceCreateInfo.ppEnabledLayerNames = nullptr;
		instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(existingExtensions.size());
		instanceCreateInfo.ppEnabledExtensionNames = existingExtensions.size() > 0 ? existingExtensions.data() : nullptr;

		if (vkCreateInstance(&instanceCreateInfo, nullptr, &vkInstance) != VK_SUCCESS || vkInstance == VK_NULL_HANDLE) {
			return false;
		}

		SetupDebugCallback();

		return true;
	}
	bool IsExtensionAvailable(std::string _requiredExtension) {
		for (int availableExtensionIndex = 0; availableExtensionIndex < m_availableExtensions.size(); availableExtensionIndex++) {
			if (_requiredExtension.compare(m_availableExtensions[availableExtensionIndex].extensionName) == 0) {
				return true;
			}
		}
		return false;
	}
	bool GetAvailableExtensions() {
		uint32_t extensions_count;

		if (vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, nullptr) != VK_SUCCESS) {
			return false;
		}

		m_availableExtensions = std::vector< VkExtensionProperties >(extensions_count);
		if (vkEnumerateInstanceExtensionProperties(nullptr, &extensions_count, m_availableExtensions.data()) != VK_SUCCESS) {
			return false;
		}

		return true;
	}
	bool SetupDebugCallback(){
		VkDebugReportCallbackCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
		createInfo.pfnCallback = DebugCallback;

		auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(vkInstance, "vkCreateDebugReportCallbackEXT");
		if (func != nullptr)
			return func(vkInstance, &createInfo , nullptr, &m_callback );
		else
			return false;
	}
	void DestroyDebugReportCallback(VkDebugReportCallbackEXT _callback, const VkAllocationCallbacks* _pAllocator)	{
		PFN_vkDestroyDebugReportCallbackEXT func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(vkInstance, "vkDestroyDebugReportCallbackEXT");
		if (func != nullptr) {
			func(vkInstance, _callback, _pAllocator);
		}
	}
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback( VkDebugReportFlagsEXT _flags, VkDebugReportObjectTypeEXT _objType,	uint64_t _obj, size_t _location,int32_t _code,const char* _layerPrefix,const char* _msg,void* _userData){
		(void)_flags;
		(void)_objType;
		(void)_obj;
		(void)_location;
		(void)_code;
		(void)_layerPrefix;
		(void)_userData;

		std::cerr << "validation layer: " << _msg << std::endl;
		return VK_FALSE;
	}
};