#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <stdexcept>
#include <iostream>

const int windowWidth = 800;
const int windowHeight = 600;

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class HelloTriangleApplication
{
private:
	GLFWwindow* _window;
	VkInstance _instance;
	VkDebugReportCallbackEXT _callback;
	VkDevice _device;
	VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
	VkQueue _graphicsQueue;
	VkSurfaceKHR _surface;
	VkQueue _presentQueue;

	const std::vector<const char*> validationLayers = {
		"VK_LAYER_LUNARG_standard_validation"
	};

	struct QueueFamilyIndices {
		int graphicsFamily = -1;
		int presentFamily = -1;

		bool isComplete() {
			return graphicsFamily >= 0 && presentFamily >= 0;
		}
	};

public:
	void Run();

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugReportFlagBitsEXT flags,
		VkDebugReportObjectTypeEXT objType,
		uint64_t obj,
		size_t location,
		int32_t code,
		const char * layerPrefix,
		const char * msg,
		void * userData)
	{
		std::cerr << "Validation layer: " << msg << std::endl;

		return VK_FALSE;
	}
private:
	// Main Workers
	void InitWindow();
	void InitVulcan();
	void MainLoop();
	void CleanUp();

	// Helpers
	void CreateInstance();
	void ExtensionSupportCheck();
	bool CheckValidationLayerSupport();
	void SetupDebugCallback();
	void PickPhysicalDevice();
	std::vector<const char*> GetRequiredExtensions();
	void CreateLogicalDevice();
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice);
	bool IsDeviceSuitable(VkPhysicalDevice device);
	void CreateSurface();

};