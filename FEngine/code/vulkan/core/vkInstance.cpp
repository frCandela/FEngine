#include "fanIncludes.h"

#include "vulkan/core/vkInstance.h"

namespace vk {

	//================================================================================================================================
	//================================================================================================================================
	Instance::Instance() {
		// Get desired extensions
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwInit();
		int res = glfwVulkanSupported() == GLFW_TRUE;
		(void)res;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		std::vector< const char * > desiredExtensions = { VK_EXT_DEBUG_REPORT_EXTENSION_NAME, VK_KHR_SURFACE_EXTENSION_NAME };
		for (unsigned glfwExtensionIndex = 0; glfwExtensionIndex < glfwExtensionCount; glfwExtensionIndex++) {
			desiredExtensions.push_back(glfwExtensions[glfwExtensionIndex]);
		}
		SetDesiredExtensions(desiredExtensions);


#ifdef NDEBUG
		const std::vector<const char*> validationLayers = {};
#else	
		SetDesiredValidationLayers({
			 "VK_LAYER_LUNARG_standard_validation"
			 //,"VK_LAYER_LUNARG_assistant_layer"
			 ,"VK_LAYER_LUNARG_core_validation"
			/*,"VK_LAYER_KHRONOS_validation"
			,"VK_LAYER_LUNARG_monitor"
			,"VK_LAYER_LUNARG_object_tracker"
			,"VK_LAYER_LUNARG_screenshot"
			,"VK_LAYER_LUNARG_standard_validation"
			,"VK_LAYER_LUNARG_parameter_validation"*/
			});
#endif

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
		instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
		instanceCreateInfo.ppEnabledLayerNames = m_validationLayers.data();
		instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_extensions.size());
		instanceCreateInfo.ppEnabledExtensionNames = m_extensions.size() > 0 ? m_extensions.data() : nullptr;

		if (vkCreateInstance(&instanceCreateInfo, nullptr, &vkInstance) != VK_SUCCESS || vkInstance == VK_NULL_HANDLE) {
			std::cout << "ouch, this is going to be messy" << std::endl;
		}
		SetupDebugCallback();
	}

	//================================================================================================================================
	//================================================================================================================================
	Instance::~Instance() {
		DestroyDebugReportCallback(m_callback, nullptr);
		vkDestroyInstance(vkInstance, nullptr);
		vkInstance = VK_NULL_HANDLE;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Instance::IsExtensionAvailable(std::string _requiredExtension) {
		for (int availableExtensionIndex = 0; availableExtensionIndex < m_availableExtensions.size(); availableExtensionIndex++) {
			if (_requiredExtension.compare(m_availableExtensions[availableExtensionIndex].extensionName) == 0) {
				return true;
			}
		}
		return false;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Instance::IsLayerAvailable(std::string _requiredLayer) {
		for (int availableLayerIndex = 0; availableLayerIndex < m_availableLayers.size(); availableLayerIndex++) {
			if (_requiredLayer.compare(m_availableLayers[availableLayerIndex].layerName) == 0) {
				return true;
			}
		}
		return false;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Instance::SetDesiredValidationLayers(const std::vector < const char *> _desiredLayers)
	{
		// Get available layers
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		m_availableLayers.resize(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, m_availableLayers.data());

		m_validationLayers.clear();
		m_validationLayers.reserve(_desiredLayers.size());
		for (int layerIndex = 0; layerIndex < _desiredLayers.size(); layerIndex++) {
			if (IsLayerAvailable(_desiredLayers[layerIndex])) {
				m_validationLayers.push_back(_desiredLayers[layerIndex]);
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Instance::SetDesiredExtensions(const std::vector < const char *> _desiredExtensions)
	{
		// Get available extensions
		uint32_t extensionsCount;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr);
		m_availableExtensions.resize(extensionsCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, m_availableExtensions.data());

		m_extensions.clear();
		m_extensions.reserve(_desiredExtensions.size());
		for (int extensionIndex = 0; extensionIndex < _desiredExtensions.size(); extensionIndex++) {
			if (IsExtensionAvailable(_desiredExtensions[extensionIndex])) {
				m_extensions.push_back(_desiredExtensions[extensionIndex]);
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Instance::SetupDebugCallback() {
		VkDebugReportCallbackCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
		createInfo.pfnCallback = DebugCallback;

		auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(vkInstance, "vkCreateDebugReportCallbackEXT");
		if (func != nullptr && func(vkInstance, &createInfo, nullptr, &m_callback) == VK_SUCCESS) {
			std::cout << std::hex << "VkDebugCallback\t\t" << m_callback << std::dec << std::endl;
			return true;
		}

		return false;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Instance::DestroyDebugReportCallback(VkDebugReportCallbackEXT _callback, const VkAllocationCallbacks* _pAllocator) {
		PFN_vkDestroyDebugReportCallbackEXT func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(vkInstance, "vkDestroyDebugReportCallbackEXT");
		if (func != nullptr) {
			func(vkInstance, _callback, _pAllocator);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	VKAPI_ATTR VkBool32 VKAPI_CALL Instance::DebugCallback(VkDebugReportFlagsEXT _flags, VkDebugReportObjectTypeEXT _objType, uint64_t _obj, size_t _location, int32_t _code, const char* _layerPrefix, const char* _msg, void* _userData) {
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
}