#ifndef _GLTF_LOADER_H_H_
#define _GLTF_LOADER_H_H_

#include <memory>
#include <mutex>
#include <string>

#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include <tiny_gltf.h>

#include "app/vk_device_manager.h"

#include "submesh.h"

namespace Scene
{
    class GltfLoader
    {
    public:
        explicit GltfLoader(APP::VkDeviceManager* deviceManager);
        virtual ~GltfLoader() = default;
        std::unique_ptr<SubMesh> read_model_from_file(const std::string& file_name, uint32_t index);
    private:

        std::unique_ptr<SubMesh> loadModel(uint32_t index);

        APP::VkDeviceManager* m_deviceManager{ nullptr };
        tinygltf::Model                 m_model;
    };
}

#endif
