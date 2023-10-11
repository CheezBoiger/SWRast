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


private:
    float4_t rgba8_to_norm(uint32_t color)
    {
        float4_t result;
        float inv = 1.f / 255.f;
        result.r = (float)((color & 0x000000ff)) * inv;
        result.g = (float)((color & 0x0000ff00) >> 8) * inv;
        result.b = (float)((color & 0x00ff0000) >> 16) * inv;
        result.a = (float)((color & 0xff000000) >> 24) * inv;
        return result;
    }
    
    float4_t texel_to_color(uintptr_t texel, format_t format)
    {
        float4_t result;
        switch (format)
        {
            case format_r8g8b8a8_unorm:
            {
                uint32_t color = *(uint32_t*)texel;
                result = rgba8_to_norm(color);
                break;
            }
            default:
                break;
        }
        return result;
    }

    float2_t denorm_coordinate(const float2_t& uv, const float2_t& tex_size)
    {
        float x = uv[0] * tex_size[0];
        float y = uv[1] * tex_size[1];
        return float2_t(x, y);
    }

    uintptr_t texel(uintptr_t texture, const uint2_t& c_s, uint32_t format_size, uint32_t row_pitch)
    {
        return texture + (c_s[0] * format_size) + (row_pitch * c_s[1]);
    }

    uint filter_texel_coord(uint coord, uint size, texture_address_mode_t address_mode)
    {
        switch (address_mode)
        {
            case texture_address_mode_clamp:
            {
                coord = clamp(coord, (uint)0, size - 1);
                break;
            }
            case texture_address_mode_wrap:
            {
                coord = (uint)fabs((float)(coord % size));
                break;
            }
            case texture_address_mode_mirror:
            {
                coord = (coord < 0) ? (size - 1) - (uint)fabs(coord) % size : (uint)fabs(coord) % size;
                break;
            }
        }
        return coord;
    }

    float4_t sample(uintptr_t texture, const sampler_desc_t& sampler, const uint2_t& c_s, const uint2_t& tex_size, format_t format)
    {
        const uint32_t format_size = format_size_bytes(format);
        const uint32_t row_pitch = tex_size[0] * format_size;
        
        uint x = filter_texel_coord(c_s[0], tex_size[0], sampler.address_u);
        uint y = filter_texel_coord(c_s[1], tex_size[1], sampler.address_v);

        return texel_to_color(texel(texture, uint2_t(x, y), format_size, row_pitch), format);
    }

protected:
    void set_varying_info(uint32_t in_stride_bytes, uint32_t in_pos_offset_bytes)
    {
        this->in_varying_stride_bytes = in_stride_bytes;
        this->in_pos_offset_bytes = in_pos_offset_bytes;
    }

    // Simple texel fetch.
    float4_t textureFetch(uintptr_t texture, const float2_t& tex_coord)
    {
        float4_t texel_color = float4_t();
        if (texture != 0)
        {
            resource_desc_t* desc = (resource_desc_t*)(texture - sizeof(resource_desc_t));
            const float2_t tex_size = float2_t(desc->width, desc->height);
            const uint32_t format_size = format_size_bytes(desc->format);
            const uint32_t row_pitch = tex_size[0] * format_size;
            float2_t denorm = denorm_coordinate(tex_coord, tex_size);
            texel_color = texel_to_color(texel(texture, uint2_t(denorm), format_size, row_pitch), desc->format);
        }
        return texel_color;
    }

    // Sample a texture with a sampler.
    float4_t texture2d(uintptr_t texture, const sampler_desc_t& sampler, const float2_t& tex_coord)
    {
        float4_t texel_color = float4_t();
        if (texture != 0)
        {
            resource_desc_t* desc = (resource_desc_t*)(texture - sizeof(resource_desc_t));
            const float2_t tex_size = float2_t(desc->width, desc->height);
            float2_t denorm = denorm_coordinate(tex_coord, tex_size);

            switch (sampler.filter)
            {
                case sampler_filter_linear:
                {
                    float2_t half_pixel = float2_t(0.5f, 0.5f);
                    float2_t v_cell = floor(denorm - half_pixel);
                    float2_t offset = denorm - half_pixel - v_cell;
                    float4_t c_tl = sample(texture, sampler, uint2_t(v_cell) + uint2_t(0, 0), tex_size, desc->format);
                    float4_t c_tr = sample(texture, sampler, uint2_t(v_cell) + uint2_t(1, 0), tex_size, desc->format);
                    float4_t c_bl = sample(texture, sampler, uint2_t(v_cell) + uint2_t(0, 1), tex_size, desc->format);
                    float4_t c_br = sample(texture, sampler, uint2_t(v_cell) + uint2_t(1, 1), tex_size, desc->format);
            
                    float4_t c_tx = c_tr * offset[0] + c_tl * (1 - offset[0]);
                    float4_t c_bx = c_br * offset[0] + c_bl * (1 - offset[0]);
                    texel_color = c_bx * offset[1] + c_tx * (1 - offset[1]);
                    break;
                }
                case sampler_filter_point:
                default:
                {
                    texel_color = sample(texture, sampler, uint2_t(denorm), tex_size, desc->format);
                    break;
                }
            }
        }
        return texel_color;
    }

    uint32_t in_varying_stride_bytes;
    uint32_t in_pos_offset_bytes;
};

} // swrast