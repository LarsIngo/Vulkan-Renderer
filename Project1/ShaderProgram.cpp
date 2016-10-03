#include "ShaderProgram.hpp"
#include "RendererTools.hpp"

using namespace Manovreraren;

ShaderProgram::ShaderProgram( const VkDevice& device, const VkFormat& format, const VkExtent2D& extent, const std::vector<PrimitiveShaderStage>& primitive_shader_stage_list, const std::vector<Model*>& model_list, const std::vector<VkImageView>& render_target_list )
{
    m_p_device = &device;
    m_format = format;
    m_extent = extent;
    m_p_model_list = &model_list;

    std::vector<VkPipelineShaderStageCreateInfo> shader_stage_list;
    {
        size_t shader_stage_count = primitive_shader_stage_list.size();
        m_shader_module_list.resize( shader_stage_count );
        for ( size_t i = 0; i < shader_stage_count; ++i )
            Tools::CreateShaderModule( device, primitive_shader_stage_list[i].file_path, m_shader_module_list[ i ] );
        for ( size_t i = 0; i < shader_stage_count; ++i )
            shader_stage_list.push_back( Tools::CreatePipelineShaderStageCreateInfo( device, m_shader_module_list[ i ], primitive_shader_stage_list[ i ].shader_stage_flag_bits, primitive_shader_stage_list[ i ].entry_point ) );
    }

    Tools::CreateRenderPass( device, format, m_render_pass );

    Tools::CreatePipelineLayout( device, m_pipeline_layout );

    Tools::CreateGraphicsPipeline( device, extent, shader_stage_list, m_render_pass, m_pipeline_layout, m_graphics_pipeline );

    m_frame_buffer_list.resize( render_target_list.size() );
    for ( size_t i = 0; i < render_target_list.size(); ++i ) {
        Tools::CreateFramebuffer( device, extent, m_render_pass, render_target_list[ i ], m_frame_buffer_list[ i ] );
    }
}


ShaderProgram::~ShaderProgram()
{
    for ( auto& frame_buffer : m_frame_buffer_list ) {
        vkDestroyFramebuffer( *m_p_device, frame_buffer, nullptr );
        frame_buffer = VK_NULL_HANDLE;
    }

    vkDestroyRenderPass( *m_p_device, m_render_pass, nullptr );
    m_render_pass = VK_NULL_HANDLE;

    vkDestroyPipelineLayout( *m_p_device, m_pipeline_layout, nullptr );
    m_pipeline_layout = VK_NULL_HANDLE;

    vkDestroyPipeline( *m_p_device, m_graphics_pipeline, nullptr );
    m_graphics_pipeline = VK_NULL_HANDLE;

    for ( auto& shader_module : m_shader_module_list ) {
        vkDestroyShaderModule( *m_p_device, shader_module, nullptr );
        shader_module = VK_NULL_HANDLE;
    }
}


const VkRenderPass& ShaderProgram::GetRenderPass()
{
    return m_render_pass;
}


const VkPipeline& ShaderProgram::GetGraphicsPipeline()
{
    return m_graphics_pipeline;
}


const std::vector<VkFramebuffer>& ShaderProgram::GetFramebufferList()
{
    return m_frame_buffer_list;
}


const VkFormat& ShaderProgram::GetFormat()
{
    return m_format;
}


const VkExtent2D& ShaderProgram::GetExtent()
{
    return m_extent;
}


const std::vector<Model*>* ShaderProgram::GetModelList()
{
    return m_p_model_list;
}
