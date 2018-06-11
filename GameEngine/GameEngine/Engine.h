#pragma once
#include<vulkan\vulkan.h>
#include "VDeleter.h"
#include<vector>
namespace GameEngine
{
	class Engine
	{
	public:
		Engine();
		~Engine();

		void InitVulkan();

	private:
		void CreateInstance();
		
		bool CheckValidationLayerSupport();

		std::vector<const char*> GetRequiredExtensions();

		VkResult CreateDebugReportCallBackEXT();
		static void DestroyDebugReportCallBackEXT();

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugReportFlagsEXT flags,
			VkDebugReportObjectTypeEXT objType,
			uint64_t obj,
			size_t location,
			int32_t code,
			const char* layerPrefix,
			const char* msg,
			void* userData
		)
		{
			std::cerr << "Validation Layer: " << msg << std::endl;
			return VK_FALSE;
		
		
		};

		///Vulkan Handles
		VDeleter<VkInstance> instance{ vkDestroyInstance };

		VDeleter<VkDebugReportCallbackEXT> callBack{instance}
		const std::vector<const char*> validationLayers = {"VK_LAYER_LUNARG_standard_validation"};
		//only happens in debug
#ifdef NDEBUG
		const bool enableValidationLayers = false;
#else
		const bool enableValidationLayers = true;
#endif
	};
}

