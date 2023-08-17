#pragma once
#include <filesystem>
#include <lib/renderer/buffers/staging-buffer.txx>
#include <lib/renderer/command-buffers.hxx>
#include <lib/renderer/command-pool.hxx>
#include <lib/renderer/device.hxx>
#include <memory>
#include <stb_image.h>

namespace global_ao {

class TextureImage {
  public:
    TextureImage(
        const Device& device,
        const std::filesystem::path& texturePath,
        const vk::raii::Queue& graphicsQueue,
        const CommandPool& commandPool);

    auto getTextureImageView() const -> const vk::raii::ImageView&;


  private:
    auto loadImageData(const std::filesystem::path& texturePath) -> std::unique_ptr<stbi_uc, void (*)(void*)>;
    auto createStagingBuffer(const Device& device, const std::unique_ptr<stbi_uc, void (*)(void*)>& imageData) const
        -> StagingBuffer<stbi_uc>;
    auto createImage(const Device& device) -> vk::raii::Image;
    auto loadTexture(const vk::raii::Queue& graphicsQueue, const CommandPool& commandPool) -> void;
    auto transitionImageLayout(
        const vk::raii::Queue& graphicsQueue,
        const vk::raii::CommandBuffer& commandBuffer,
        vk::ImageLayout oldLayout,
        vk::ImageLayout newLayout) -> void;
    auto copyBufferToImage(const vk::raii::Queue& graphicsQueue, const vk::raii::CommandBuffer& commandBuffer) -> void;
    auto recordCommandBufferToTransitionImageLayout(
        const vk::raii::CommandBuffer& commandBuffer,
        vk::ImageLayout oldLayout,
        vk::ImageLayout newLayout) -> void;
    auto recordCommandBufferToCopyBufferToImage(const vk::raii::CommandBuffer& commandBuffer) -> void;
    auto createImageView(const Device& device) -> vk::raii::ImageView;

    const vk::ImageLayout initialLayout = vk::ImageLayout::eUndefined;
    int width = 0;
    int height = 0;
    int channels = 0;
    vk::DeviceSize imageSize;
    std::unique_ptr<StagingBuffer<stbi_uc>> stagingBuffer;  // unique_ptr so we can free the image data after the
                                                            // staging buffer was used to load the image into the GPU
    vk::raii::Image textureImage;
    vk::raii::DeviceMemory textureImageMemory;
    vk::raii::ImageView textureImageView;
};

}  // namespace global_ao
