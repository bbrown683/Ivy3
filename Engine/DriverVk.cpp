/*
MIT License

Copyright (c) 2018 Ben Brown

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "DriverVk.hpp"

#include <iostream>
#include <SDL2/SDL_syswm.h>

#include "thirdparty/loguru.hpp"
#include "RenderableVk.hpp"
#include "System.hpp"

DriverVk::DriverVk(const SDL_Window* pWindow) : Driver(pWindow) {}

bool DriverVk::initialize() {
    std::vector<vk::ExtensionProperties> extensionProperties;
    auto extensionPropertiesResult = vk::enumerateInstanceExtensionProperties();
    if (extensionPropertiesResult.result == vk::Result::eSuccess)
        extensionProperties = extensionPropertiesResult.value;

	bool displayKHRSupport = false, surfaceKHRSupport = false, wmKHRSupport = false;
    for (vk::ExtensionProperties extension : extensionProperties) {
		if (strcmp(extension.extensionName, VK_KHR_DISPLAY_EXTENSION_NAME) == 0)
			displayKHRSupport = true;
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
	/*
	if (!displayKHRSupport) {
		LOG_F(FATAL, "Vulkan driver does not support rendering to a display.");
		return false;
	} else*/ if (!surfaceKHRSupport || !wmKHRSupport) {
        LOG_F(FATAL, "Vulkan driver does not support rendering to a surface!");
        return false;
    }

    std::vector<vk::LayerProperties> layerProperties;
    auto layerPropertiesResult = vk::enumerateInstanceLayerProperties();
    if (layerPropertiesResult.result == vk::Result::eSuccess)
        layerProperties = layerPropertiesResult.value;

    vk::ApplicationInfo appInfo;
    appInfo.apiVersion = VK_API_VERSION_1_0;

    std::vector<const char*> instanceExtensions;
    std::vector<const char*> instanceLayers;

#ifdef _DEBUG
    instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    instanceLayers.push_back("VK_LAYER_LUNARG_standard_validation");
#endif

	//instanceExtensions.push_back(VK_KHR_DISPLAY_EXTENSION_NAME);
    instanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    if (!SDL_GetWindowWMInfo(const_cast<SDL_Window*>(getWindow()), &wmInfo))
        return false;

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
    default: LOG_F(FATAL, "Could not detect a supported Window Manager!"); return false;
    }

    vk::InstanceCreateInfo instanceInfo;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
    instanceInfo.ppEnabledExtensionNames = instanceExtensions.data();
    instanceInfo.enabledLayerCount = static_cast<uint32_t>(instanceLayers.size());
    instanceInfo.ppEnabledLayerNames = instanceLayers.data();

    auto instanceResult = vk::createInstanceUnique(instanceInfo);
    if (instanceResult.result != vk::Result::eSuccess) {
		LOG_F(FATAL, "Vulkan instance could not be created!");
        return false;
    }
    m_pInstance.swap(instanceResult.value);

#ifdef VK_USE_PLATFORM_MIR_KHR
    vk::MIRSurfaceCreateInfoKHR surfaceCreateInfo;
    surfaceCreateInfo.connection = wmInfo.info.mir.connection;
    surfaceCreateInfo.surface = wmInfo.info.mir.surface;
    auto SurfaceResult = m_pInstance->createMirSurfaceKHRUnique(surfaceCreateInfo);
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
    vk::WaylandSurfaceCreateInfoKHR surfaceCreateInfo;
    surfaceCreateInfo.display = wmInfo.info.wl.display;
    surfaceCreateInfo.surface = wmInfo.info.wl.surface;
    auto surfaceResult = m_pInstance->createWaylandSurfaceKHRUnique(surfaceCreateInfo);
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
    vk::Win32SurfaceCreateInfoKHR surfaceCreateInfo;
    surfaceCreateInfo.hinstance = wmInfo.info.win.hinstance;
    surfaceCreateInfo.hwnd = wmInfo.info.win.window;
    auto surfaceResult = m_pInstance->createWin32SurfaceKHRUnique(surfaceCreateInfo);
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
    vk::XCBSurfaceCreateInfoKHR surfaceCreateInfo;
    surfaceCreateInfo.dpy = wmInfo.info.x11.display;
    surfaceCreateInfo.window = wmInfo.info.x11.window;
    auto surfaceResult = m_pInstance->createXlibSurfaceKHRUnique(surfaceCreateInfo);
#else
    logger.logMessage("Could not detect a supported Window Manager!");
    return false;
#endif

    if (surfaceResult.result != vk::Result::eSuccess) {
        LOG_F(FATAL, "Could not create a Vulkan rendering surface!");
        return false;
    }
    m_pSurface.swap(surfaceResult.value);

    auto m_PhysicalDevicesResult = m_pInstance->enumeratePhysicalDevices();
    if (m_PhysicalDevicesResult.result != vk::Result::eSuccess) {
        LOG_F(FATAL, "Could not detect a Vulkan supported hardware device!");
        return false;
    }
    m_PhysicalDevices.swap(m_PhysicalDevicesResult.value);

	LOG_F(INFO, "Enumerating Physical Devices:");
    uint32_t counter = 0;
    for (vk::PhysicalDevice physicalDevice : m_PhysicalDevices) {
        vk::PhysicalDeviceProperties properties = physicalDevice.getProperties();

        Gpu gpu;
        gpu.id = counter++;
        std::strcpy(gpu.name, properties.deviceName);
		gpu.vendorId = properties.vendorID;
		gpu.deviceId = properties.deviceID;
        gpu.memory = properties.limits.maxMemoryAllocationCount;
        gpu.software = properties.deviceType == vk::PhysicalDeviceType::eVirtualGpu ? true : false;
		
		LOG_F(INFO, "\t[%u]: %s", gpu.id, gpu.name);
		LOG_F(INFO, "\t\tVideoMemory: %u", gpu.memory);
		LOG_F(INFO, "\t\tVendorId: %u", gpu.vendorId);
		LOG_F(INFO, "\t\tDeviceId: %u", gpu.deviceId);
		addGpu(gpu);
    }
    return !m_PhysicalDevices.empty();
}

bool DriverVk::selectGpu(uint32_t id) {
    // id Does not correlate to a proper GPU.
    if (id >= m_PhysicalDevices.size())
        return false;

    std::vector<vk::ExtensionProperties> deviceExtensions;
    auto deviceExtensionsResult = m_PhysicalDevices[id].enumerateDeviceExtensionProperties();
    if (deviceExtensionsResult.result == vk::Result::eSuccess)
        deviceExtensions = deviceExtensionsResult.value;

    // Check for VK_KHR_swapchain extension.
    bool swapchainKHRSupport = false;
	for (vk::ExtensionProperties deviceExtension : deviceExtensions) {
		if (std::strcmp(deviceExtension.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0)
			swapchainKHRSupport = true;
	}

    if (!swapchainKHRSupport) {
        LOG_F(FATAL, "Hardware device does not support presenting to a surface!");
        return false;
    }

    vk::PhysicalDeviceFeatures features = m_PhysicalDevices[id].getFeatures();
    if (features.samplerAnisotropy) {
        anisotropy = true;
        vk::PhysicalDeviceProperties properties = m_PhysicalDevices[id].getProperties();
        maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    }

    std::vector<vk::QueueFamilyProperties> queueFamilies = m_PhysicalDevices[id].getQueueFamilyProperties();
    vk::SurfaceCapabilitiesKHR surfaceCapabilities;
    std::vector<vk::SurfaceFormatKHR> surfaceFormats;
    std::vector<vk::PresentModeKHR> surfacePresentModes;

    std::vector<uint32_t> graphicsSupport;
    std::vector<uint32_t> surfaceSupport;

    for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilies.size()); i++) {
        // Only graphics queues should be checked.
        if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics)
            graphicsSupport.push_back(i);

        // Check for surface support.
        VkBool32 supportsSurface = VK_TRUE;
        auto surfaceSupportResult = m_PhysicalDevices[id].getSurfaceSupportKHR(i, m_pSurface.get());
        if (surfaceSupportResult.result == vk::Result::eSuccess)
            supportsSurface = surfaceSupportResult.value;
        if (supportsSurface) {
            auto surfaceCapabilitiesResult = m_PhysicalDevices[id].getSurfaceCapabilitiesKHR(m_pSurface.get());
            if (surfaceCapabilitiesResult.result == vk::Result::eSuccess)
                surfaceCapabilities = surfaceCapabilitiesResult.value;

            auto surfaceFormatsResult = m_PhysicalDevices[id].getSurfaceFormatsKHR(m_pSurface.get());
            if (surfaceFormatsResult.result == vk::Result::eSuccess)
                surfaceFormats = surfaceFormatsResult.value;

            auto surfacePresentModesResult = m_PhysicalDevices[id].getSurfacePresentModesKHR(m_pSurface.get());
            if (surfacePresentModesResult.result == vk::Result::eSuccess)
                surfacePresentModes = surfacePresentModesResult.value;

            surfaceSupport.push_back(i);
        }
    }

    // Graphics or Surface are not supported.
    if (graphicsSupport.empty() || surfaceSupport.empty()) {
        LOG_F(FATAL, "Hardware device does not support drawing or surface operations!");
        return false;
    } else {
        // Pick first queue family which supports both.
        bool foundQueueIndex = false;
        for (size_t i = 0; i < graphicsSupport.size() || foundQueueIndex != true; i++) {
            for (size_t j = 0; j < surfaceSupport.size(); j++) {
                if (graphicsSupport[i] == surfaceSupport[j]) {
                    queueFamilyIndex = graphicsSupport[i];
                    foundQueueIndex = true;
                    break;
                }
            }
        }
    }

    float priority = 1.0f;
    vk::DeviceQueueCreateInfo deviceQueueInfo;
    deviceQueueInfo.queueCount = 1;
    deviceQueueInfo.queueFamilyIndex = queueFamilyIndex;
    deviceQueueInfo.pQueuePriorities = &priority;

    std::vector<const char*> enabledDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    vk::PhysicalDeviceFeatures enabledFeatures;
    enabledFeatures.samplerAnisotropy = VK_TRUE;

    vk::DeviceCreateInfo deviceInfo;
    deviceInfo.pEnabledFeatures = &enabledFeatures;
    deviceInfo.enabledLayerCount = 0;
    deviceInfo.ppEnabledExtensionNames = enabledDeviceExtensions.data();
    deviceInfo.enabledExtensionCount = static_cast<uint32_t>(enabledDeviceExtensions.size());
    deviceInfo.pQueueCreateInfos = &deviceQueueInfo;
    deviceInfo.queueCreateInfoCount = 1;

    auto deviceResult = m_PhysicalDevices[id].createDeviceUnique(deviceInfo);
    if (deviceResult.result != vk::Result::eSuccess) {
        LOG_F(FATAL, "Failed to create a rendering device!");
        return false;
    }
    if (m_pDevice)
        m_pDevice.reset();
    m_pDevice.swap(deviceResult.value);

    // Select present mode.
    // Prefer to use Mailbox present mode if it exists. (Triple buffering Vsync)
    vk::PresentModeKHR presentMode = vk::PresentModeKHR::eFifo;
    if (surfaceCapabilities.minImageCount > 2)
        for (vk::PresentModeKHR tempPresentMode : surfacePresentModes)
            if (tempPresentMode == vk::PresentModeKHR::eMailbox)
                presentMode = tempPresentMode;

    // Select swapchain format.
    vk::Format format = vk::Format::eUndefined;

    // Check to see if the driver lets us select which one we want.
    if (surfaceFormats.size() == 1 && surfaceFormats[0].format == vk::Format::eUndefined)
        format = vk::Format::eB8G8R8A8Srgb;
    else {
        // Iterate through each format and check to see if it has the format we want.
        for (vk::SurfaceFormatKHR surfaceFormat : surfaceFormats)
            if (surfaceFormat.format == vk::Format::eB8G8R8A8Srgb)
                format = surfaceFormat.format;
        // If we still didnt find a format just pick the first one we get.
        if (format == vk::Format::eUndefined)
            format = surfaceFormats.front().format;
    }

    vk::SwapchainCreateInfoKHR swapchainInfo;
    swapchainInfo.clipped = VK_TRUE;
    swapchainInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    swapchainInfo.minImageCount = surfaceCapabilities.minImageCount;
    swapchainInfo.imageArrayLayers = 1;
    swapchainInfo.imageExtent = surfaceCapabilities.currentExtent;
    swapchainInfo.imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
    swapchainInfo.imageFormat = format;
    swapchainInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst;
    swapchainInfo.presentMode = presentMode;
    swapchainInfo.preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
    swapchainInfo.surface = m_pSurface.get();

    auto swapchainResult = m_pDevice->createSwapchainKHRUnique(swapchainInfo);
    if (swapchainResult.result != vk::Result::eSuccess) {
        LOG_F(FATAL, "Failed to create a swapchain for rendering surface!");
        return false;
    }
    if (m_pSwapchain)
        m_pSwapchain.reset();
    m_pSwapchain.swap(swapchainResult.value);
    return true;
}

bool DriverVk::presentFrame() {
	/*
    vk::FenceCreateInfo fenceInfo;
    auto fenceResult = m_pDevice->createFenceUnique(fenceInfo);
    if (fenceResult.result != vk::Result::eSuccess)
        return false;
    m_pFence.swap(fenceResult.value);

    // Grab a queue related to our device.
    vk::Queue queue = m_pDevice->getQueue(queueFamilyIndex, 0);
    
    // We are only submitting  the primary command list.
    vk::SubmitInfo submitInfo;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_pPrimaryCommandBuffer.get();
    queue.submit(submitInfo, m_pFence.get());

    m_pDevice->waitForFences(m_pFence.get(), true, UINT64_MAX);
    m_pDevice->resetFences(m_pFence.get());

    vk::PresentInfoKHR presentInfo;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &m_pSwapchain.get();
    queue.presentKHR(presentInfo);
	*/
    return true;
}

std::unique_ptr<Renderable> DriverVk::createRenderable() {
    return std::make_unique<RenderableVk>(this);
}

const vk::UniqueDevice& DriverVk::getDevice() const {
    return m_pDevice;
}

const vk::UniqueCommandBuffer& DriverVk::getPrimaryCommandBuffer() const {
    return m_pPrimaryCommandBuffer;
}

const vk::UniqueSwapchainKHR& DriverVk::getSwapchain() const {
    return m_pSwapchain;
}

const vk::UniqueShaderModule& DriverVk::getModuleFromCache(const char* pFilename) const {
    auto iter = m_pModuleCache.find(pFilename);
    return iter->second;
}

vk::ResultValue<vk::UniqueShaderModule> DriverVk::getShaderModuleFromFile(const char* pFilename) {
	const char* pModuleName = std::strcat(const_cast<char*>(pFilename), ".spv");
	auto file = System::readFile(pModuleName);
	vk::ShaderModuleCreateInfo moduleInfo;
	moduleInfo.pCode = reinterpret_cast<const uint32_t*>(file.first);
	moduleInfo.codeSize = file.second;

	return m_pDevice->createShaderModuleUnique(moduleInfo);
}

void DriverVk::createBuffer(vk::DeviceSize size, vk::BufferUsageFlagBits usage, vk::MemoryPropertyFlags properties) {
	vk::BufferCreateInfo bufferInfo;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = vk::SharingMode::eExclusive;
	
}
