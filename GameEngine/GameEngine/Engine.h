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

		void SetupDebugCallback();

		VkResult CreateDebugReportCallBackEXT(
			VkInstance instance,
			const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
			const VkAllocationCallbacks* pAllocator,
			VkDebugReportCallbackEXT* pCallback

		) {
			auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
			if (func != nullptr)
			{
				return func(instance, pCreateInfo, pAllocator, pCallback);
			}
			else
			{
				return VK_ERROR_EXTENSION_NOT_PRESENT;
			}
		};
		static void DestroyDebugReportCallBackEXT(
			VkInstance instance, 
			VkDebugReportCallbackEXT callback,
			const VkAllocationCallbacks* pAllocator
			)
		{
			auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");

			if (func != nullptr)
			{
				func(instance, callback, pAllocator);
			}
		};

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

		VDeleter<VkDebugReportCallbackEXT> callBack{instance, DestroyDebugReportCallBackEXT };
		const std::vector<const char*> validationLayers = {"VK_LAYER_LUNARG_standard_validation"};
		//only happens in debug
#ifdef NDEBUG
		const bool enableValidationLayers = false;
#else
		const bool enableValidationLayers = true;
#endif
	};
}

