#pragma once
#include <lib/renderer/device.hxx>
#include <vulkan/vulkan_raii.hpp>

namespace global_ao {

class SyncObjectsHandler {
  public:
    explicit SyncObjectsHandler(const Device& device);;

    struct SyncObjects {
        vk::raii::Semaphore imageAvailableSemaphore;
        vk::raii::Semaphore renderFinishedSemaphore;
        vk::raii::Fence inFlightFence;
    };

    auto getSyncObjects() const -> const SyncObjects&;

  private:
    auto createSyncObjects() -> SyncObjects;

    const Device& device;
    SyncObjects syncObjects;
};

}  // namespace global_ao
