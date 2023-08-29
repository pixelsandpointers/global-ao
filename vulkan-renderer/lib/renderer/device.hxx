#pragma once
#include <lib/renderer/instance.hxx>
#include <vulkan/vulkan_raii.hpp>

namespace global_ao {

class Device {
  public:
    explicit Device(const Instance& instance);
    ~Device() = default;

    auto getPhysicalDevice() const -> const vk::raii::PhysicalDevice&;
    auto getLogicalDevice() const -> const vk::raii::Device&;
    auto getGraphicsQueueFamilyIndex() const -> uint32_t;
    auto getPresentQueueFamilyIndex() const -> uint32_t;
    auto getGraphicsQueueHandle() const -> const vk::raii::Queue&;
    auto getPresentQueueHandle() const -> const vk::raii::Queue&;

  private:
    auto setupPhysicalDevice() -> vk::raii::PhysicalDevice;
    auto findGraphicsQueueFamilyIndices() -> uint32_t;
    auto findPresentQueueFamilyIndices() -> uint32_t;
    auto setupLogicalDevice() -> vk::raii::Device;
    auto setupGraphicsQueueHandle() -> vk::raii::Queue;
    auto setupPresentQueueHandle() -> vk::raii::Queue;
    static auto logPhysicalDevices(const vk::raii::PhysicalDevices& physicalDevices) -> void;
    auto selectPhysicalDevice(const vk::raii::PhysicalDevices& _physicalDevice) -> const vk::raii::PhysicalDevice&;


    const Instance& instance;
    const std::vector<const char*> requiredDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    const vk::PhysicalDeviceFeatures requiredDeviceFeatures = { .samplerAnisotropy = VK_TRUE };
    vk::raii::PhysicalDevice physicalDevice;
    uint32_t graphicsQueueFamilyIndex;
    uint32_t presentQueueFamilyIndex;
    vk::raii::Device logicalDevice;
    vk::raii::Queue graphicsQueueHandle;
    vk::raii::Queue presentQueueHandle;
};

}  // namespace global_ao