#pragma once
#include <lib/renderer/buffer-objects/uniform-buffer-object.hxx>
#include <lib/renderer/buffers/buffer.hxx>
#include <lib/renderer/device.hxx>

namespace global_ao {

template<typename UniformBufferObjectT>
class UniformBuffer {
  public:
    explicit UniformBuffer(const Device& device)
      : bufferSizeBytes { calculateSize() },
        buffer { device,
                 bufferSizeBytes,
                 vk::BufferUsageFlagBits::eUniformBuffer,
                 vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent } {
    }

    auto loadUniformBufferObject(const UniformBufferObjectT& uniformBufferObject) const -> void {
        const auto& deviceMemory = buffer.getDeviceMemory();

        uint8_t* pData = static_cast<uint8_t*>(deviceMemory.mapMemory(0, bufferSizeBytes));
        memcpy(pData, &uniformBufferObject, bufferSizeBytes);
        deviceMemory.unmapMemory();
    }

    auto getSize() const -> vk::DeviceSize {
        return bufferSizeBytes;
    }

    auto getBuffer() const -> const Buffer& {
        return buffer;
    }

  private:
    auto calculateSize() const -> vk::DeviceSize {
        return sizeof(UniformBufferObjectT);
    }

    vk::DeviceSize bufferSizeBytes;
    Buffer buffer;
};

}  // namespace global_ao