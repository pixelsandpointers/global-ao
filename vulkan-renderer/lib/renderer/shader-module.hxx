#pragma once
#include <lib/renderer/device.hxx>
#include <filesystem>
#include <vector>

namespace global_ao {

class ShaderModule {
  public:
    ShaderModule(const std::filesystem::path& filename, const Device& device);

    auto getShaderModule() const -> const vk::raii::ShaderModule& {
        return shaderModule;
    }

  private:
    auto readCode(const std::filesystem::path& filename) -> std::vector<char>;
    auto createShaderModule() -> vk::raii::ShaderModule;

    const Device& device;
    std::vector<char> code;
    vk::raii::ShaderModule shaderModule;
};

}  // namespace global_ao
