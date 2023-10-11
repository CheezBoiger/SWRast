//
#pragma once

#include "Core.hpp"
#include "Math.hpp"

namespace swrast {


typedef uint32_t shader_t;

struct i_shader_t
{
    shader_t id;
};


// Vertex shader implementation.
class vertex_shader_t : public i_shader_t
{
public:
    virtual ~vertex_shader_t() { }

    virtual void setup() = 0;

    // Must output a vertex, in clip space.
    virtual void execute(uintptr_t in_vertex_ptr, uintptr_t out_vertex, uint32_t vert_id) = 0;

    uint32_t get_out_vertex_stride() const { return out_vertex_stride_bytes; }
    uint32_t get_out_pos_offset_bytes() const { return out_pos_offset_bytes; }
    
protected:
    void set_out_vert_info(uint32_t out_vertex_stride_bytes, uint32_t out_pos_offset_bytes)
    {
        this->out_vertex_stride_bytes = out_vertex_stride_bytes;
        this->out_pos_offset_bytes = out_pos_offset_bytes;
    }

    void set_in_vert_info(uint32_t in_vertex_stride_bytes)
    {
        this->in_vertex_stride_bytes = in_vertex_stride_bytes;
    }

    uint32_t out_vertex_stride_bytes;
    uint32_t out_pos_offset_bytes;
    
    uint32_t in_vertex_stride_bytes;
};


// Pixel shader implementation.
class pixel_shader_t : public i_shader_t
{
public:
    virtual ~pixel_shader_t() { }

    virtual void setup() = 0;

    virtual void interpolate_varying(uintptr_t v0, uintptr_t v1, uintptr_t v2, const float3_t& barycentrics) = 0;
    // Should output the color. Ideally we want to pass in the 
    // screen space coordinates, which might be used for other processes.
    // We will also want to pass any vertex attributes that might need to be 
    // used for texturing as well.
    virtual float4_t execute() = 0;

protected:
    void set_varying_info(uint32_t in_stride_bytes, uint32_t in_pos_offset_bytes)
    {
        this->in_varying_stride_bytes = in_stride_bytes;
        this->in_pos_offset_bytes = in_pos_offset_bytes;
    }

    //
    float4_t textureFetch(uintptr_t texture, const float2_t& tex_coord)
    {
        float4_t texel_color = float4_t();
        if (texture != 0)
        {
            resource_desc_t* desc = (resource_desc_t*)(texture - sizeof(resource_desc_t));
            const uint32_t width = desc->width;
            const uint32_t height = desc->height;
            const uint32_t format_size = format_size_bytes(desc->format);
            const uint32_t row_pitch = width * format_size;
            uint32_t x = (uint32_t)(tex_coord.s * (float)width);
            uint32_t y = (uint32_t)(tex_coord.t * (float)height);
            uintptr_t texel_address = texture + (x * format_size) + (row_pitch * y);
            switch (desc->format)
            {
                case format_r8g8b8a8_unorm:
                {
                    uint32_t color = *(uint32_t*)texel_address;
                    float inv = 1.f / 255.f;
                    texel_color.r = (float)((color & 0x000000ff)) * inv;
                    texel_color.g = (float)((color & 0x0000ff00) >> 8) * inv;
                    texel_color.b = (float)((color & 0x00ff0000) >> 16) * inv;
                    texel_color.a = (float)((color & 0xff000000) >> 24) * inv;
                    break;
                }
                default:
                    break;
            }
        }
        return texel_color;
    }

    uint32_t in_varying_stride_bytes;
    uint32_t in_pos_offset_bytes;
};

} // swrast