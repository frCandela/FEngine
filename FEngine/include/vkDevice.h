#pragma once

#pragma once

#include "AllInclude.h"

#include "vkInstance.h"

class Device {
public:
	Device( Instance & _instance ) {
		Create( _instance );
	}

	~Device() {
		vkDestroyDevice(vkDevice, nullptr);
	}

	VkPhysicalDevice vkPhysicalDevice = VK_NULL_HANDLE;
	VkDevice vkDevice = VK_NULL_HANDLE;

private:
	VkPhysicalDeviceFeatures m_availableFeatures;
	VkPhysicalDeviceProperties m_deviceProperties;
	std::vector<VkExtensionProperties> m_availableExtensions;
	std::vector<VkQueueFamilyProperties> m_queueFamilyProperties;
	uint32_t m_graphicsQueueFamilyIndex = 1000;
	uint32_t m_computeQueueFamilyIndex=1000;
	VkQueue m_graphicsQueue;
	VkQueue m_computeQueue;
	VkQueue m_presentQueue;

	bool Create( Instance & _instance) {
		SelectPhysicalDevice(_instance.vkInstance);
		GetQueueFamilies();
		std::vector< const char * > existingExtensions = GetDesiredExtensions({ VK_KHR_SWAPCHAIN_EXTENSION_NAME });

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

		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pNext = nullptr;
		deviceCreateInfo.flags = 0;
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>( queueCreateInfos.size() );
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>( _instance.GetValidationLayers().size() );
		deviceCreateInfo.ppEnabledLayerNames = _instance.GetValidationLayers().data();
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>( existingExtensions.size() );
		deviceCreateInfo.ppEnabledExtensionNames = existingExtensions.data();
		deviceCreateInfo.pEnabledFeatures = &desiredFeatures;

		if (vkCreateDevice(vkPhysicalDevice, &deviceCreateInfo, nullptr, &vkDevice) != VK_SUCCESS) {
			return false;
		}

		vkGetDeviceQueue(vkDevice, m_graphicsQueueFamilyIndex, 0, &m_graphicsQueue);
		vkGetDeviceQueue(vkDevice, m_computeQueueFamilyIndex, 0, &m_computeQueue);

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
	bool IsFeatureAvailable(std::string _requiredFeature) {
		return false;
	}
	void GetQueueFamilies() {
		uint32_t queueFamiliesCount;
		vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamiliesCount, nullptr);
		m_queueFamilyProperties = std::vector<VkQueueFamilyProperties>(queueFamiliesCount);
		vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamiliesCount, m_queueFamilyProperties.data());

		VkQueueFlags desiredGraphicsCapabilities = VK_QUEUE_GRAPHICS_BIT;
		VkQueueFlags desiredComputeCapabilities = VK_QUEUE_COMPUTE_BIT;

		for (int queueIndex = 0; queueIndex < m_queueFamilyProperties.size(); queueIndex++) {
			if ((m_queueFamilyProperties[queueIndex].queueCount > 0) && 
				(m_queueFamilyProperties[queueIndex].queueFlags & desiredGraphicsCapabilities)) {
				m_graphicsQueueFamilyIndex = queueIndex;
				break;
			}
		}
		for (int queueIndex = 0; queueIndex < m_queueFamilyProperties.size(); queueIndex++) {
			if ((m_queueFamilyProperties[queueIndex].queueCount > 0) &&
				(m_queueFamilyProperties[queueIndex].queueFlags & desiredComputeCapabilities)) {
				m_computeQueueFamilyIndex = queueIndex;
				break;
			}
		}
	}

};