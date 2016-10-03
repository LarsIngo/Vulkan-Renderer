#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace Manovreraren
{
    class ShaderProgram;
    namespace Geometry 
    {
        class SuperGeometry;
    }

    class Model
    {
    public:
        Model( const VkDevice& device, const VkCommandPool& command_pool, Geometry::SuperGeometry& mesh );
        ~Model();

        //const std::vector<VkCommandBuffer>& GetCommandBufferList();

        Geometry::SuperGeometry* GetMesh();

    private:
        //void m_InitCommandBuffers();

        const VkDevice* m_p_device = nullptr;
        const VkCommandPool* m_p_command_pool = nullptr;
        Geometry::SuperGeometry* m_p_mesh = nullptr;
        //ShaderProgram* m_p_shader_program = nullptr;
        //
        //std::vector<VkCommandBuffer> m_command_buffer_list;
    };
}
