#include "Model.hpp"
#include "RendererTools.hpp"
#include "ShaderProgram.hpp"

using namespace Manovreraren;

Model::Model( const VkDevice& device, const VkCommandPool& command_pool, Geometry::SuperGeometry& mesh )
{
    m_p_device = &device;
    m_p_command_pool = &command_pool;
    m_p_mesh = &mesh;
    //m_p_shader_program = &shader_program;

    //m_InitCommandBuffers();
}


Model::~Model()
{
    //vkDeviceWaitIdle( *m_p_device );

    //vkFreeCommandBuffers( *m_p_device, *m_p_command_pool, static_cast<uint32_t>( m_command_buffer_list.size() ), m_command_buffer_list.data() );
}


Geometry::SuperGeometry* Model::GetMesh()
{
    return m_p_mesh;
}


//void Model::m_InitCommandBuffers()
//{
//    size_t command_buffer_count = static_cast<uint32_t>( m_p_shader_program->GetFramebufferList().size() );
//    m_command_buffer_list.resize( command_buffer_count );
//    Tools::CreateCommandBuffers( *m_p_device, *m_p_command_pool, static_cast<uint32_t>( command_buffer_count ), m_command_buffer_list.data() );
//
//    for ( size_t i = 0; i < command_buffer_count; ++i ) {
//        VkCommandBufferBeginInfo command_buffer_begin_info = {};
//        command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//        command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
//
//        Tools::VkErrorCheck( vkBeginCommandBuffer( m_command_buffer_list[ i ], &command_buffer_begin_info ) );
//
//        VkRenderPassBeginInfo render_pass_begin_info = {};
//        render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//        render_pass_begin_info.renderPass = m_p_shader_program->GetRenderPass();
//        render_pass_begin_info.framebuffer = m_p_shader_program->GetFramebufferList()[ i ];
//        render_pass_begin_info.renderArea.offset = { 0, 0 };
//        render_pass_begin_info.renderArea.extent = m_p_shader_program->GetExtent();
//
//        VkClearValue clear_color = { i%2, 0.f, 0.f, 1.f };
//        render_pass_begin_info.clearValueCount = 1;
//        render_pass_begin_info.pClearValues = &clear_color;
//
//        vkCmdBeginRenderPass( m_command_buffer_list[ i ], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE );
//
//        vkCmdBindPipeline( m_command_buffer_list[ i ], VK_PIPELINE_BIND_POINT_GRAPHICS, m_p_shader_program->GetGraphicsPipeline() );
//
//        VkBuffer vertex_Buffer_list[] = { m_p_mesh->GetVertexBuffer() };
//        VkDeviceSize offsets[] = { 0 };
//        vkCmdBindVertexBuffers( m_command_buffer_list[ i ], 0, 1, vertex_Buffer_list, offsets );
//
//        vkCmdDraw( m_command_buffer_list[ i ], m_p_mesh->GetNumberOfVertices(), 1, 0, 0 );
//
//        vkCmdEndRenderPass( m_command_buffer_list[ i ] );
//
//        Tools::VkErrorCheck( vkEndCommandBuffer( m_command_buffer_list[ i ] ) );
//    }
//}
//
//
//const std::vector<VkCommandBuffer>& Model::GetCommandBufferList()
//{
//    return m_command_buffer_list;
//}
