#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace Manovreraren
{
    class Model;

    class ShaderProgram
    {
    public:
        struct PrimitiveShaderStage {
            const char* file_path;
            VkShaderStageFlagBits shader_stage_flag_bits;
            const char* entry_point;

            PrimitiveShaderStage( const char* file_path, const VkShaderStageFlagBits& shader_stage_flag_bits, const char* entry_point )
            {
                this->file_path = file_path;
                this->shader_stage_flag_bits = shader_stage_flag_bits;
                this->entry_point = entry_point;
            }
        };

        ShaderProgram( const VkDevice& device, const VkFormat& format, const VkExtent2D& extent, const std::vector<PrimitiveShaderStage>& primitive_shader_stage_list, const std::vector<Model*>& model_list, const std::vector<VkImageView>& render_target_list );
        ~ShaderProgram();

        const VkRenderPass& GetRenderPass();
        const VkPipeline& GetGraphicsPipeline();
        const std::vector<VkFramebuffer>& GetFramebufferList();
        const VkFormat& GetFormat();
        const VkExtent2D& GetExtent();
        const std::vector<Model*>* GetModelList();

    private:
        const VkDevice* m_p_device = nullptr;

        const std::vector<Model*>* m_p_model_list = nullptr;

        VkFormat m_format = {};
        VkExtent2D m_extent = {};
        std::vector<VkShaderModule> m_shader_module_list;
        VkRenderPass  m_render_pass = VK_NULL_HANDLE;
        VkPipelineLayout  m_pipeline_layout = VK_NULL_HANDLE;
        VkPipeline  m_graphics_pipeline = VK_NULL_HANDLE;

        std::vector<VkFramebuffer> m_frame_buffer_list;
    };
}
