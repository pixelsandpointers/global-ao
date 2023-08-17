#pragma once

#include <lib/renderer/buffers/buffer.hxx>
#include <lib/renderer/buffers/vertex-object.hxx>
#include <vector>

namespace global_ao {
template<typename T>
class StagingBuffer {
  public:
    StagingBuffer(const Device& device, const std::vector<T>& data)
      : bufferSizeBytes { calculateSize(data) },
        buffer { device,
                 bufferSizeBytes,
                 vk::BufferUsageFlagBits::eTransferSrc,
                 vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent } {
        loadData(data);
    }

    StagingBuffer(const Device& device, const T* const data, size_t size)
      : bufferSizeBytes { size * sizeof(T) },
        buffer { device,
                 bufferSizeBytes,
                 vk::BufferUsageFlagBits::eTransferSrc,
                 vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent } {
        loadData(data);
    }

    auto getSize() const -> vk::DeviceSize {
        return bufferSizeBytes;
    }

    auto getBuffer() const -> const Buffer& {
        return buffer;
    }


  private:
    auto calculateSize(const std::vector<T>& data) const -> vk::DeviceSize {
        return data.size() * sizeof(T);
    }

    auto loadData(const std::vector<T>& data) -> void {
        const auto& deviceMemory = buffer.getDeviceMemory();

        uint8_t* pData = static_cast<uint8_t*>(deviceMemory.mapMemory(0, bufferSizeBytes));
        memcpy(pData, data.data(), bufferSizeBytes);
        deviceMemory.unmapMemory();
    }

    auto loadData(const T* const data) -> void {
        const auto& deviceMemory = buffer.getDeviceMemory();

        uint8_t* pData = static_cast<uint8_t*>(deviceMemory.mapMemory(0, bufferSizeBytes));
        memcpy(pData, data, bufferSizeBytes);
        deviceMemory.unmapMemory();
    }

    vk::DeviceSize bufferSizeBytes;
    Buffer buffer;
};

}  // namespace global_ao