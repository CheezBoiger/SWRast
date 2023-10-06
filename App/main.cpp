// Software rasterizer test.
#include "SoftwareRaster/Context.hpp"
#define STB_IMAGE_WRITE_IMPLEMENTATION 1
#include "stb_image_write.h"
#include <iostream>
#include <vector>
#define WINDING_ORDER_COUNTER_CLOCKWISE 0
int main(int c, char* argv[])
{
    swrast::initialize();
    swrast::resource_desc_t resource_desc = { };
    resource_desc.width = (sizeof(float) * 4) * 9;
    resource_desc.height = 1;
    resource_desc.type = swrast::resource_type_buffer;
    resource_desc.mip_count = 1;
    resource_desc.depth_or_array_size = 1;
    swrast::resource_t vb = swrast::allocate_resource(resource_desc);

    resource_desc.width = 800;
    resource_desc.height = 600;
    resource_desc.format = swrast::format_r8g8b8a8_unorm;
    swrast::resource_t rt = swrast::allocate_resource(resource_desc);
   
    resource_desc.format = swrast::format_r32_float;
    swrast::resource_t ds = swrast::allocate_resource(resource_desc);

    swrast::shader_t vs = swrast::create_shader(swrast::shader_type_vertex, nullptr, 0);
    swrast::shader_t ps = swrast::create_shader(swrast::shader_type_pixel, nullptr, 0);

    {
        std::vector<float> triangle = 
            { 
#if WINDING_ORDER_COUNTER_CLOCKWISE
                -0.5f,  0.5f, 0.f, 1.f,
                 0.5f,  0.5f, 0.f, 1.f, 
                 0.0f, -0.5f, 0.f, 1.f,

                -0.5f,  -0.5f, 0.f, 1.f,
                -0.5f,   0.5f, 0.f, 1.f, 
                 0.0f,  -0.5f, 0.f, 1.f
#else
                 0.3f,  0.3f, 0.f, 1.f, 
                -0.3f,  0.3f, 0.f, 1.f,
                 0.0f, -0.3f, 0.f, 1.f,

                 1.f,  0.6f, 1.f, 1.f, 
                -0.5f,  0.3f, -1.f, 3.f,
                 -0.7f, -0.2f, -1.f, 3.f,

                 1.f,   0.6f, 1.f, 1.f, 
                -0.7f,  -0.2f, -1.f, 3.f,
                 0.5f,  -0.5f, 1.f, 1.f
#endif
            };
       memcpy((void*)vb, triangle.data(), (sizeof(float) * 4) * 9);
    }

    swrast::viewport_t viewport = { };
    viewport.x = viewport.y = 0;
    viewport.width = 800;
    viewport.height = 600;
    viewport.near = 0.0000f;
    viewport.far = 1.0f;
    swrast::bind_render_targets(1, &rt, ds);
    swrast::set_front_face(swrast::front_face_clockwise);
    swrast::set_viewports(1, &viewport);
    swrast::bind_vertex_shader(vs);
    swrast::bind_pixel_shader(ps);
    swrast::enable_depth(true);
    swrast::enable_depth_write(true);
    swrast::bind_vertex_buffers(1, &vb);
    swrast::draw_instanced(9, 1, 0, 0);

    int err = stbi_write_png("img.png", viewport.width, viewport.height, 4, (void*)rt, viewport.width * 4);
    err = stbi_write_png("depth.png", viewport.width, viewport.height, 4, (void*)ds, viewport.width * 4);
    swrast::release_resource(ds);
    swrast::release_resource(vb);
    swrast::release_resource(rt);
    swrast::destroy();
    return 0;
}