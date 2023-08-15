#pragma once

#include <lib/renderer/buffers/buffer.hxx>
#include <lib/renderer/buffers/vertex-object.hxx>
#include <vector>

namespace global_ao {
// TODO: make this a templated based on the data
class StagingBuffer {
  public:
    StagingBuffer(const Device& device, const std::vector<VertexObject>& vertices);
    StagingBuffer(const Device& device, const std::vector<uint32_t>& indices);

    auto loadVertices(const std::vector<VertexObject>& vertices) -> void;
    auto loadIndices(const std::vector<uint32_t>& indices) -> void;
    auto getSize() const -> vk::DeviceSize;
    auto getBuffer() const -> const Buffer&;

  private:
    auto calculateSize(const std::vector<VertexObject>& vertices) const -> vk::DeviceSize;
    auto calculateSize(const std::vector<uint32_t>& indices) const -> vk::DeviceSize;

    vk::DeviceSize bufferSizeBytes;
    Buffer buffer;
};

}  // namespace global_ao