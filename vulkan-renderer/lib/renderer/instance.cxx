#include <iostream>
#include <lib/renderer/instance.hxx>
#include <vulkan/vulkan_raii.hpp>

namespace global_ao {

static VKAPI_ATTR auto VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT /*messageSeverity*/,
    VkDebugUtilsMessageTypeFlagsEXT /*messageType*/,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* /*pUserData*/) -> VkBool32 {
    std::cout << "Vulkan Validation Layer: " << pCallbackData->pMessage << "\n";
    return VkBool32 { VK_FALSE };
}

Instance::Instance(const std::span<const char*>& externallyRequiredExtensions)
  : applicationInfo { .pApplicationName = applicationName.c_str(),
                      .applicationVersion = 1,
                      .pEngineName = applicationName.c_str(),
                      .engineVersion = 1,
                      .apiVersion = VK_API_VERSION_1_3 },
    instanceCreateInfo { createInstanceCreateInfo(externallyRequiredExtensions) },
    instance { createInstance() },
    debugMessenger { setupDebugMessenger() } {};

Instance::Instance()
  : Instance { std::span<const char*> {} } {
}

auto Instance::createInstanceCreateInfo(const std::span<const char*>& externallyRequiredExtensions)
    -> vk::InstanceCreateInfo {
    setupVulkanExtensions(externallyRequiredExtensions);
    setupVulkanLayers();

    auto createInfo = vk::InstanceCreateInfo {
        .pApplicationInfo = &applicationInfo,
        .enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
        .ppEnabledLayerNames = requiredLayers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
        .ppEnabledExtensionNames = requiredExtensions.data(),
    };

    return createInfo;
}

auto Instance::createInstance() const -> vk::raii::Instance {
    queryAvailableVulkanExtensions();
    checkRequiredExtensions();

    queryAvailableVulkanLayers();
    checkRequiredVulkanLayers();

    if constexpr (enableValidationLayers) {
        auto debugUtilsMessengerCreateInfoEXT = createDebugUtilsMessengerCreateInfoEXT();
        auto chain = vk::StructureChain<vk::InstanceCreateInfo, vk::DebugUtilsMessengerCreateInfoEXT> {
            instanceCreateInfo,
            debugUtilsMessengerCreateInfoEXT,
        };
        return vk::raii::Instance { context, chain.get<vk::InstanceCreateInfo>() };
    } else {
        return vk::raii::Instance { context, instanceCreateInfo };
    }
}

auto Instance::setupDebugMessenger() -> std::optional<vk::raii::DebugUtilsMessengerEXT> {
    if constexpr (enableValidationLayers) {
        auto debugUtilsMessengerCreateInfoEXT = createDebugUtilsMessengerCreateInfoEXT();
        return vk::raii::DebugUtilsMessengerEXT { instance, debugUtilsMessengerCreateInfoEXT };
    } else {
        return std::nullopt;
    }
}

auto Instance::setupVulkanExtensions(const std::span<const char*>& externallyRequiredExtensions) -> void {
    if constexpr (enableValidationLayers) {
        requiredExtensions.push_back("VK_EXT_debug_utils");  // for validation layer logging
    }

    for (const auto* const externallyRequiredExtension : externallyRequiredExtensions) {
        requiredExtensions.push_back(externallyRequiredExtension);
    }
}

auto Instance::setupVulkanLayers() -> void {
    if constexpr (enableValidationLayers) {
        requiredLayers.push_back("VK_LAYER_KHRONOS_validation");
    }
}

auto Instance::queryAvailableVulkanExtensions() -> const std::vector<vk::ExtensionProperties>& {
    if (!extensionsAlreadyQueried) {
        availableExtensions = vk::enumerateInstanceExtensionProperties();
        extensionsAlreadyQueried = true;
        logAvailableVulkanExtensions();
    }
    return availableExtensions;
}

auto Instance::queryAvailableVulkanLayers() -> const std::vector<vk::LayerProperties>& {
    if (!layersAlreadyQueried) {
        availableLayers = vk::enumerateInstanceLayerProperties();
        layersAlreadyQueried = true;
        logAvailableVulkanLayers();
    }
    return availableLayers;
}

auto Instance::checkRequiredExtensions() const -> void {
    auto hasRequiredExtensions = true;
    for (const auto* const requiredExtension : requiredExtensions) {
        auto checkExtensionAvailable = [&](auto& availableExtension) {
            return std::strcmp(availableExtension.extensionName, requiredExtension) == 0;
        };

        auto hasRequiredExtension =
            std::find_if(availableExtensions.begin(), availableExtensions.end(), checkExtensionAvailable)
            != availableExtensions.end();

        if (!hasRequiredExtension) {
            hasRequiredExtensions = false;
            break;
        }
    }

    if (!hasRequiredExtensions) {
        throw std::runtime_error("Required Vulkan extensions are not available.");
    }
}

auto Instance::checkRequiredVulkanLayers() const -> void {
    auto hasRequiredLayers = true;
    for (const auto* const requiredLayer : requiredLayers) {
        auto checkLayerAvailable = [&](auto& availableLayer) {
            return std::strcmp(availableLayer.layerName, requiredLayer) == 0;
        };

        auto hasRequiredLayer =
            std::find_if(availableLayers.begin(), availableLayers.end(), checkLayerAvailable) != availableLayers.end();

        if (!hasRequiredLayer) {
            hasRequiredLayers = false;
            break;
        }
    }

    if (!hasRequiredLayers) {
        throw std::runtime_error("Required Vulkan layers are not available.");
    }
}

auto Instance::logAvailableVulkanExtensions() -> void {
    std::cout << "Available Vulkan extensions:"
              << "\n";
    for (auto availableExtension : availableExtensions) {
        std::cout << "Available Vulkan extension: " << availableExtension.extensionName << "\n";
    }
}

auto Instance::logAvailableVulkanLayers() -> void {
    std::cout << "Available Vulkan layers:"
              << "\n";
    for (auto availableLayer : availableLayers) {
        std::cout << "Available Vulkan layer: " << availableLayer.layerName << "\n";
    }
}

auto Instance::createDebugUtilsMessengerCreateInfoEXT() -> vk::DebugUtilsMessengerCreateInfoEXT {
    // all severities will be logged
    vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
        | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);

    // all message types will be logged
    vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
        | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);

    return vk::DebugUtilsMessengerCreateInfoEXT { .messageSeverity = severityFlags,
                                                  .messageType = messageTypeFlags,
                                                  .pfnUserCallback = &debugCallback };
}

}  // namespace global_ao