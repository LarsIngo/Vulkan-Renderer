#include "Renderer.hpp"

#include <string>
#include "RendererTools.hpp"
#include "Image.hpp"
#include "ShaderProgram.hpp"
#include "Model.hpp"
#include "OBJMesh.hpp"

int main() {

    Manovreraren::Renderer renderer( "Manovreraren" );

    auto device = renderer.GetVkDevice();
    //auto queue = renderer.GetVkGraphicsQueue();
    auto gpu = renderer.GetVkPhysicalDevice();



    //Manovreraren::Image image = Manovreraren::Image( device, gpu, renderer.GetExtent(), renderer.GetFormat(), renderer.GetGraphicsFamilyIndex() );
    //std::vector<VkImageView> image_view_list = { image.GetImageView() };

    Manovreraren::Geometry::OBJMesh mesh1 = Manovreraren::Geometry::OBJMesh( device, gpu, "1" );
    Manovreraren::Geometry::OBJMesh mesh2 = Manovreraren::Geometry::OBJMesh( device, gpu, "2" );
    Manovreraren::Model model1 = Manovreraren::Model( device, renderer.GetCommandPool(), mesh1 );
    Manovreraren::Model model2 = Manovreraren::Model( device, renderer.GetCommandPool(), mesh2 );
    std::vector<Manovreraren::Model*> model_list = { &model1, &model2 };

    std::vector<Manovreraren::ShaderProgram::PrimitiveShaderStage> primitive_shader_stage_list;
    primitive_shader_stage_list.push_back( Manovreraren::ShaderProgram::PrimitiveShaderStage( "shaders/vert.spv", VK_SHADER_STAGE_VERTEX_BIT, "main" ) );
    primitive_shader_stage_list.push_back( Manovreraren::ShaderProgram::PrimitiveShaderStage( "shaders/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT, "main" ) );
    Manovreraren::ShaderProgram shader_program = Manovreraren::ShaderProgram( device, renderer.GetFormat(), renderer.GetExtent(), primitive_shader_stage_list, model_list, renderer.GetImageViewList() );
    //Manovreraren::ShaderProgram image_buffer_shader_program = Manovreraren::ShaderProgram(device, renderer.GetFormat(), renderer.GetExtent(), primitive_shader_stage_list, image_view_list );

    //shader_program.AddModel( model ); // Contains a list of models to render

    //Manovreraren::Model image_buffer_model = Manovreraren::Model( device, renderer.GetCommandPool(), mesh1, image_buffer_shader_program );

    //renderer.Bind( shader_program ); 
    while ( renderer.Run() ) {
        //renderer.StartNewFrame();

        //renderer.Render( model1 );
        //renderer.Render( model2 );
        //renderer.Render( image_buffer_model );
        renderer.Render( shader_program ); // Creates Cmd buffer, begin render, draw, end render

        //renderer.EndFrame();
        renderer.Present();
    }

    return 0;
}
