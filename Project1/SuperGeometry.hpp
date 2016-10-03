#pragma once

#include <vulkan/vulkan.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include <stddef.h>

namespace Manovreraren 
{
    namespace Geometry 
    {
        struct Vertex {
            glm::vec2 position;
            glm::vec3 color;

            static VkVertexInputBindingDescription GetBindingDescription() {
                VkVertexInputBindingDescription binding_description = {};
                binding_description.binding = 0;
                binding_description.stride = sizeof( Vertex );
                binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
                return binding_description;
            }

            static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions() {
                std::vector<VkVertexInputAttributeDescription> attribute_descriptions( 2 );
                attribute_descriptions[ 0 ].binding = 0;
                attribute_descriptions[ 0 ].location = 0;
                attribute_descriptions[ 0 ].format = VK_FORMAT_R32G32_SFLOAT;
                attribute_descriptions[ 0 ].offset = offsetof(Vertex, position);
                attribute_descriptions[ 1 ].binding = 0;
                attribute_descriptions[ 1 ].location = 1;
                attribute_descriptions[ 1 ].format = VK_FORMAT_R32G32B32_SFLOAT;
                attribute_descriptions[ 1 ].offset = offsetof(Vertex, color);
                return attribute_descriptions;
            }
        };

        class SuperGeometry {
        public:
            SuperGeometry( const VkDevice& device, const VkPhysicalDevice& physical_device );
            virtual ~SuperGeometry();

            const VkBuffer& GetVertexBuffer();
            const uint32_t& GetNumberOfVertices();

        protected:
            const VkDevice* m_p_device = nullptr;
            const VkPhysicalDevice* m_p_physical_device = nullptr;
            VkDeviceMemory m_vertex_buffer_device_memory = VK_NULL_HANDLE;
            VkBuffer m_vertex_buffer = VK_NULL_HANDLE;
            uint32_t m_nr_of_vertices = 0;

            void m_InitVertexBuffer( const std::vector<Vertex>& vertex_list );
            void m_DeInitVertexBuffer();
        };
    }
}