#include "SuperGeometry.hpp"

#include "RendererTools.hpp"

using namespace Manovreraren;
using namespace Geometry;

SuperGeometry::SuperGeometry( const VkDevice& device, const VkPhysicalDevice& physical_device )
{
    m_p_device = &device;
    m_p_physical_device = &physical_device;
}

SuperGeometry::~SuperGeometry() 
{
    vkDeviceWaitIdle( *m_p_device );

    m_DeInitVertexBuffer();
}


void SuperGeometry::m_InitVertexBuffer( const std::vector<Vertex>& vertex_list )
{
    m_nr_of_vertices = static_cast<uint32_t>( vertex_list.size() );

    VkBufferCreateInfo buffer_create_info = {};
    buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.size = sizeof( vertex_list[ 0 ] ) * m_nr_of_vertices;
    buffer_create_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    Tools::VkErrorCheck(vkCreateBuffer( *m_p_device, &buffer_create_info, nullptr, &m_vertex_buffer ) );

    {
        VkMemoryRequirements memory_requirements;
        vkGetBufferMemoryRequirements( *m_p_device, m_vertex_buffer, &memory_requirements);

        VkMemoryAllocateInfo memory_allocate_info = {};
        memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memory_allocate_info.allocationSize = memory_requirements.size;
        memory_allocate_info.memoryTypeIndex = Tools::FindMemoryType( *m_p_physical_device, memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT );

        // TODO vkAllocateMemory large chunk of memory, then vkBindxMemory() on same memory chunk 
        Tools::VkErrorCheck( vkAllocateMemory( *m_p_device, &memory_allocate_info, nullptr, &m_vertex_buffer_device_memory ) );
    }

    Tools::VkErrorCheck( vkBindBufferMemory( *m_p_device, m_vertex_buffer, m_vertex_buffer_device_memory, 0) );

    void* data;
    vkMapMemory( *m_p_device, m_vertex_buffer_device_memory, 0, buffer_create_info.size, 0, &data);
    memcpy( data, vertex_list.data(), (size_t)buffer_create_info.size );
    vkUnmapMemory( *m_p_device, m_vertex_buffer_device_memory );
}


void SuperGeometry::m_DeInitVertexBuffer()
{
    vkDestroyBuffer( *m_p_device, m_vertex_buffer, nullptr);
    m_vertex_buffer = VK_NULL_HANDLE;
    vkFreeMemory( *m_p_device, m_vertex_buffer_device_memory, nullptr );
    m_vertex_buffer_device_memory = VK_NULL_HANDLE;
}


const VkBuffer& SuperGeometry::GetVertexBuffer()
{
    return m_vertex_buffer;
}


const uint32_t& SuperGeometry::GetNumberOfVertices()
{
    return m_nr_of_vertices;
}
