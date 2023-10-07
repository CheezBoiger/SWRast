// Software rasterizer test.
#include "SoftwareRaster/Context.hpp"
#define STB_IMAGE_WRITE_IMPLEMENTATION 1
#include "stb_image_write.h"
#include <iostream>
#include <vector>
#define WINDING_ORDER_COUNTER_CLOCKWISE 0


// Vertex shader implementation.
class simple_vertex_t : public swrast::vertex_shader_t
{
public:

    struct in_vert_t
    {
        swrast::float4_t pos;
        swrast::float4_t color;
    };

    struct out_vert_t
    {
        swrast::float4_t color;
        swrast::float4_t pos;
    };

    void setup() override
    {
        set_out_vert_info(sizeof(out_vert_t), sizeof(swrast::float4_t));
        set_in_vert_info(sizeof(in_vert_t));
    }

    // Must output a vertex, in clip space.
    void execute(uintptr_t in_vertex_ptr, uintptr_t out_vertex, uint32_t vert_id) override
    {
        swrast::float3_t c[] = { {1, 0, 0}, {0, 1, 0}, {0, 0, 1} }; 
        in_vert_t* in_vert = (in_vert_t*)in_vertex_ptr;
        out_vert_t* out = (out_vert_t*)out_vertex;
        out->pos = in_vert->pos;
        out->color = in_vert->color;
    }
};

// Pixel shader implementation.
class simple_pixel_t : public swrast::pixel_shader_t
{
public:
    struct in_varying_t
    {
        swrast::float4_t color;
        swrast::float4_t pos;
    };

    virtual void setup() override
    {
        set_varying_info(sizeof(in_varying_t), sizeof(swrast::float4_t));
    }

    virtual void interpolate_varying(uintptr_t v0, uintptr_t v1, uintptr_t v2, const swrast::float3_t& barycentrics) override
    {
        in_varying_t* varying_v0 = (in_varying_t*)v0;
        in_varying_t* varying_v1 = (in_varying_t*)v1;
        in_varying_t* varying_v2 = (in_varying_t*)v2;
        varying.color.r = varying_v0->color.r * barycentrics[0] + varying_v1->color.r * barycentrics[1] + varying_v2->color.r * barycentrics[2];
        varying.color.g = varying_v0->color.g * barycentrics[0] + varying_v1->color.g * barycentrics[1] + varying_v2->color.g * barycentrics[2];
        varying.color.b = varying_v0->color.b * barycentrics[0] + varying_v1->color.b * barycentrics[1] + varying_v2->color.b * barycentrics[2];
        varying.color.a = 1;
    }
    // Should output the color. Ideally we want to pass in the 
    // screen space coordinates, which might be used for other processes.
    // We will also want to pass any vertex attributes that might need to be 
    // used for texturing as well.
    swrast::float4_t execute() override 
    {
        return varying.color; 
    }
private:
    in_varying_t varying;
};

int main(int c, char* argv[])
{
    swrast::initialize();
    swrast::resource_desc_t resource_desc = { };
    resource_desc.width = (sizeof(float) * 8) * 9;
    resource_desc.height = 1;
    resource_desc.type = swrast::resource_type_buffer;
    resource_desc.mip_count = 1;
    resource_desc.depth_or_array_size = 1;
    swrast::resource_t vb = swrast::allocate_resource(resource_desc);

    resource_desc.width = 1920;
    resource_desc.height = 1080;
    resource_desc.format = swrast::format_r8g8b8a8_unorm;
    swrast::resource_t rt = swrast::allocate_resource(resource_desc);
   
    resource_desc.format = swrast::format_r32_float;
    swrast::resource_t ds = swrast::allocate_resource(resource_desc);

    //swrast::shader_t vs = swrast::create_shader(swrast::shader_type_vertex, nullptr, 0);
    //swrast::shader_t ps = swrast::create_shader(swrast::shader_type_pixel, nullptr, 0);
    swrast::input_layout_t layout = 0;
    {
        swrast::input_element_desc inputs[2];
        inputs[0].format = swrast::format_r32g32b32a32_float;
        inputs[0].input_slot = 0;
        inputs[0].offset = 0;
        inputs[1].format = swrast::format_r32g32b32a32_float;
        inputs[1].input_slot = 0;
        inputs[1].offset = 16;
        
        layout = swrast::create_input_layout(2, inputs);
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
                 0.3f,  0.3f, 0.f, 1.f,       0, 0, 1, 1,
                -0.3f,  0.3f, 0.f, 1.f,       0, 1, 0, 1,
                 0.0f, -0.3f, 0.f, 1.f,       1, 0, 0, 1,

                 1.f,  0.6f, 1.f, 1.f,        1, 0, 0, 1,
                -0.5f,  0.3f, -1.f, 3.f,      0, 1, 0, 1,
                 -0.7f, -0.2f, -1.f, 3.f,     0, 0, 1, 1,

                 1.f,   0.6f, 1.f, 1.f,       1, 0, 0, 1,
                -0.7f,  -0.2f, -1.f, 3.f,     0, 0, 1, 1,
                 0.5f,  -0.5f, 1.f, 1.f,      0, 1, 0, 1
#endif
            };
       memcpy((void*)vb, triangle.data(), (sizeof(float) * 8) * 9);
    }

    simple_vertex_t* vs = new simple_vertex_t();
    vs->setup();
    simple_pixel_t* ps = new simple_pixel_t();
    ps->setup();

    swrast::viewport_t viewport = { };
    viewport.x = viewport.y = 0;
    viewport.width = 1920;
    viewport.height = 1080;
    viewport.near = 0.0000f;
    viewport.far = 1.0f;
    swrast::bind_render_targets(1, &rt, ds);
    float rgba[4] = { 0.f, 0.3f, 0.3f, 1.f };
    swrast::rect_t clear_rect = { };
    clear_rect.x = 0;
    clear_rect.y = 0;
    clear_rect.width = viewport.width / 2;
    clear_rect.height = viewport.height / 2;
    swrast::clear_render_target(0, clear_rect, rgba);
    swrast::set_front_face(swrast::front_face_clockwise);
    swrast::set_input_layout(layout);
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