#include <lib/renderer/buffers/staging-buffer.hxx>

namespace global_ao {
StagingBuffer::StagingBuffer(const Device& device, const std::vector<Vertex>& vertices)
  : bufferSizeBytes { calculateSize(vertices) },
    buffer { device,
             bufferSizeBytes,
             vk::BufferUsageFlagBits::eTransferSrc,
             vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent }

{
}

auto StagingBuffer::loadVertices(const std::vector<Vertex>& vertices) -> void {
    const auto& deviceMemory = buffer.getDeviceMemory();

    uint8_t* pData = static_cast<uint8_t*>(deviceMemory.mapMemory(0, bufferSizeBytes));
    memcpy(pData, vertices.data(), bufferSizeBytes);
}

auto StagingBuffer::calculateSize(const std::vector<Vertex>& vertices) const -> vk::DeviceSize {
    return vertices.size() * sizeof(Vertex);
}

auto StagingBuffer::getSize() const -> vk::DeviceSize {
    return bufferSizeBytes;
}

auto StagingBuffer::getBuffer() const -> const Buffer& {
    return buffer;
}
}  // namespace global_ao
