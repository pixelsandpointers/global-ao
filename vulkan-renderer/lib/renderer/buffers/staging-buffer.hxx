#pragma once

#include <lib/renderer/buffers/buffer.hxx>
#include <lib/renderer/vertex.hxx>
#include <vector>

namespace global_ao {
class StagingBuffer {
  public:
    StagingBuffer(const Device& device, const std::vector<Vertex>& vertices);

    auto loadVertices(const std::vector<Vertex>& vertices) -> void;
    auto getSize() const -> vk::DeviceSize;
    auto getBuffer() const -> const Buffer&;

  private:
    auto calculateSize(const std::vector<Vertex>& vertices) const -> vk::DeviceSize;

    vk::DeviceSize bufferSizeBytes;
    Buffer buffer;
};

}  // namespace global_ao