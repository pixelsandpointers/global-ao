#include <lib/renderer/buffers/vertex-buffer.hxx>

namespace global_ao {
VertexBuffer::VertexBuffer(const Device& device, const vk::DeviceSize size, size_t vertexCount)
  : vertexCount { vertexCount },
    bufferSizeBytes { size },
    buffer { device,
             size,
             vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
             vk::MemoryPropertyFlagBits::eDeviceLocal } {
}

auto VertexBuffer::getVertexCount() const -> vk::DeviceSize {
    return vertexCount;
}

auto VertexBuffer::getBuffer() const -> const Buffer& {
    return buffer;
}
}  // namespace global_ao