#pragma once
#include<vulkan\vulkan.h>
#include "VDeleter.h"
#include<vector>
#include"Window.h"
namespace GameEngine
{

	struct QueueFamilyIndices
	{
		int displayFamily = -1;

		bool isComplete()
		{
			return displayFamily >= 0;
		}
	};

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentMode;
	};

	class Engine
	{
	public:
		Engine();
		~Engine();

		void InitVulkan();

		Window windowobject;

	private:
		void CreateInstance();
		
		bool CheckValidationLayerSupport();

		std::vector<const char*> GetRequiredExtensions();

		void SetupDebugCallback();
		void CreateSurface();
		void GetPhysicalDevices();
		int RateDeviceSuitability(VkPhysicalDevice deviceToRate);
		void CreateLogicalDevice();
		void CreateSwapChain();
		void CreateImageViews();
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilites);

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
		const std::vector<const char*> validationLayers = { "VK_LAYER_LUNARG_standard_validation" };

		const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
		///Vulkan Handles
		VDeleter<VkInstance> instance{ vkDestroyInstance };

		VDeleter<VkDebugReportCallbackEXT> callBack{instance, DestroyDebugReportCallBackEXT };
		
		VDeleter<VkSurfaceKHR> surface{instance, vkDestroySurfaceKHR};

		VkPhysicalDevice physcialDevice = VK_NULL_HANDLE;

		//needs to be under the instance
		VDeleter<VkDevice>logicalDevice{ vkDestroyDevice };

		VkQueue displayQueue;

		VDeleter<VkSwapchainKHR> swapChain{logicalDevice, vkDestroySwapchainKHR};

		std::vector<VkImage> swapChainImages;

		std::vector<VDeleter<VkImageView>>swapChainIMageViews;
		
		//store swap chain details
		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;

		//only happens in debug
#ifdef NDEBUG
		const bool enableValidationLayers = false;
#else
		const bool enableValidationLayers = true;
#endif
	};
}

