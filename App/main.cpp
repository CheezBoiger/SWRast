// Software rasterizer test.
#include "SoftwareRaster/Context.hpp"
#define STB_IMAGE_WRITE_IMPLEMENTATION 1
#include "stb_image_write.h"
#include <iostream>
#include <vector>

int main(int c, char* argv[])
{
    swrast::initialize();
    swrast::resource_desc_t resource_desc = { };
    resource_desc.width = (sizeof(float) * 4) * 6;
    resource_desc.height = 1;
    resource_desc.type = swrast::resource_type_buffer;
    resource_desc.mip_count = 1;
    resource_desc.depth_or_array_size = 1;
    swrast::resource_t vb = swrast::allocate_resource(resource_desc);

    resource_desc.width = 1920;
    resource_desc.height = 1080;
    resource_desc.format = swrast::format_r8g8b8a8_unorm;
    swrast::resource_t rt = swrast::allocate_resource(resource_desc);

    swrast::shader_t vs = swrast::create_shader(swrast::shader_type_vertex, nullptr, 0);
    swrast::shader_t ps = swrast::create_shader(swrast::shader_type_pixel, nullptr, 0);

    {
        std::vector<float> triangle = 
            { 
                -0.5f,  0.5f, 0.f, 1.f,
                 0.5f,  0.5f, 0.f, 1.f, 
                 0.0f, -0.5f, 0.f, 1.f,
                -0.5f,  -0.5f, 0.f, 1.f,
                -0.5f,   0.5f, 0.f, 1.f, 
                 0.0f,  -0.5f, 0.f, 1.f
            };
       memcpy((void*)vb, triangle.data(), (sizeof(float) * 4) * 6);
    }

    swrast::viewport_t viewport = { };
    viewport.x = viewport.y = 0;
    viewport.width = 1920;
    viewport.height = 1080;
    swrast::bind_render_targets(1, &rt);
    swrast::set_viewports(1, &viewport);
    swrast::bind_vertex_shader(vs);
    swrast::bind_pixel_shader(ps);
    swrast::bind_vertex_buffers(1, &vb);
    swrast::draw_instanced(6, 1, 0, 0);

    int err = stbi_write_png("img.png", viewport.width, viewport.height, 4, (void*)rt, viewport.width * 4);

    swrast::release_resource(vb);
    swrast::release_resource(rt);
    swrast::destroy();
    return 0;
}