#pragma once
#include <lib/renderer/buffers/buffer.hxx>
#include <lib/renderer/device.hxx>
#include <vulkan/vulkan.hpp>

namespace global_ao {

class IndexBuffer {
  public:
    IndexBuffer(const Device& device, vk::DeviceSize size, size_t indexCount);

    auto getIndexCount() const -> vk::DeviceSize;
    auto getBuffer() const -> const Buffer&;

  private:
    const size_t indexCount;
    vk::DeviceSize bufferSizeBytes;
    Buffer buffer;
};

}  // namespace global_ao
