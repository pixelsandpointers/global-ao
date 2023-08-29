#include <iostream>
#include <lib/renderer/device.hxx>
#include <lib/renderer/instance.hxx>
#include <set>

namespace global_ao {
Device::Device(const Instance& instance)
  : instance { instance },
    physicalDevice { setupPhysicalDevice() },
    graphicsQueueFamilyIndex { findGraphicsQueueFamilyIndices() },
    presentQueueFamilyIndex { findPresentQueueFamilyIndices() },
    logicalDevice { setupLogicalDevice() },
    graphicsQueueHandle { setupGraphicsQueueHandle() },
    presentQueueHandle { setupPresentQueueHandle() } {
}

auto Device::setupPhysicalDevice() -> vk::raii::PhysicalDevice {
    auto physicalDevices = vk::raii::PhysicalDevices { instance.getVulkanInternalInstance() };
    logPhysicalDevices(physicalDevices);
    auto selectedPhysicalDevice = selectPhysicalDevice(physicalDevices);
    std::cout << "Selected Physical Device: " << selectedPhysicalDevice.getProperties().deviceName << "\n";
    return selectedPhysicalDevice;
}

auto Device::setupLogicalDevice() -> vk::raii::Device {
    // queue family indices must be unique, see:
    // https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkDeviceCreateInfo.html#VUID-VkDeviceCreateInfo-queueFamilyIndex-02802
    auto uniqueQueueFamilyIndices = std::set<uint32_t> { graphicsQueueFamilyIndex, presentQueueFamilyIndex };

    // setup queue create infos
    float queuePriority = 1.0F;  // we only have one queue, so this doesn't matter
    auto queueCreateInfos = std::vector<vk::DeviceQueueCreateInfo> {};
    for (auto queueFamilyIndex : uniqueQueueFamilyIndices) {
        queueCreateInfos.push_back(
            { .queueFamilyIndex = queueFamilyIndex, .queueCount = 1, .pQueuePriorities = &queuePriority });
    }


    // setup device create info
    auto deviceCreateInfo =
        vk::DeviceCreateInfo { .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
                               .pQueueCreateInfos = queueCreateInfos.data(),
                               .enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions.size()),
                               .ppEnabledExtensionNames = requiredDeviceExtensions.data(),
                               .pEnabledFeatures = &requiredDeviceFeatures };

    return { physicalDevice, deviceCreateInfo };
}

auto Device::findGraphicsQueueFamilyIndices() -> uint32_t {
    const auto queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
    auto graphicsQueueFamilyProperty = std::find_if(
        queueFamilyProperties.begin(),
        queueFamilyProperties.end(),
        [](const vk::QueueFamilyProperties& queueFamilyProperty) {
            return queueFamilyProperty.queueFlags & vk::QueueFlagBits::eGraphics;
        });
    assert(graphicsQueueFamilyProperty != queueFamilyProperties.end());

    auto graphicsQueueIndex =
        static_cast<uint32_t>(std::distance(queueFamilyProperties.begin(), graphicsQueueFamilyProperty));
    return graphicsQueueIndex;
}

auto Device::findPresentQueueFamilyIndices() -> uint32_t {
    const auto queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
    auto presentQueueFamilyProperty = std::find_if(
        queueFamilyProperties.begin(),
        queueFamilyProperties.end(),
        [](const vk::QueueFamilyProperties& queueFamilyProperty) {
            return queueFamilyProperty.queueFlags & vk::QueueFlagBits::eGraphics;
        });
    assert(presentQueueFamilyProperty != queueFamilyProperties.end());

    auto presentQueueIndex =
        static_cast<uint32_t>(std::distance(queueFamilyProperties.begin(), presentQueueFamilyProperty));
    return presentQueueIndex;
}

auto Device::setupGraphicsQueueHandle() -> vk::raii::Queue {
    return { logicalDevice, graphicsQueueFamilyIndex, 0 };
}

auto Device::setupPresentQueueHandle() -> vk::raii::Queue {
    return { logicalDevice, presentQueueFamilyIndex, 0 };
}

auto Device::logPhysicalDevices(const vk::raii::PhysicalDevices& physicalDevices) -> void {
    for (const auto& _physicalDevice : physicalDevices) {
        std::cout << "Vulkan found Physical Device: " << _physicalDevice.getProperties().deviceName << "\n";
    }
}

auto Device::selectPhysicalDevice(const vk::raii::PhysicalDevices& physicalDevices) -> const vk::raii::PhysicalDevice& {
    // Check for swapchain support
    const auto& _requiredDeviceExtensions = requiredDeviceExtensions;

    auto supportsRequiredExtensions = [&_requiredDeviceExtensions](const vk::raii::PhysicalDevice& _physicalDevice) {
        auto hasRequiredExtensions = true;
        auto availableExtensions = _physicalDevice.enumerateDeviceExtensionProperties();
        for (const auto& requiredExtension : _requiredDeviceExtensions) {
            auto extensionFound = std::find_if(
                availableExtensions.begin(),
                availableExtensions.end(),
                [&requiredExtension](const vk::ExtensionProperties& availableExtension) {
                    return strcmp(availableExtension.extensionName, requiredExtension) == 0;
                });
            if (extensionFound == availableExtensions.end()) {
                // not found
                hasRequiredExtensions = false;
                break;  // no need to continue
            }
        }
        return hasRequiredExtensions;
    };

    auto supportsRequiredFeatures = [](const vk::raii::PhysicalDevice& _physicalDevice,
                                       const vk::PhysicalDeviceFeatures& requiredDeviceFeatures) {
        // TODO: this is a bit hacky, but it works for now
        //       ideally, we would have a list to check against
        auto _physicalDeviceFeatures = _physicalDevice.getFeatures();
        return _physicalDeviceFeatures.samplerAnisotropy == requiredDeviceFeatures.samplerAnisotropy;
    };

    for (const auto& _physicalDevice : physicalDevices) {
        // find a discrete GPU with swapchain support and sampler anisotropy
        if (_physicalDevice.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu
            && supportsRequiredExtensions(_physicalDevice)
            && supportsRequiredFeatures(_physicalDevice, requiredDeviceFeatures)) {
            return _physicalDevice;
        }
    }

    // in case we didn't find a suitable GPU, raise an error
    throw std::runtime_error("No suitable GPU found!");
}

auto Device::getPhysicalDevice() const -> const vk::raii::PhysicalDevice& {
    return physicalDevice;
}

auto Device::getLogicalDevice() const -> const vk::raii::Device& {
    return logicalDevice;
}

auto Device::getGraphicsQueueFamilyIndex() const -> uint32_t {
    return graphicsQueueFamilyIndex;
}

auto Device::getPresentQueueFamilyIndex() const -> uint32_t {
    return presentQueueFamilyIndex;
}

auto Device::getGraphicsQueueHandle() const -> const vk::raii::Queue& {
    return graphicsQueueHandle;
}

auto Device::getPresentQueueHandle() const -> const vk::raii::Queue& {
    return presentQueueHandle;
}


}  // namespace global_ao