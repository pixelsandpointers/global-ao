#include <lib/renderer/buffers/index-buffer.hxx>

namespace global_ao {
IndexBuffer::IndexBuffer(const Device& device, vk::DeviceSize size, size_t indexCount)
  : indexCount { indexCount },
    bufferSizeBytes { size },
    buffer { device,
             size,
             vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
             vk::MemoryPropertyFlagBits::eDeviceLocal } {
}

auto IndexBuffer::getIndexCount() const -> vk::DeviceSize {
    return indexCount;
}

auto IndexBuffer::getBuffer() const -> const Buffer& {
    return buffer;
}
}  // namespace global_ao