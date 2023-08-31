#pragma once
#include <lib/renderer/device.hxx>
#include <filesystem>
#include <vector>

namespace global_ao {

class ShaderModule {
  public:
    ShaderModule(const Device& device, const std::filesystem::path& shaderPath);

    auto getShaderModule() const -> const vk::raii::ShaderModule& {
        return shaderModule;
    }

  private:
    auto readCode(const std::filesystem::path& shaderPath) -> std::vector<char>;
    auto createShaderModule() -> vk::raii::ShaderModule;

    const Device& device;
    std::vector<char> code;
    vk::raii::ShaderModule shaderModule;
};

}  // namespace global_ao
