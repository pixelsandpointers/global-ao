#pragma once
#include <lib/renderer/buffers/buffer.hxx>
#include <lib/renderer/device.hxx>

namespace global_ao {

class UniformBuffer {
  public:
    explicit UniformBuffer(const Device& device);

    auto loadUniformBuffer(const UniformBufferObject& uniformBufferObject) -> void;

    auto getSize() const -> vk::DeviceSize;
    auto getBuffer() const -> const Buffer&;

  private:
    auto calculateSize() const -> vk::DeviceSize;

    vk::DeviceSize bufferSizeBytes;
    Buffer buffer;
};

}  // namespace global_ao