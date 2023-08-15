#include <lib/renderer/buffers/staging-buffer.hxx>

namespace global_ao {
StagingBuffer::StagingBuffer(const Device& device, const std::vector<VertexObject>& vertices)
  : bufferSizeBytes { calculateSize(vertices) },
    buffer { device,
             bufferSizeBytes,
             vk::BufferUsageFlagBits::eTransferSrc,
             vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent } {
}

StagingBuffer::StagingBuffer(const Device& device, const std::vector<uint32_t>& indices)
  : bufferSizeBytes { calculateSize(indices) },
    buffer { device,
             bufferSizeBytes,
             vk::BufferUsageFlagBits::eTransferSrc,
             vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent } {
}

auto StagingBuffer::loadVertices(const std::vector<VertexObject>& vertices) -> void {
    const auto& deviceMemory = buffer.getDeviceMemory();

    uint8_t* pData = static_cast<uint8_t*>(deviceMemory.mapMemory(0, bufferSizeBytes));
    memcpy(pData, vertices.data(), bufferSizeBytes);
    deviceMemory.unmapMemory();
}

auto StagingBuffer::loadIndices(const std::vector<uint32_t>& indices) -> void {
    const auto& deviceMemory = buffer.getDeviceMemory();

    uint8_t* pData = static_cast<uint8_t*>(deviceMemory.mapMemory(0, bufferSizeBytes));
    memcpy(pData, indices.data(), bufferSizeBytes);
    deviceMemory.unmapMemory();
}

auto StagingBuffer::calculateSize(const std::vector<VertexObject>& vertices) const -> vk::DeviceSize {
    return vertices.size() * sizeof(VertexObject);
}

auto StagingBuffer::calculateSize(const std::vector<uint32_t>& indices) const -> vk::DeviceSize {
    return indices.size() * sizeof(uint32_t);
}

auto StagingBuffer::getSize() const -> vk::DeviceSize {
    return bufferSizeBytes;
}

auto StagingBuffer::getBuffer() const -> const Buffer& {
    return buffer;
}

}  // namespace global_ao
