#include "helper_vk.hpp"

#include <fstream>
#include <streambuf>

#include "thirdparty/loguru/loguru.hpp"

bool HelperVk::hasRequiredInstanceExtensions() {
	std::vector<vk::ExtensionProperties> extensionProperties = vk::enumerateInstanceExtensionProperties();

	LOG_F(INFO, "Enumerating Vulkan instance extensions: ");
	bool surfaceKHRSupport = false, wmKHRSupport = false;
	for (vk::ExtensionProperties extension : extensionProperties) {
		LOG_F(INFO, "\t%s", extension.extensionName);
		if (strcmp(extension.extensionName, VK_KHR_SURFACE_EXTENSION_NAME) == 0)
			surfaceKHRSupport = true;
		if (strcmp(extension.extensionName,
#ifdef VK_USE_PLATFORM_MIR_KHR
			VK_KHR_MIR_SURFACE_EXTENSION_NAME) == 0)
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
			VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME) == 0)
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME) == 0)
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
			VK_KHR_XLIB_SURFACE_EXTENSION_NAME) == 0)
#endif
			wmKHRSupport = true;
	}

	if (!surfaceKHRSupport || !wmKHRSupport) {
		LOG_F(FATAL, "Vulkan driver does not support rendering to a surface!");
		return false;
	}
	return true;
}

bool HelperVk::hasRequiredDeviceExtensionsAndFeatures(vk::PhysicalDevice physicalDevice) {
	// Grabbing for logging purposes.
	vk::PhysicalDeviceProperties properties = physicalDevice.getProperties();

	vk::PhysicalDeviceFeatures features = physicalDevice.getFeatures();
	LOG_F(INFO, "Enumerating Vulkan features for [%s]", properties.deviceName);
	LOG_F(INFO, "\talphaToOne: %u", features.alphaToOne);
	LOG_F(INFO, "\tdepthBiasClamp: %u", features.depthBiasClamp);
	LOG_F(INFO, "\tdepthBounds: %u", features.depthBounds);
	LOG_F(INFO, "\tdrawIndirectFirstInstance: %u", features.drawIndirectFirstInstance);
	LOG_F(INFO, "\tdualSrcBlend: %u", features.dualSrcBlend);
	LOG_F(INFO, "\tfillModeNonSolid: %u", features.fillModeNonSolid);
	LOG_F(INFO, "\tfragmentStoresAndAtomics: %u", features.fragmentStoresAndAtomics);
	LOG_F(INFO, "\tfullDrawIndexUint32: %u", features.fullDrawIndexUint32);
	LOG_F(INFO, "\tgeometryShader: %u", features.geometryShader);
	LOG_F(INFO, "\timageCubeArray: %u", features.imageCubeArray);
	LOG_F(INFO, "\tindependentBlend: %u", features.independentBlend);
	LOG_F(INFO, "\tinheritedQueries: %u", features.inheritedQueries);
	LOG_F(INFO, "\tlargePoints: %u", features.largePoints);
	LOG_F(INFO, "\tlogicOp: %u", features.logicOp);
	LOG_F(INFO, "\tmultiDrawIndirect: %u", features.multiDrawIndirect);	
	LOG_F(INFO, "\tmultiViewport: %u", features.multiViewport);
	LOG_F(INFO, "\tocclusionQueryPrecise: %u", features.occlusionQueryPrecise);
	LOG_F(INFO, "\tpipelineStatisticsQuery: %u", features.pipelineStatisticsQuery);
	LOG_F(INFO, "\trobustBufferAccess: %u", features.robustBufferAccess);
	LOG_F(INFO, "\tsamplerAnisotropy: %u", features.samplerAnisotropy);
	LOG_F(INFO, "\tsampleRateShading: %u", features.sampleRateShading);	
	LOG_F(INFO, "\tshaderClipDistance: %u", features.shaderClipDistance);
	LOG_F(INFO, "\tshaderCullDistance: %u", features.shaderCullDistance);
	LOG_F(INFO, "\tshaderFloat64: %u", features.shaderFloat64);
	LOG_F(INFO, "\tshaderImageGatherExtended: %u", features.shaderImageGatherExtended);
	LOG_F(INFO, "\tshaderInt16: %u", features.shaderInt16);
	LOG_F(INFO, "\tshaderInt64: %u", features.shaderInt64);
	LOG_F(INFO, "\tshaderResourceMinLod: %u", features.shaderResourceMinLod);
	LOG_F(INFO, "\tshaderResourceResidency: %u", features.shaderResourceResidency);
	LOG_F(INFO, "\tshaderSampledImageArrayDynamicIndexing: %u", features.shaderSampledImageArrayDynamicIndexing);
	LOG_F(INFO, "\tshaderStorageBufferArrayDynamicIndexing: %u", features.shaderStorageBufferArrayDynamicIndexing);
	LOG_F(INFO, "\tshaderStorageImageArrayDynamicIndexing: %u", features.shaderStorageImageArrayDynamicIndexing);
	LOG_F(INFO, "\tshaderStorageImageExtendedFormats: %u", features.shaderStorageImageExtendedFormats);
	LOG_F(INFO, "\tshaderStorageImageMultisample: %u", features.shaderStorageImageMultisample);
	LOG_F(INFO, "\tshaderStorageImageReadWithoutFormat: %u", features.shaderStorageImageReadWithoutFormat);
	LOG_F(INFO, "\tshaderStorageImageWriteWithoutFormat: %u", features.shaderStorageImageWriteWithoutFormat);
	LOG_F(INFO, "\tshaderTessellationAndGeometryPointSize: %u", features.shaderTessellationAndGeometryPointSize);
	LOG_F(INFO, "\tshaderUniformBufferArrayDynamicIndexing: %u", features.shaderUniformBufferArrayDynamicIndexing);
	LOG_F(INFO, "\tsparseBinding: %u", features.sparseBinding);
	LOG_F(INFO, "\tsparseResidency16Samples: %u", features.sparseResidency16Samples);
	LOG_F(INFO, "\tsparseResidency2Samples: %u", features.sparseResidency2Samples);
	LOG_F(INFO, "\tsparseResidency4Samples: %u", features.sparseResidency4Samples);
	LOG_F(INFO, "\tsparseResidency8Samples: %u", features.sparseResidency8Samples);
	LOG_F(INFO, "\tsparseResidencyAliased: %u", features.sparseResidencyAliased);
	LOG_F(INFO, "\tsparseResidencyBuffer: %u", features.sparseResidencyBuffer);
	LOG_F(INFO, "\tsparseResidencyImage2D: %u", features.sparseResidencyImage2D);
	LOG_F(INFO, "\tsparseResidencyImage3D: %u", features.sparseResidencyImage3D);
	LOG_F(INFO, "\ttessellationShader: %u", features.tessellationShader);
	LOG_F(INFO, "\ttextureCompressionASTC_LDR: %u", features.textureCompressionASTC_LDR);
	LOG_F(INFO, "\ttextureCompressionBC: %u", features.textureCompressionBC);
	LOG_F(INFO, "\ttextureCompressionETC2: %u", features.textureCompressionETC2);
	LOG_F(INFO, "\tvariableMultisampleRate: %u", features.variableMultisampleRate);
	LOG_F(INFO, "\tvertexPipelineStoresAndAtomics: %u", features.vertexPipelineStoresAndAtomics);
	LOG_F(INFO, "\twideLines: %u", features.wideLines);

	std::vector<vk::ExtensionProperties> deviceExtensions = physicalDevice.enumerateDeviceExtensionProperties();
	LOG_F(INFO, "Enumerating Vulkan device extensions for [%s]", properties.deviceName);
	// Check for VK_KHR_swapchain extension.
	bool swapchainKHRSupport = false;
	for (vk::ExtensionProperties deviceExtension : deviceExtensions) {
		LOG_F(INFO, "\t%s ", deviceExtension.extensionName);
		if (std::strcmp(deviceExtension.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0)
			swapchainKHRSupport = true;
	}

	if (!features.samplerAnisotropy && !swapchainKHRSupport)
		return false;
	return true;
}

vk::UniqueInstance HelperVk::createInstance(SDL_SysWMinfo wmInfo) {
	std::vector<vk::LayerProperties> layerProperties = vk::enumerateInstanceLayerProperties();

	vk::ApplicationInfo appInfo;
	appInfo.apiVersion = VK_API_VERSION_1_0;

	std::vector<const char*> instanceExtensions;
	std::vector<const char*> instanceLayers;

#ifdef _DEBUG
	instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	instanceLayers.push_back("VK_LAYER_LUNARG_standard_validation");
#endif

	instanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

	switch (wmInfo.subsystem) {
#ifdef VK_USE_PLATFORM_MIR_KHR
	case SDL_SYSWM_MIR: instanceExtensions.push_back(VK_KHR_MIR_SURFACE_EXTENSION_NAME); break;
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
	case SDL_SYSWM_WAYLAND: instanceExtensions.push_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME); break;
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
	case SDL_SYSWM_WINDOWS: instanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME); break;
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
	case SDL_SYSWM_X11: instanceExtensions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME); break;
#endif
	}

	vk::InstanceCreateInfo instanceInfo;
	instanceInfo.pApplicationInfo = &appInfo;
	instanceInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
	instanceInfo.ppEnabledExtensionNames = instanceExtensions.data();
	instanceInfo.enabledLayerCount = static_cast<uint32_t>(instanceLayers.size());
	instanceInfo.ppEnabledLayerNames = instanceLayers.data();
	return std::move(vk::createInstanceUnique(instanceInfo));
}

vk::UniqueSurfaceKHR HelperVk::createSurface(vk::Instance instance, SDL_SysWMinfo wmInfo) {
#ifdef VK_USE_PLATFORM_MIR_KHR
	vk::MIRSurfaceCreateInfoKHR surfaceCreateInfo;
	surfaceCreateInfo.connection = wmInfo.info.mir.connection;
	surfaceCreateInfo.surface = wmInfo.info.mir.surface;
	return std::move(instance.createMirSurfaceKHRUnique(surfaceCreateInfo));
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
	vk::WaylandSurfaceCreateInfoKHR surfaceCreateInfo;
	surfaceCreateInfo.display = wmInfo.info.wl.display;
	surfaceCreateInfo.surface = wmInfo.info.wl.surface;
	return std::move(instance.createWaylandSurfaceKHRUnique(surfaceCreateInfo));
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
	vk::Win32SurfaceCreateInfoKHR surfaceCreateInfo;
	surfaceCreateInfo.hinstance = wmInfo.info.win.hinstance;
	surfaceCreateInfo.hwnd = wmInfo.info.win.window;
	return std::move(instance.createWin32SurfaceKHRUnique(surfaceCreateInfo));
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
	vk::XCBSurfaceCreateInfoKHR surfaceCreateInfo;
	surfaceCreateInfo.dpy = wmInfo.info.x11.display;
	surfaceCreateInfo.window = wmInfo.info.x11.window;
	return std::move(instance.createXlibSurfaceKHRUnique(surfaceCreateInfo));
#endif
}

vk::UniqueDevice HelperVk::createDevice(vk::PhysicalDevice physicalDevice, uint32_t queueIndex) {
	float priority = 1.0f;
	vk::DeviceQueueCreateInfo deviceQueueInfo;
	deviceQueueInfo.queueCount = 1;
	deviceQueueInfo.queueFamilyIndex = queueIndex;
	deviceQueueInfo.pQueuePriorities = &priority;
	
	std::vector<const char*> enabledDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	vk::PhysicalDeviceFeatures enabledFeatures = { enabledFeatures.samplerAnisotropy = VK_TRUE };

	// Create device.
	vk::DeviceCreateInfo deviceInfo;
	deviceInfo.pEnabledFeatures = &enabledFeatures;
	deviceInfo.enabledLayerCount = 0;
	deviceInfo.ppEnabledExtensionNames = enabledDeviceExtensions.data();
	deviceInfo.enabledExtensionCount = static_cast<uint32_t>(enabledDeviceExtensions.size());
	deviceInfo.pQueueCreateInfos = &deviceQueueInfo;
	deviceInfo.queueCreateInfoCount = 1;
	return std::move(physicalDevice.createDeviceUnique(deviceInfo));
}

vk::UniqueSwapchainKHR HelperVk::createSwapchain(vk::Device device, vk::SurfaceKHR surface, vk::Extent2D extent,
	uint32_t numImages, vk::Format format, vk::PresentModeKHR presentMode, vk::SwapchainKHR previousSwapchain) {
	// Create the swapchain for the surface.
	vk::SwapchainCreateInfoKHR swapchainInfo;
	swapchainInfo.clipped = VK_TRUE;
	swapchainInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	swapchainInfo.minImageCount = numImages;
	swapchainInfo.imageArrayLayers = 1;
	swapchainInfo.imageExtent = extent;
	swapchainInfo.imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
	swapchainInfo.imageFormat = format;
	swapchainInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst;
	swapchainInfo.presentMode = presentMode;
	swapchainInfo.preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
	swapchainInfo.surface = surface;
	swapchainInfo.oldSwapchain = previousSwapchain;
	return std::move(device.createSwapchainKHRUnique(swapchainInfo));
}

std::vector<vk::UniqueImageView> HelperVk::createImageViews(vk::Device device, vk::SwapchainKHR swapchain, vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags) {
	vk::ImageViewCreateInfo viewInfo;
	viewInfo.viewType = vk::ImageViewType::e2D;
	viewInfo.format = format;
	viewInfo.components.r = vk::ComponentSwizzle::eR;
	viewInfo.components.g = vk::ComponentSwizzle::eG;
	viewInfo.components.b = vk::ComponentSwizzle::eB;
	viewInfo.components.a = vk::ComponentSwizzle::eA;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;
	
	std::vector<vk::UniqueImageView> imageViews;
	std::vector<vk::Image> swapchainImages = device.getSwapchainImagesKHR(swapchain);
	for (uint32_t i = 0; i < swapchainImages.size(); i++) {
		if (image == nullptr)
			viewInfo.image = swapchainImages[i];
		else
			viewInfo.image = image;
		imageViews.push_back(device.createImageViewUnique(viewInfo));
	}
	return imageViews;
}

std::optional<uint32_t> HelperVk::selectQueueFamilyIndex(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface) {
	std::vector<vk::QueueFamilyProperties> queueFamilies = physicalDevice.getQueueFamilyProperties();
	for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilies.size()); i++) {
		// Must be both a graphics queue, and support presenting to a surface.
		if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics && physicalDevice.getSurfaceSupportKHR(i, surface))
			return i;
	}
	return {};
}

vk::Format HelperVk::selectColorFormat(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface) {
	// Grab all available surface formats;
	auto surfaceFormats = physicalDevice.getSurfaceFormatsKHR(surface);

	// By default, select the first one in the list.
	vk::Format format = surfaceFormats.front().format;
	
	// Check to see if the driver lets us select which one we want.
	if (surfaceFormats.size() == 1 && surfaceFormats[0].format == vk::Format::eUndefined)
		format = vk::Format::eR8G8B8A8Unorm;
	else {
		// Iterate through each format and check to see if it has the format we want.
		for (vk::SurfaceFormatKHR surfaceFormat : surfaceFormats)
			if (surfaceFormat.format == vk::Format::eR8G8B8A8Unorm)
				format = surfaceFormat.format;
	}
	return format;
}

std::optional<vk::Format> HelperVk::selectDepthStencilFormat(vk::PhysicalDevice physicalDevice) {
	// Since all depth formats may be optional, we need to find a suitable depth format to use
	// Start with the highest precision packed format
	std::vector<vk::Format> depthFormats = {
		vk::Format::eD32SfloatS8Uint,
		vk::Format::eD32Sfloat,
		vk::Format::eD24UnormS8Uint,
		vk::Format::eD16UnormS8Uint,
		vk::Format::eD16Unorm
	};

	for (vk::Format format : depthFormats) {
		vk::FormatProperties properties = physicalDevice.getFormatProperties(format);
		// Format must support depth stencil attachment for optimal tiling
		if (properties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
			return format;
	}
	return {};
}

vk::PresentModeKHR HelperVk::selectPresentMode(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, bool vsync, bool tripleBuffering, bool tearing) {
	// Grab supported surface capabilities.
	auto supportedPresentModes = physicalDevice.getSurfacePresentModesKHR(surface);
	for (vk::PresentModeKHR supportedPresentMode : supportedPresentModes) {
		// Immediate will not wait for blanking periods to present.
		if (supportedPresentMode == vk::PresentModeKHR::eImmediate &&
			vsync && tripleBuffering)
			return supportedPresentMode;
		// Fifo waits for blanking periods.
		if (supportedPresentMode == vk::PresentModeKHR::eFifo && 
			vsync & !tripleBuffering & !tearing)
			return supportedPresentMode;
		// FifoRelaxed will wait for the blanking period, but if its exceeded it will continue. (tearing)
		if (supportedPresentMode == vk::PresentModeKHR::eFifoRelaxed &&
			tearing && vsync)
			return supportedPresentMode;
		// Mailbox uses an image queue to replace any outstanding images.
		if (supportedPresentMode == vk::PresentModeKHR::eMailbox &&
			tripleBuffering && !vsync && !tearing)
			return supportedPresentMode;
	}

	// Return fifo as fallback since its the only spec supported guarantee.
	return vk::PresentModeKHR::eFifo;
}

vk::SampleCountFlagBits HelperVk::getMaxUsableSampleCount(vk::PhysicalDevice physicalDevice) {
	vk::PhysicalDeviceProperties properties = physicalDevice.getProperties();
	
	// std::min has no idea how to calculate the vulkan-hpp enum variant. Convert it to the original.
	VkSampleCountFlags counts = std::min(static_cast<VkSampleCountFlags>(properties.limits.framebufferColorSampleCounts), 
		static_cast<VkSampleCountFlags>(properties.limits.framebufferDepthSampleCounts));
	if (counts & VK_SAMPLE_COUNT_64_BIT) return vk::SampleCountFlagBits::e64;
	else if (counts & VK_SAMPLE_COUNT_32_BIT) return vk::SampleCountFlagBits::e32;
	else if (counts & VK_SAMPLE_COUNT_16_BIT) return vk::SampleCountFlagBits::e16;
	else if (counts & VK_SAMPLE_COUNT_8_BIT) return vk::SampleCountFlagBits::e8;
	else if (counts & VK_SAMPLE_COUNT_4_BIT) return vk::SampleCountFlagBits::e4;
	else if (counts & VK_SAMPLE_COUNT_2_BIT) return vk::SampleCountFlagBits::e2;
	else return vk::SampleCountFlagBits::e1;
}

std::optional<uint32_t> HelperVk::getMemoryTypeIndex(vk::PhysicalDevice physicalDevice, uint32_t typeBits, vk::MemoryPropertyFlags properties) {
	vk::PhysicalDeviceMemoryProperties memororyProperties = physicalDevice.getMemoryProperties();
	for (uint32_t i = 0; i < memororyProperties.memoryTypeCount; i++) {
		if ((typeBits & 1) == 1) {
			if ((memororyProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}
		typeBits >>= 1;
	}
	return {};
}

vk::UniqueShaderModule HelperVk::createShaderModule(vk::Device device, const char* pFilePath) {
	std::ifstream shaderStream(pFilePath, std::ios::binary);
	std::string shaderContent((std::istreambuf_iterator<char>(shaderStream)),
		std::istreambuf_iterator<char>());
	vk::ShaderModuleCreateInfo moduleInfo;
	moduleInfo.codeSize = shaderContent.size();
	moduleInfo.pCode = reinterpret_cast<const uint32_t*>(shaderContent.data());
	return std::move(device.createShaderModuleUnique(moduleInfo));
}
