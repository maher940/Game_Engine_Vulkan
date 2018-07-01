#include "Engine.h"
#define GLFW_INCLUDE_VULKAN
#include<GLFW\glfw3.h>
#include<cstring>
#include<map>
#include <algorithm>
namespace GameEngine
{
	Engine::Engine()
	{
	}


	Engine::~Engine()
	{
	}

	void Engine::InitVulkan()
	{
		windowobject.Create();
		CreateInstance();
		SetupDebugCallback();
		CreateSurface();
		GetPhysicalDevices();
		CreateLogicalDevice();
		CreateSwapChain();
		CreateImageViews();
	}
	void Engine::CreateInstance()
	{
		//check for validation layer availability
		if (enableValidationLayers && !CheckValidationLayerSupport())
		{
			throw std::runtime_error("Validation Layers requested but not available");
		}
		VkApplicationInfo appinfo = {};
		appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appinfo.pNext = nullptr;
		appinfo.pApplicationName = "Game Engine";
		appinfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appinfo.pEngineName = "No Engine";
		appinfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appinfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo = {};

		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.pApplicationInfo = &appinfo;

		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = validationLayers.size();
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
			createInfo.ppEnabledLayerNames = nullptr;
		}
		

		auto extensions = GetRequiredExtensions();
		createInfo.enabledExtensionCount = extensions.size();
		createInfo.ppEnabledExtensionNames = extensions.data();

		if (vkCreateInstance(&createInfo, nullptr, instance.replace()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create instance");
		}
		else
		{
			std::cout << "Vulkan Instance Created" << std::endl;
		}
	}

	bool Engine::CheckValidationLayerSupport()
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		std::vector<VkLayerProperties> aviLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, aviLayers.data());

		for (const char* layerName : validationLayers)
		{
			bool layerFound = false;
			for (const auto &layerProperties : aviLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
			{
				return false;
			}
		}
		return true;
	}

	std::vector<const char*> Engine::GetRequiredExtensions()
	{
		std::vector<const char*> extensions;
		unsigned int glfwExtenstionCount = 0;

		const char**glfwExtensions;

		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtenstionCount);


		for (unsigned int i = 0; i < glfwExtenstionCount; i++)
		{
			extensions.push_back(glfwExtensions[i]);
		}
		if (enableValidationLayers)
		{
			
			extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		}

		return extensions;
	}

	void Engine::SetupDebugCallback()
	{
		if (!enableValidationLayers)
		{
			return;
		}
		VkDebugReportCallbackCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
		createInfo.pfnCallback = debugCallback;

		if (CreateDebugReportCallBackEXT(instance, &createInfo, nullptr, callBack.replace()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to setup debug callback");
		}
		else
		{
			std::cout << "Debug Callback setup successful" << std::endl;
		}
	}
	void Engine::CreateSurface()
	{
		if (glfwCreateWindowSurface(instance,windowobject.window, nullptr, surface.replace()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create window surface");
		}
		else
		{
			std::cout << "Window Surface Created" << std::endl;
		}
	}
	void Engine::GetPhysicalDevices()
	{
		uint32_t physcialDeviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &physcialDeviceCount, nullptr);
		if (physcialDeviceCount == 0)
		{
			throw std::runtime_error("no devices found with Vulkan Support");
		}

		std::vector<VkPhysicalDevice> foundPhysicalDevices(physcialDeviceCount);
		vkEnumeratePhysicalDevices(instance, &physcialDeviceCount, foundPhysicalDevices.data());
		
		std::multimap<int, VkPhysicalDevice> rankedDevices;

		for (const auto& currentDevice : foundPhysicalDevices)
		{
			int score = RateDeviceSuitability(currentDevice);
			rankedDevices.insert(std::make_pair(score, currentDevice));
		}


		if (rankedDevices.rbegin()->first > 0)
		{
			physcialDevice = rankedDevices.rbegin()->second;
			std:: cout << "Physcial Device found" << std::endl;
		}
		else
		{
			throw std::runtime_error("No Physical devices meet criteria");
		}
	}

	int Engine::RateDeviceSuitability(VkPhysicalDevice deviceToRate)
	{
		int score = 0;
		//finds a queuefamily that meets our needs
		QueueFamilyIndices indices = FindQueueFamilies(deviceToRate);
		bool extensionsSupported = CheckDeviceExtensionSupport(deviceToRate);
		if (!indices.isComplete() || !extensionsSupported)
		{
			return 0;
		}

		bool swapChainAdequate = false;
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(deviceToRate);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentMode.empty();

		if (!swapChainAdequate)
		{
			return 0;
		}

		VkPhysicalDeviceFeatures deviceFeatures;
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(deviceToRate,&deviceProperties);
		vkGetPhysicalDeviceFeatures(deviceToRate,&deviceFeatures);

		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			score += 1000;
		}
		score += deviceProperties.limits.maxImageDimension2D;

		if (!deviceFeatures.geometryShader)
		{
			return 0;
		}

		return score;

	}

	void Engine::CreateLogicalDevice()
	{
		QueueFamilyIndices indices = FindQueueFamilies(physcialDevice);


		VkDeviceQueueCreateInfo queueCreateInfo = {};

		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.pNext = nullptr;
		queueCreateInfo.flags = 0;
		queueCreateInfo.queueFamilyIndex = indices.displayFamily;
		queueCreateInfo.queueCount = 1;
		const float queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority;


		VkPhysicalDeviceFeatures deviceFeatures = {};
		VkDeviceCreateInfo createInfo = {};

		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.queueCreateInfoCount = 1;
		createInfo.pQueueCreateInfos = &queueCreateInfo;

		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = validationLayers.size();
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
			createInfo.ppEnabledLayerNames = nullptr;
		}
		
		createInfo.enabledExtensionCount = deviceExtensions.size();
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();
		createInfo.pEnabledFeatures = &deviceFeatures;

		if (vkCreateDevice(physcialDevice,&createInfo,nullptr, logicalDevice.replace()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create logical device");
		}
		else
		{
			std::cout << "Logical Device created" << std::endl;
		}

		vkGetDeviceQueue(logicalDevice, indices.displayFamily, 0, &displayQueue);
	}

	void Engine::CreateSwapChain()
	{
		//get support detials for swaqp chain
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(physcialDevice);

		//use helper functions to get optimal settings

		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentMode);
		VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

		//fill in data
		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		//get proper image count
		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
		{
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		//might need to change this for steroscpic 3d games
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;
		//attempt to create swapchain
		if (vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, swapChain.replace()) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create swpa chain");
		}
		else
		{
			std::cout << "Swap Chain Created" << std::endl;
		}
		//populate swapchain image vector
		vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr);
		swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, swapChainImages.data());

		//stores data for chose surface foramt and extent
		swapChainImageFormat = surfaceFormat.format;
		swapChainExtent = extent;
	}

	void Engine::CreateImageViews()
	{
		swapChainIMageViews.resize(swapChainImages.size(), VDeleter<VkImageView>{logicalDevice, vkDestroyImageView});

		for (uint32_t i = 0; i < swapChainImages.size(); i++)
		{
			VkImageViewCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = swapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = swapChainImageFormat;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;


			if (vkCreateImageView(logicalDevice, &createInfo, nullptr, swapChainIMageViews[i].replace()) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to create image views");
			}
		}

		std::cout << "Image views created" << std::endl;
	}

	QueueFamilyIndices Engine::FindQueueFamilies(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;

		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;

		for (const auto &queueFamily : queueFamilies)
		{
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
			
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags && VK_QUEUE_GRAPHICS_BIT && presentSupport)
			{
				indices.displayFamily = i;
			}

			if (indices.isComplete())
			{
				break;
			}
			i++;
		}
		return indices;

	}
	bool Engine::CheckDeviceExtensionSupport(VkPhysicalDevice device)
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);

		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		for (const char* currrentExtension : deviceExtensions)
		{
			bool extensionFound = false;
			for (const auto& extention : availableExtensions)
			{
				//compares if the string is the same
				if (strcmp(currrentExtension, extention.extensionName) == 0)
				{
					extensionFound = true;
					break;
				}
			}
			if (!extensionFound)
			{
				return false;
			}
		}
		return true;
	}
	

	SwapChainSupportDetails Engine::QuerySwapChainSupport(VkPhysicalDevice device)
	{
		SwapChainSupportDetails details;


		//capabilites 
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

		//formats

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			details.formats.resize(formatCount);
		}
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());

		//presentModes

		uint32_t presentModesCount;

		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModesCount, nullptr);

		if (presentModesCount != 0)
		{
			details.presentMode.resize(presentModesCount);
		}
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModesCount, details.presentMode.data());
		return details;
	}

	VkSurfaceFormatKHR Engine::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		//if surface has not prefered format
		if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
		{
			return{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
		}

		for (const auto& currentFormat : availableFormats)
		{
			if (currentFormat.format == VK_FORMAT_B8G8R8A8_UNORM && currentFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return currentFormat;
			}
		}

		return availableFormats[0];
	}

	VkPresentModeKHR Engine::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
	{
		for (const auto& currentMode : availablePresentModes)
		{
			if (currentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return currentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D Engine::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilites)
	{
		if (capabilites.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return capabilites.currentExtent;
		}
		else
		{
			VkExtent2D actualExtent = { windowobject.window_width ,windowobject.window_height };
			actualExtent.width = std::max(capabilites.minImageExtent.width, std::min(capabilites.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(capabilites.minImageExtent.height, std::min(capabilites.maxImageExtent.height, actualExtent.height));
			return actualExtent;
		}
	}
}

