#pragma once

#include <vulkan/vulkan.h>

namespace Manovreraren 
{
    class Image 
    {
    public:
        Image( const VkDevice& device, const VkPhysicalDevice physical_device, const VkExtent2D& extent, const VkFormat& format, const uint32_t& graphics_family_index);
        ~Image();

        const VkImageCreateInfo& GetImageInfo();
        const VkImageView& GetImageView();
        const VkImage& GetImage();

    private:
        const VkDevice* m_p_device = nullptr;
        const VkPhysicalDevice* m_p_physical_device = nullptr;
        VkExtent2D m_extent = {};
        VkFormat m_format = {};
        uint32_t m_graphics_family_index = 0;

        VkImageCreateInfo m_image_info = {};
        VkImage m_image = VK_NULL_HANDLE;
        VkImageView m_image_view = VK_NULL_HANDLE;
        VkDeviceMemory m_image_device_memory = VK_NULL_HANDLE;
        //VkFramebuffer m_framebuffer = VK_NULL_HANDLE;

        void m_InitImage();
        void m_DeInitImage();

        void m_InitImageView();
        void m_DeInitImageView();

        //void m_InitFramebuffer();
        //void m_DeInitFramebuffer();
    };
}
