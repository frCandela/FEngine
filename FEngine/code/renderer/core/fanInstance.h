#pragma once

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class Instance {
	public:
		Instance();
		~Instance();

		const std::vector < const char *> & GetValidationLayers() const { return m_validationLayers; }

		VkInstance vkInstance = VK_NULL_HANDLE;

	private:
		std::vector< VkExtensionProperties > m_availableExtensions;
		std::vector<VkLayerProperties> m_availableLayers;
		VkDebugReportCallbackEXT m_callback;

		std::vector < const char *> m_validationLayers;
		std::vector < const char *> m_extensions;

		bool IsExtensionAvailable(std::string _requiredExtension);
		bool IsLayerAvailable(std::string _requiredLayer);
		void SetDesiredValidationLayers(const std::vector < const char *> _desiredLayers);
		void SetDesiredExtensions(const std::vector < const char *> _desiredExtensions);
		bool SetupDebugCallback();
		void DestroyDebugReportCallback(VkDebugReportCallbackEXT _callback, const VkAllocationCallbacks* _pAllocator);
		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
			VkDebugReportFlagsEXT _flags,
			VkDebugReportObjectTypeEXT _objType,
			uint64_t _obj,
			size_t _location,
			int32_t _code,
			const char* _layerPrefix,
			const char* msg,
			void* _userData);
	};
}