#include <lib/renderer/buffers/buffer.hxx>
#include <lib/renderer/device.hxx>
#include <lib/renderer/utilities.hxx>

namespace global_ao {
Buffer::Buffer(
    const Device& device,
    vk::DeviceSize size,
    const vk::BufferUsageFlags usage,
    const vk::MemoryPropertyFlags properties)
  : device { device },
    bufferSizeBytes { size },
    buffer { createBuffer(usage) },
    deviceMemory { Utilities::createDeviceMemory(buffer, device, properties) } {
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

auto Buffer::getDeviceMemory() const -> const vk::raii::DeviceMemory& {
    return deviceMemory;
}

}  // namespace global_ao