#pragma once
#include <lib/renderer/buffer-objects/vertex-object.hxx>
#include <lib/renderer/buffers/buffer.hxx>

namespace global_ao {
class VertexBuffer {
  public:
    VertexBuffer(const Device& device, vk::DeviceSize size, size_t vertexCount);

    auto getVertexCount() const -> vk::DeviceSize;
    auto getBuffer() const -> const Buffer&;

  private:
    const size_t vertexCount;
    vk::DeviceSize bufferSizeBytes;
    Buffer buffer;
};
}  // namespace global_ao