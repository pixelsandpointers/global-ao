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

  private:
    static auto findSuitableMemoryTypeIndex(
        const Device& device,
        uint32_t typeFilter,
        vk::MemoryPropertyFlags requestedProperties) -> uint32_t {
        const auto& physicalDevice = device.getPhysicalDevice();
        const auto memoryProperties = physicalDevice.getMemoryProperties();

        for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
            if (((typeFilter & (1 << i)) != 0U)
                && (memoryProperties.memoryTypes[i].propertyFlags & requestedProperties) == requestedProperties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }
};

}  // namespace global_ao