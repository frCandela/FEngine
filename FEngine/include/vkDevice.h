#pragma once

#pragma once

#include "AllInclude.h"

class Device {
public:
	Device( VkInstance _vkInstance ) {
		Create( _vkInstance );
		GetQueueFamily();
	}

	VkPhysicalDevice vkPhysicalDevice = VK_NULL_HANDLE;
	VkDevice vkDevice = VK_NULL_HANDLE;

private:
	VkPhysicalDeviceFeatures m_availableFeatures;
	VkPhysicalDeviceProperties m_deviceProperties;
	std::vector<VkExtensionProperties> m_availableExtensions;
	std::vector<VkQueueFamilyProperties> m_queueFamilyProperties;
	std::vector<VkLayerProperties> m_availableLayers;
	uint32_t m_graphicsQueueFamilyIndex;
	VkQueue m_graphicsQueue;
	VkQueue m_presentQueue;

	bool Create(VkInstance _vkInstance) {

		SelectPhysicalDevice(_vkInstance);
		std::vector< const char * > existingExtensions = GetDesiredExtensions({ "VK_KHR_swapchain" });

		float queuePriority = 1.0f;
		std::vector <VkDeviceQueueCreateInfo> queueCreateInfos;

		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.pNext = nullptr;
		queueCreateInfo.flags = 0;
		queueCreateInfo.queueFamilyIndex = m_graphicsQueueFamilyIndex;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);

		VkPhysicalDeviceFeatures desiredFeatures = {};
		desiredFeatures.samplerAnisotropy = m_availableFeatures.samplerAnisotropy == VK_TRUE;

		const std::vector<const char*> validationLayers = GetDesiredValidationLayers({"VK_LAYER_LUNARG_standard_validation" });
#ifdef NDEBUG
		const bool enableValidationLayers = false;
#else
		const bool enableValidationLayers = true;
#endif

		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pNext = nullptr;
		deviceCreateInfo.flags = 0;
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>( queueCreateInfos.size() );
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>( validationLayers.size() );
		deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>( existingExtensions.size() );
		deviceCreateInfo.ppEnabledExtensionNames = existingExtensions.data();
		deviceCreateInfo.pEnabledFeatures = &desiredFeatures;

		if (vkCreateDevice(vkPhysicalDevice, &deviceCreateInfo, nullptr, &vkDevice) != VK_SUCCESS) {
			return false;
		}

		return true;
	}
	bool SelectPhysicalDevice( VkInstance _vkInstance ) {
		uint32_t devicesCount;
		if (vkEnumeratePhysicalDevices(_vkInstance, &devicesCount, nullptr) != VK_SUCCESS) { return false; }
		std::vector< VkPhysicalDevice> availableDevices(devicesCount);
		if (vkEnumeratePhysicalDevices(_vkInstance, &devicesCount, availableDevices.data()) != VK_SUCCESS) { return false; }

		for (int deviceIndex = 0; deviceIndex < availableDevices.size(); deviceIndex++) {
			vkPhysicalDevice = availableDevices[deviceIndex];

			uint32_t extensionsCount;
			if (vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &extensionsCount, nullptr) != VK_SUCCESS) { return false; }
			m_availableExtensions = std::vector<VkExtensionProperties>(extensionsCount);
			if (vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &extensionsCount, m_availableExtensions.data()) != VK_SUCCESS) { return false; }

			vkGetPhysicalDeviceProperties(vkPhysicalDevice, &m_deviceProperties);
			vkGetPhysicalDeviceFeatures(vkPhysicalDevice, &m_availableFeatures);

			if (m_deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
				break;
			}
		}

		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);		
		m_availableLayers = std::vector<VkLayerProperties>(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, m_availableLayers.data());

		std::cout << "Using device : " << m_deviceProperties.deviceName << std::endl;
		return true;
	}
	std::vector < const char *> GetDesiredExtensions(const std::vector < const char *> _desiredExtensions) {
		std::vector < const char *> existingExtensions;
		existingExtensions.reserve(_desiredExtensions.size());

		for (int extensionIndex = 0; extensionIndex < _desiredExtensions.size(); extensionIndex++) {
			if (IsExtensionAvailable(_desiredExtensions[extensionIndex])) {
				existingExtensions.push_back(_desiredExtensions[extensionIndex]);
			}
		}
		return existingExtensions;
	}
	bool IsExtensionAvailable( std::string _requiredExtension) {
		for (int availableExtensionIndex = 0; availableExtensionIndex < m_availableExtensions.size(); availableExtensionIndex++) {
			if (_requiredExtension.compare(m_availableExtensions[availableExtensionIndex].extensionName) == 0) {
				return true;
			}
		}
		return false;
	}
	bool IsLayerAvailable(std::string _requiredLayer) {
		for (int availableLayerIndex = 0; availableLayerIndex < m_availableLayers.size(); availableLayerIndex++) {
			if (_requiredLayer.compare(m_availableLayers[availableLayerIndex].layerName) == 0) {
				return true;
			}
		}
		return false;
	}
	bool IsFeatureAvailable(std::string _requiredFeature) {
		return false;
	}
	void GetQueueFamily() {
		uint32_t queueFamiliesCount;
		vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamiliesCount, nullptr);
		m_queueFamilyProperties = std::vector<VkQueueFamilyProperties>(queueFamiliesCount);
		vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamiliesCount, m_queueFamilyProperties.data());

		VkQueueFlags desiredCapabilities = VK_QUEUE_GRAPHICS_BIT;
		for (int queueIndex = 0; queueIndex < m_queueFamilyProperties.size(); queueIndex++) {
			if ((m_queueFamilyProperties[queueIndex].queueCount > 0) && 
				(m_queueFamilyProperties[queueIndex].queueFlags & desiredCapabilities)) {
				m_graphicsQueueFamilyIndex = queueIndex;
				break;
			}
		}

		vkGetDeviceQueue(vkDevice, m_graphicsQueueFamilyIndex, 0, &m_graphicsQueue);
	}
	// Checks if all of the requested layers are available
	std::vector < const char *> GetDesiredValidationLayers(const std::vector < const char *> _desiredLayers)
	{		
		std::vector < const char *> existingLayers;
		existingLayers.reserve(_desiredLayers.size());
		for (int layerIndex = 0; layerIndex < _desiredLayers.size(); layerIndex++) {
			if (IsLayerAvailable(_desiredLayers[layerIndex])) {
				existingLayers.push_back(_desiredLayers[layerIndex]);
			}
		}

		return existingLayers;
	}
};