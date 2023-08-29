#pragma once

#include <lib/renderer/device.hxx>
#include <vulkan/vulkan_raii.hpp>

namespace global_ao {

class Utilities {
  public:
    template<typename T>
    static auto createDeviceMemory(
        const T& vulkanMemoryObject,
        const Device& device,
        vk::MemoryPropertyFlags properties) -> vk::raii::DeviceMemory {
        const auto& _device = device.getLogicalDevice();

        const auto memoryRequirements = vulkanMemoryObject.getMemoryRequirements();
        const auto memoryAllocateInfo = vk::MemoryAllocateInfo {
            .allocationSize = memoryRequirements.size,
            .memoryTypeIndex = findSuitableMemoryTypeIndex(device, memoryRequirements.memoryTypeBits, properties)
        };
        auto deviceMemory = vk::raii::DeviceMemory(_device, memoryAllocateInfo);

        // bind the complete device memory to the buffer
        vulkanMemoryObject.bindMemory(*deviceMemory, 0);

        return deviceMemory;
    }

    static auto findSupportedImageFormat(
        const Device& device,
        const std::vector<vk::Format>& candidates,
        vk::ImageTiling tiling,
        vk::FormatFeatureFlags features) -> vk::Format;

  private:
    static auto findSuitableMemoryTypeIndex(
        const Device& device,
        uint32_t typeFilter,
        vk::MemoryPropertyFlags requestedProperties) -> uint32_t;
};

}  // namespace global_ao