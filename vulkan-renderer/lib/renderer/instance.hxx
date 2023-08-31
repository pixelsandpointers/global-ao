#pragma once

#include <optional>
#include <span>
#include <vulkan/vulkan_raii.hpp>

namespace global_ao {

// Vulkan instance handler.
class Instance {
  public:
    Instance();
    Instance(const std::span<const char*>& externallyRequiredExtensions);

    ~Instance() = default;

    Instance(const Instance& other) = delete;
    auto operator=(const Instance& other) -> Instance& = delete;

    Instance(Instance&&) = delete;
    auto operator=(Instance&&) -> Instance& = delete;

    auto getVulkanInternalInstance() const -> const vk::raii::Instance& {
        return instance;
    }

  private:
    auto createInstanceCreateInfo(const std::span<const char*>& externallyRequiredExtensions) -> vk::InstanceCreateInfo;
    auto createInstance() const -> vk::raii::Instance;
    auto setupDebugMessenger() -> std::optional<vk::raii::DebugUtilsMessengerEXT>;
    auto setupVulkanExtensions(const std::span<const char*>& externallyRequiredExtensions) -> void;
    auto setupVulkanLayers() -> void;
    static auto queryAvailableVulkanExtensions() -> const std::vector<vk::ExtensionProperties>&;
    auto checkRequiredExtensions() const -> void;
    static auto queryAvailableVulkanLayers() -> const std::vector<vk::LayerProperties>&;
    auto checkRequiredVulkanLayers() const -> void;
    static auto logAvailableVulkanExtensions() -> void;
    static auto logAvailableVulkanLayers() -> void;
    static auto createDebugUtilsMessengerCreateInfoEXT() -> vk::DebugUtilsMessengerCreateInfoEXT;

#ifdef global_ao_DEBUG
    inline static constexpr bool enableValidationLayers = true;
#else
    inline static constexpr bool enableValidationLayers = false;
#endif
    std::string applicationName { "Global AO" };
    vk::raii::Context context;
    vk::ApplicationInfo applicationInfo;
    std::vector<const char*> requiredExtensions;
    std::vector<const char*> requiredLayers;
    vk::InstanceCreateInfo instanceCreateInfo;
    vk::raii::Instance instance;
    std::optional<vk::raii::DebugUtilsMessengerEXT> debugMessenger;  // will not be initialized if
                                                                     // enableValidationLayers is false
    inline static bool extensionsAlreadyQueried = false;
    inline static std::vector<vk::ExtensionProperties> availableExtensions;
    inline static bool layersAlreadyQueried = false;
    inline static std::vector<vk::LayerProperties> availableLayers;
};

}  // namespace global_ao