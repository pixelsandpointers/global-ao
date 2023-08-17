#pragma once
#include <lib/renderer/device.hxx>
#include <vector>
#include <vulkan/vulkan_raii.hpp>

namespace global_ao {

class Buffer {
  public:
    Buffer(const Device& device, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);

    auto getSize() const -> vk::DeviceSize;
    auto getBuffer() const -> const vk::raii::Buffer&;
    auto getDeviceMemory() const -> const vk::raii::DeviceMemory&;

  private:
    auto createBuffer(vk::BufferUsageFlags usage) -> vk::raii::Buffer;
    const Device& device;
    vk::DeviceSize bufferSizeBytes;
    vk::raii::Buffer buffer;
    vk::raii::DeviceMemory deviceMemory;
};

}  // namespace global_ao
