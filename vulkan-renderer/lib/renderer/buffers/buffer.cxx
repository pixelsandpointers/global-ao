#include <lib/renderer/buffers/buffer.hxx>

namespace global_ao {
Buffer::Buffer(
    const Device& device,
    vk::DeviceSize size,
    const vk::BufferUsageFlags usage,
    const vk::MemoryPropertyFlags properties)
  : device { device },
    bufferSizeBytes { size },
    buffer { createBuffer(usage) },
    deviceMemory { createDeviceMemory(properties) } {
}

auto Buffer::getSize() const -> vk::DeviceSize {
    return bufferSizeBytes;
}

auto Buffer::getBuffer() const -> const vk::raii::Buffer& {
    return buffer;
}

auto Buffer::createBuffer(const vk::BufferUsageFlags usage) -> vk::raii::Buffer {
    const auto bufferCreateInfo =
        vk::BufferCreateInfo { .size = bufferSizeBytes, .usage = usage, .sharingMode = vk::SharingMode::eExclusive };

    const auto& _device = device.getLogicalDevice();
    return _device.createBuffer(bufferCreateInfo);
}

auto Buffer::createDeviceMemory(const vk::MemoryPropertyFlags properties) -> vk::raii::DeviceMemory {
    const auto& _device = device.getLogicalDevice();

    const auto memoryRequirements = buffer.getMemoryRequirements();
    const auto memoryAllocateInfo =
        vk::MemoryAllocateInfo { .allocationSize = memoryRequirements.size,
                                 .memoryTypeIndex =
                                     findSuitableMemoryTypeIndex(memoryRequirements.memoryTypeBits, properties) };
    auto deviceMemory = _device.allocateMemory(memoryAllocateInfo);

    // bind the complete device memory to the buffer
    buffer.bindMemory(*deviceMemory, 0);

    return deviceMemory;
}

auto Buffer::findSuitableMemoryTypeIndex(uint32_t typeFilter, vk::MemoryPropertyFlags requestedProperties) const
    -> uint32_t {
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

auto Buffer::getDeviceMemory() const -> const vk::raii::DeviceMemory& {
    return deviceMemory;
}

}  // namespace global_ao