// Software rasterizer test.
#include "SoftwareRaster/Context.hpp"
#define STB_IMAGE_WRITE_IMPLEMENTATION 1
#define STB_IMAGE_IMPLEMENTATION 1
#include "stb_image_write.h"
#include "stb_image.h"
#include <iostream>
#include <vector>
#define WINDING_ORDER_COUNTER_CLOCKWISE 0

#define CHECKERBOARD_TEXTURE 0
#define USE_TEXTURE_FETCH 0

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#include <dxgi.h>
#include <d3d11.h>

#undef near
#undef far

// Vertex shader implementation.
class simple_vertex_t : public swrast::vertex_shader_t
{
public:
    swrast::float4x4_t mvp;
    swrast::float4x4_t m;
    swrast::float4x4_t n;

    struct in_vert_t
    {
        swrast::float3_t pos;
        swrast::float4_t color;
        swrast::float3_t normal;
        swrast::float2_t texcoord;
    };

    struct out_vert_t
    {
        swrast::float4_t color;
        swrast::float4_t pos;
        swrast::float3_t normal;
        swrast::float2_t texcoord;
        swrast::float3_t frag_pos;
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
        out->pos = swrast::float4_t(in_vert->pos, 1.0f) * mvp;
        out->color = in_vert->color;
        out->texcoord = in_vert->texcoord;
        swrast::float4_t frag_pos = swrast::float4_t(in_vert->pos, 1.0f) * m;
        
        out->frag_pos = swrast::float3_t(frag_pos.x, frag_pos.y, frag_pos.z);
        swrast::float4_t nn = swrast::float4_t(in_vert->normal, 0.0f) * n;
        out->normal = swrast::float3_t(nn.x, nn.y, nn.z);
    }
};

// Pixel shader implementation.
class simple_pixel_t : public swrast::pixel_shader_t
{
public:
    swrast::resource_t m_texture;

    swrast::float3_t light_pos;
    swrast::float4_t light_color;
    
    struct in_varying_t
    {
        swrast::float4_t color;
        swrast::float4_t pos;
        swrast::float3_t normal;
        swrast::float2_t texcoord;
        swrast::float3_t frag_pos;
    };

    virtual void setup() override
    {
        set_varying_info(sizeof(in_varying_t), sizeof(swrast::float4_t));
        varying_attribute(0, data_type_float4, interpolation_perspective);
        varying_attribute(16, data_type_float4, interpolation_perspective);
        varying_attribute(32, data_type_float3, interpolation_perspective);
        varying_attribute(44, data_type_float3, interpolation_perspective);
        varying_attribute(56, data_type_float2, interpolation_perspective);
        varying_attribute(64, data_type_float3, interpolation_perspective);
        m_texture = 0;
    }

    // Should output the color. Ideally we want to pass in the 
    // screen space coordinates, which might be used for other processes.
    // We will also want to pass any vertex attributes that might need to be 
    // used for texturing as well.
    swrast::float4_t execute(uintptr_t varying_address) override 
    {
        in_varying_t* varying = (in_varying_t*)varying_address;
        // checkerboard pattern
        //const int M = 10;
        //float p = (fmod(varying.texcoord.s * M, 1.0) > 0.5) ^ (fmod(varying.texcoord.t * M, 1.0) < 0.5);
        //return swrast::float4_t(p, p, p, 1); 
        //swrast::float4_t color = textureFetch(m_texture, varying.texcoord);
        swrast::sampler_desc_t desc;
        desc.filter = swrast::sampler_filter_linear;
        desc.address_u = swrast::texture_address_mode_clamp;
        desc.address_v = swrast::texture_address_mode_clamp;
        //varying.texcoord = varying.texcoord * 2.5f;
        swrast::float4_t color;
#if USE_TEXTURE_FETCH
        swrast::float3_t tex_size = texture_size(m_texture);
        swrast::uint x = swrast::clamp(varying.texcoord.x * tex_size.x, 0.f, tex_size.x - 1);
        swrast::uint y = swrast::clamp(varying.texcoord.y * tex_size.y, 0.f, tex_size.y - 1);
        color = textureFetch(m_texture, swrast::uint2_t(x, y));
#else
        //color = texture(m_texture, desc, varying->texcoord);

        swrast::float3_t N = swrast::normalize(varying->normal);
        swrast::float3_t P = varying->frag_pos;
        swrast::float3_t LightDir = swrast::normalize(light_pos - P);
        swrast::float4_t diffuse_color = texture(m_texture, desc, varying->texcoord);
        float diff = swrast::maximum<float, float, float>(swrast::dot(N, LightDir), 0.0f);
        swrast::float3_t ambient = swrast::float3_t(diffuse_color.x, diffuse_color.y, diffuse_color.z) * 0.1;
        swrast::float3_t diffuse = diff * diffuse_color;
        color = swrast::float4_t(diffuse + ambient, 1.0f);
#endif
        return color;
    }
};

int main(int c, char* argv[])
{
    uint32_t screen_width = 1920;
    uint32_t screen_height = 1080;
    swrast::initialize();
    swrast::resource_desc_t resource_desc = { };
    resource_desc.width = (sizeof(float) * 12) * (9 + 36);
    resource_desc.height = 1;
    resource_desc.type = swrast::resource_type_buffer;
    resource_desc.mip_count = 1;
    resource_desc.depth_or_array_size = 1;
    swrast::resource_t vb = swrast::allocate_resource(resource_desc);

    resource_desc.width = sizeof(swrast::uint) * 36;
    swrast::resource_t ib = swrast::allocate_resource(resource_desc);

    resource_desc.width = screen_width;
    resource_desc.height = screen_height;
    resource_desc.format = swrast::format_r8g8b8a8_unorm;
    swrast::resource_t rt = swrast::allocate_resource(resource_desc);
   
    resource_desc.format = swrast::format_r32_float;
    swrast::resource_t ds = swrast::allocate_resource(resource_desc);

    int width = 128;
    int height = 128;
#if !CHECKERBOARD_TEXTURE
    int n;
    void* data = stbi_load("example.png", &width, &height, &n, 4);
#endif
    resource_desc.format = swrast::format_r8g8b8a8_unorm;
    resource_desc.width = width;
    resource_desc.height = height;
    swrast::resource_t tex = swrast::allocate_resource(resource_desc);

#if CHECKERBOARD_TEXTURE
    for (uint32_t y = 0; y < width; ++y)
    {
        for (uint32_t x = 0; x < height; ++x)
        {
            uint32_t c = (((y & 0x8) == 0) ^ ((x & 0x8)  == 0)) * 255;
            uint32_t* address = (uint32_t*)(tex + x * 4 + (128 * 4 * y));
            *address = ((c) | (c << 8) | (c << 16) | (255 << 24));
        }
    }
#else
    for (uint32_t y = 0; y < height; ++y)
    {
        for (uint32_t x = 0; x < width; ++x)
        {
            uint32_t c = *(uint32_t*)(((uintptr_t)data) + x * 4 + (width * 4 * y));
            uint32_t* address = (uint32_t*)(tex + x * 4 + (width * 4 * y));
            *address = c;
        }
    }
    stbi_image_free(data);
#endif

    //swrast::shader_t vs = swrast::create_shader(swrast::shader_type_vertex, nullptr, 0);
    //swrast::shader_t ps = swrast::create_shader(swrast::shader_type_pixel, nullptr, 0);
    swrast::input_layout_t layout = 0;
    {
        swrast::input_element_desc inputs[4];
        inputs[0].format = swrast::format_r32g32b32_float;
        inputs[0].input_slot = 0;
        inputs[0].offset = 0;
        inputs[1].format = swrast::format_r32g32b32a32_float;
        inputs[1].input_slot = 0;
        inputs[1].offset = 12;
        inputs[2].format = swrast::format_r32g32b32_float;
        inputs[2].input_slot = 0;
        inputs[2].offset = 28;
        inputs[3].format = swrast::format_r32g32_float;
        inputs[3].input_slot = 0;
        inputs[3].offset = 40;
        
        layout = swrast::create_input_layout(4, inputs);
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
                // Position              Color          Normal          TexCoord
                 0.5f,  0.5f, 0.f,       0, 0, 1, 1,    0, 1, 0,        1, 0,
                -0.5f,  0.5f, 0.f,       0, 1, 0, 1,    0, 1, 0,        1, 1,
                 0.0f, -0.5f, 0.f,       1, 0, 0, 1,    0, 1, 0,        0, 0.5,

                 0.5f,  0.5f, 0.f,       1, 0, 0, 1,    0, 1, 0,        1, 1,
                 -0.5f, 0.5f, 0.f,       0, 0, 1, 1,    0, 1, 0,        0, 1,
                -0.5f,  -0.5f, 0.f,      0, 1, 0, 1,    0, 1, 0,        0, 0,

                 0.5f,  -0.5f, 0.f,      0, 0, 1, 1,    0, 1, 0,        1, 0,
                 0.5f,   0.5f, 0.f,      1, 0, 0, 1,    0, 1, 0,        1, 1,
                -0.5f,  -0.5f, 0.f,      0, 1, 0, 1,    0, 1, 0,        0, 0,

                 // front                                                                             
                -1.0f, -1.0f, 1.0f,      0, 1, 1, 1,   0, 0, 1,         0, 0,
                 1.0f, -1.0f, 1.0f,      0, 1, 1, 1,   0, 0, 1,         1, 0,
                 1.0f,  1.0f, 1.0f,      0, 1, 1, 1,   0, 0, 1,         1, 1,
                 1.0f,  1.0f, 1.0f,      0, 1, 1, 1,   0, 0, 1,         1, 1,
                -1.0f,  1.0f, 1.0f,      0, 1, 1, 1,   0, 0, 1,         0, 1,
                -1.0f, -1.0f, 1.0f,      0, 1, 1, 1,   0, 0, 1,         0, 0,
                // Back                                                     
                -1.0f, -1.0f, -1.0f,     1, 0, 1, 1,   0, 0, -1,        0, 0,
                -1.0f,  1.0f, -1.0f,     1, 0, 1, 1,   0, 0, -1,        1, 0,
                 1.0f,  1.0f, -1.0f,     1, 0, 1, 1,   0, 0, -1,        1, 1,
                 1.0f,  1.0f, -1.0f,     1, 0, 1, 1,   0, 0, -1,        1, 1,
                 1.0f, -1.0f, -1.0f,     1, 0, 1, 1,   0, 0, -1,        0, 1,
                -1.0f, -1.0f, -1.0f,     1, 0, 1, 1,   0, 0, -1,        0, 0,
                  // up                                                     
                 1.0f,  1.0f,  1.0f,     1, 1, 0, 1,   0, 1, 0,        0, 0,
                 1.0f,  1.0f, -1.0f,     1, 1, 0, 1,   0, 1, 0,        1, 0,
                -1.0f,  1.0f, -1.0f,     1, 1, 0, 1,   0, 1, 0,        1, 1,
                -1.0f,  1.0f, -1.0f,     1, 1, 0, 1,   0, 1, 0,        1, 1,
                -1.0f,  1.0f,  1.0f,     1, 1, 0, 1,   0, 1, 0,        0, 1,
                 1.0f,  1.0f,  1.0f,     1, 1, 0, 1,   0, 1, 0,        0, 0,
                // Down                                                     
                 1.0f, -1.0f,  1.0f,     1, 0, 0, 1,   0, -1, 0,       0, 0,
                -1.0f, -1.0f,  1.0f,     1, 0, 0, 1,   0, -1, 0,       1, 0,
                -1.0f, -1.0f, -1.0f,     1, 0, 0, 1,   0, -1, 0,       1, 1,
                -1.0f, -1.0f, -1.0f,     1, 0, 0, 1,   0, -1, 0,       1, 1,
                 1.0f, -1.0f, -1.0f,     1, 0, 0, 1,   0, -1, 0,       0, 1,
                 1.0f, -1.0f,  1.0f,     1, 0, 0, 1,   0, -1, 0,       0, 0,
                // right                                                    
                 1.0f, -1.0f,  1.0f,     0, 1, 0, 1,   1, 0, 0,        0, 0,
                 1.0f, -1.0f, -1.0f,     0, 1, 0, 1,   1, 0, 0,        1, 0,
                 1.0f,  1.0f, -1.0f,     0, 1, 0, 1,   1, 0, 0,        1, 1,
                 1.0f,  1.0f, -1.0f,     0, 1, 0, 1,   1, 0, 0,        1, 1,
                 1.0f,  1.0f,  1.0f,     0, 1, 0, 1,   1, 0, 0,        0, 1,
                 1.0f, -1.0f,  1.0f,     0, 1, 0, 1,   1, 0, 0,        0, 0,
                // Left                                                     
                -1.0f, -1.0f,  1.0f,     0, 0, 1, 1,   -1, 0, 0,       0, 0,
                -1.0f,  1.0f,  1.0f,     0, 0, 1, 1,   -1, 0, 0,       1, 0,
                -1.0f,  1.0f, -1.0f,     0, 0, 1, 1,   -1, 0, 0,       1, 1,
                -1.0f,  1.0f, -1.0f,     0, 0, 1, 1,   -1, 0, 0,       1, 1,
                -1.0f, -1.0f, -1.0f,     0, 0, 1, 1,   -1, 0, 0,       0, 1,
                -1.0f, -1.0f,  1.0f,     0, 0, 1, 1,   -1, 0, 0,       0, 0,
#endif
            };
        std::vector<swrast::uint> indices = 
        {
            0, 1, 2,
            3, 4, 5,
            6, 7, 8,
            9, 10, 11,
            12, 13, 14,
            15, 16, 17,
            18, 19, 20,
            21, 22, 23,
            24, 25, 26,
            27, 28, 29,
            30, 31, 32,
            33, 34, 35 
        };
       memcpy((void*)vb, triangle.data(), (sizeof(float) * 12) * (9 + 36));
       memcpy((void*)ib, indices.data(), sizeof(swrast::uint) * 36);
    }

    simple_vertex_t* vs = new simple_vertex_t();
    vs->setup();
    simple_pixel_t* ps = new simple_pixel_t();
    ps->setup();
    ps->m_texture = tex;
    ps->light_color = swrast::float4_t(1, 1, 1, 1);
    ps->light_pos = swrast::float3_t(-1, -2, 2);

    swrast::float4x4_t rot = swrast::rotate<float>(swrast::identity<float>(), swrast::float3_t(1, 0, 0), swrast::deg_to_rad(-30.f));
    swrast::float4x4_t rot2 = swrast::rotate<float>(swrast::identity<float>(), swrast::float3_t(0, 1, 0), swrast::deg_to_rad(45.f));
    swrast::float4x4_t t = swrast::translate<float>(swrast::identity<float>(), swrast::float3_t(0, 0, 5.5));
    swrast::float4x4_t model = rot * rot2 * t;
    swrast::float4x4_t mm = model;
    mm[12] = 0;
    mm[13] = 0;
    mm[14] = 0;
    mm[15] = 1;
    swrast::float4x4_t N = swrast::transpose<float>(swrast::inverse<float>(mm)); // need to inverse-transpose the model matrix, to correct the normals.
    vs->mvp =  model * swrast::perspective_lh_aspect(swrast::deg_to_rad(45.0f), (float)screen_width/(float)screen_height, 0.001f, 1000.0f);
    vs->m = model;
    vs->n = N;

    swrast::viewport_t viewport = { };
    viewport.x = viewport.y = 0;
    viewport.width = screen_width;
    viewport.height = screen_height;
    viewport.near = 0.0000f;
    viewport.far = 1.0f;
    swrast::bind_render_targets(1, &rt, ds);
    float rgba[4] = { 0.f, 0.3f, 0.3f, 1.f };
    swrast::rect_t clear_rect = { };
    clear_rect.x = 0;
    clear_rect.y = 0;
    clear_rect.width = viewport.width;
    clear_rect.height = viewport.height;
    swrast::clear_render_target(0, clear_rect, rgba);
    swrast::clear_depth_stencil(0.f, clear_rect);
    swrast::set_front_face(swrast::front_face_counter_clockwise);
    swrast::set_depth_compare(swrast::compare_op_greater);
    swrast::set_cull_mode(swrast::cull_mode_front);
    swrast::set_input_layout(layout);
    swrast::set_viewports(1, &viewport);
    swrast::bind_vertex_shader(vs);
    swrast::bind_pixel_shader(ps);
    swrast::enable_depth(true);
    swrast::enable_depth_write(true);
    swrast::bind_vertex_buffers(1, &vb);
    swrast::bind_index_buffer(ib);
    // Draws the rectangle.
    //swrast::draw_instanced(36, 1, 9, 0);
    swrast::draw_indexed_instanced(36, 1, 0, 3, 0);
    // Rotate the other triangle.
    rot = swrast::rotate<float>(swrast::identity<float>(), swrast::float3_t(0, 0, 1), swrast::deg_to_rad(45.f));
    t = swrast::translate<float>(swrast::identity<float>(), swrast::float3_t(0, 0.2, 4.0));
    swrast::float4x4_t s = swrast::scale(swrast::identity<float>(), swrast::float3_t(0.5, 0.5, 0.5));
    vs->mvp = s * rot * t * swrast::perspective_lh_aspect(swrast::deg_to_rad(45.0f), (float)screen_width/(float)screen_height, 0.001f, 1000.0f);
    swrast::set_cull_mode(swrast::cull_mode_back);
    swrast::draw_instanced(3, 1, 0, 0);

    int err = stbi_write_png("img.png", viewport.width, viewport.height, 4, (void*)rt, viewport.width * 4);

    err = stbi_write_png("depth.png", viewport.width, viewport.height, 4, (void*)ds, viewport.width * 4);
    swrast::release_resource(tex);
    swrast::release_resource(ds);
    swrast::release_resource(vb);
    swrast::release_resource(rt);
    swrast::destroy();
    return 0;
}