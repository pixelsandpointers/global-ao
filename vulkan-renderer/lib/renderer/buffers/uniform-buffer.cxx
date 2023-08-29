#include <lib/renderer/buffers/uniform-buffer-object.hxx>
#include <lib/renderer/buffers/uniform-buffer.hxx>

namespace global_ao {
UniformBuffer::UniformBuffer(const Device& device)
  : bufferSizeBytes { calculateSize() },
    buffer { device,
             bufferSizeBytes,
             vk::BufferUsageFlagBits::eUniformBuffer,
             vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent } {
}

auto UniformBuffer::loadUniformBuffer(const UniformBufferObject& uniformBufferObject) -> void {
    const auto& deviceMemory = buffer.getDeviceMemory();

    uint8_t* pData = static_cast<uint8_t*>(deviceMemory.mapMemory(0, bufferSizeBytes));
    memcpy(pData, &uniformBufferObject, bufferSizeBytes);
    deviceMemory.unmapMemory();
}

auto UniformBuffer::getSize() const -> vk::DeviceSize {
    return bufferSizeBytes;
}

auto UniformBuffer::getBuffer() const -> const Buffer& {
    return buffer;
}

auto UniformBuffer::calculateSize() const -> vk::DeviceSize {
    return sizeof(UniformBufferObject);
}

}  // namespace global_ao