#include "HelloTriangleApplication.h"
#include <stdexcept>
#include <vector>
#include <iostream>
#include <set>

VkResult CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback) {
	auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pCallback);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
	if (func != nullptr) {
		func(instance, callback, pAllocator);
	}
}

void HelloTriangleApplication::Run()
{
	InitWindow();
	InitVulcan();
	MainLoop();
	CleanUp();
}

void HelloTriangleApplication::InitWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	_window = glfwCreateWindow(windowWidth, windowHeight, "Vulkan", nullptr, nullptr);
}

void HelloTriangleApplication::InitVulcan()
{
	CreateInstance();

	// Debug callback returning null from vulkan.. :(
	SetupDebugCallback();
	CreateSurface();
	PickPhysicalDevice();
	CreateLogicalDevice();
}

void HelloTriangleApplication::MainLoop()
{
	while (glfwWindowShouldClose(_window) == false)
	{
		glfwPollEvents();
	}
}

void HelloTriangleApplication::CleanUp()
{
	// Clean up vulkan
	vkDestroyDevice(_device, nullptr);

	if (enableValidationLayers == true)
	{
		DestroyDebugReportCallbackEXT(_instance, _callback, nullptr);
	}

	vkDestroySurfaceKHR(_instance, _surface, nullptr);
	vkDestroyInstance(_instance, nullptr);

	// Clean up glfw
	glfwDestroyWindow(_window);
	glfwTerminate();
}


// Helpers
void HelloTriangleApplication::CreateInstance()
{
	if (enableValidationLayers && CheckValidationLayerSupport() == false)
	{
		throw std::runtime_error("Validation layers requested, but not available");
	}

	VkApplicationInfo appInfo = {};

	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	if (enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}


	auto extensions = GetRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkResult result = vkCreateInstance(&createInfo, nullptr, &_instance);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create vulkan instance");
	}

	ExtensionSupportCheck();
}

void HelloTriangleApplication::ExtensionSupportCheck()
{
	// Find number of extensions
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	// allocation array of appropriate size
	std::vector<VkExtensionProperties> extensions(extensionCount);

	// Query extension details
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	std::cout << "Available extensions:" << std::endl;

	for (const auto& extension : extensions)
	{
		std::cout << "\t" << extension.extensionName << std::endl;
	}

}

bool HelloTriangleApplication::CheckValidationLayerSupport()
{
	uint32_t layerCount;

	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers)
	{
		bool layerFound = false;

		for (const auto& layerPropertes : availableLayers)
		{
			if (strcmp(layerName, layerPropertes.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}

		if (layerFound == false)
		{
			return false;
		}
	}

	return true;
}

std::vector<const char*> HelloTriangleApplication::GetRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char ** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}

	return extensions;
}

void HelloTriangleApplication::CreateLogicalDevice()
{
	QueueFamilyIndices indices = FindQueueFamilies(_physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<int> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };

	float queuePriority = 1.0f;
	for (int queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = 0;

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create logical device!");
	}

	vkGetDeviceQueue(_device, indices.graphicsFamily, 0, &_graphicsQueue);
	vkGetDeviceQueue(_device, indices.presentFamily, 0, &_presentQueue);
}

void HelloTriangleApplication::SetupDebugCallback()
{
	if (enableValidationLayers == false)
	{
		return;
	}

	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_DEBUG_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = (PFN_vkDebugReportCallbackEXT)DebugCallback;

	if (CreateDebugReportCallbackEXT(_instance, &createInfo, nullptr, &_callback) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to set up debug callback!");
	}
}

void HelloTriangleApplication::PickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		throw std::runtime_error("Failed to find GPUs with Vulkan support!");
	}
	else
	{
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());

		for (const auto& device : devices)
		{
			if (IsDeviceSuitable(device))
			{
				_physicalDevice = device;
				break;
			}
		}

		if (_physicalDevice == VK_NULL_HANDLE)
		{
			throw std::runtime_error("Failed to find suitable GPU");
		}
	}
}

HelloTriangleApplication::QueueFamilyIndices HelloTriangleApplication::FindQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueCount > 0)
		{
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface, &presentSupport);

			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphicsFamily = i;
			}

			if (presentSupport)
			{
				indices.presentFamily = i;
			}
		}

		if (indices.isComplete())
		{
			break;
		}

		i++;
	}
	return indices;
}

bool HelloTriangleApplication::IsDeviceSuitable(VkPhysicalDevice device)
{
	// GEOMETRY SHADER
	//VkPhysicalDeviceProperties deviceProperties;
	//VkPhysicalDeviceFeatures deviceFeatures;
	//vkGetPhysicalDeviceProperties(device, &deviceProperties);
	//vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	//return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
	//	   deviceFeatures.geometryShader;
	QueueFamilyIndices indices = FindQueueFamilies(device);

	return indices.isComplete();
}

void HelloTriangleApplication::CreateSurface()
{
	if (glfwCreateWindowSurface(_instance, _window, nullptr, &_surface) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create window surface!");
	}
}
