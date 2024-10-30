//
#include "HardwareShader.hpp"
#include "Shader.hpp"
#include <cstdlib>

namespace swrast {


hardware_shader_t* hardware_shader_cache_t::allocate_shader(shader_type_t shader_type)
{
    static shader_t gen = 0;
    hardware_shader_t* shader_ptr = nullptr;
    switch (shader_type)
    {
        //case shader_type_vertex:
        //{
        //    shader_ptr = new vertex_shader_t();
        //    break;
        //}
        //case shader_type_pixel:
        //{
        //    shader_ptr = new pixel_shader_t();
        //    break;
        //}
    }
    // Store the shader into the cache.
    if (shader_ptr)
    {
        shader_t id = gen++;
        shader_ptr->id = id;
        shader_cache[shader_ptr->id] = shader_ptr;
        return shader_ptr;
    }
    // If no shader was created, or the creation failed, pass invalid value.
    return 0;
}


hardware_shader_t* hardware_shader_cache_t::get_hardware_shader(shader_t shader_id)
{
    hardware_shader_t* shader_ptr = nullptr;
    auto iter = shader_cache.find(shader_id);
    if (iter != shader_cache.end())
    {
        shader_ptr = iter->second;
    }
    return shader_ptr;
}


error_t hardware_shader_cache_t::destroy_shader(shader_t shader_id)
{
    auto iter = shader_cache.find(shader_id);
    if (iter != shader_cache.end())
    {
        delete iter->second;
        shader_cache.erase(iter);
    }
    return result_ok;
}


float4_t rgba8_to_norm(uint32_t color)
{
    float4_t result;
    float inv = 1.f / 255.f;
    result.r = (float)((color & 0x000000ff))        * inv;
    result.g = (float)((color & 0x0000ff00) >> 8)   * inv;
    result.b = (float)((color & 0x00ff0000) >> 16)  * inv;
    result.a = (float)((color & 0xff000000) >> 24)  * inv;
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
        case format_r32_float:
        {
            float color = *(float*)texel;
            result[0] = color;
            result[1] = result[0];
            result[2] = result[0];
            result[3] = result[0];
            break;
        }
        default:
            break;
    }
    return result;
}


static
float3_t denorm_coordinate(const float3_t& uvw, const float3_t& tex_size)
{
    float x = uvw[0] * tex_size[0];
    float y = uvw[1] * tex_size[1];
    float z = uvw[2] * tex_size[2];
    return float3_t(x, y, z);
}


uintptr_t texel(uintptr_t texture, const uint3_t& c_s, uint32_t format_size, uint32_t row_pitch, uint32_t depth)
{
    // 3d texel coordinate calculated as:
    // 3d[x, y, z] = 1d[ x + (y * width) + (z * width * height) ]
    // Should work with 1d and 2d coordinates, as long as y and/or z are 0.
    return texture + (c_s[0] * format_size) + (row_pitch * c_s[1]) + (c_s[2] * depth);
}


uintptr_t texel_cube(uintptr_t texture, const uint3_t& c_s, uint32_t format_size, uint32_t row_pitch, uint32_t depth)
{
    // This function should fetch the texel address for a texture cube.
    //
    return 0;
}


static
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


static
float4_t sample(uintptr_t texture, const sampler_desc_t& sampler, const uint3_t& c_s, const uint3_t& tex_size, const resource_desc_t& resource_desc)
{
    const uint32_t format_size = format_size_bytes(resource_desc.format);
    const uint32_t row_pitch = tex_size[0] * format_size;
    const uint32_t depth = tex_size[1] * row_pitch;
        
    const uint x = filter_texel_coord(c_s[0], tex_size[0], sampler.address_u);
    const uint y = filter_texel_coord(c_s[1], tex_size[1], sampler.address_v);
    const uint z = filter_texel_coord(c_s[2], tex_size[2], sampler.address_w);

    return (resource_desc.type == resource_type_texturecube)
        ? texel_to_color(texel_cube(texture, uint3_t(x, y, z), format_size, row_pitch, depth), resource_desc.format)
        : texel_to_color(texel(texture, uint3_t(x, y, z), format_size, row_pitch, depth), resource_desc.format);
}


float4_t pixel_shader_t::texture(uintptr_t texture_handle, const sampler_desc_t& sampler, const float3_t& tex_coord)
{
    float4_t texel_color = float4_t();
    if (texture_handle != 0)
    {
        resource_desc_t* desc = (resource_desc_t*)(texture_handle - sizeof(resource_desc_t));
        const float3_t tex_size = float3_t(desc->width, desc->height, desc->depth_or_array_size);
        float2_t denorm = denorm_coordinate(tex_coord, tex_size);

        switch (sampler.filter)
        {
            case sampler_filter_linear:
            {
                float2_t half_pixel = float2_t(0.5f, 0.5f);
                float2_t v_cell = floor(denorm - half_pixel);
                float2_t offset = denorm - half_pixel - v_cell;
                float4_t c_tl = sample(texture_handle, sampler, uint2_t(v_cell) + uint2_t(0, 0), tex_size, *desc);
                float4_t c_tr = sample(texture_handle, sampler, uint2_t(v_cell) + uint2_t(1, 0), tex_size, *desc);
                float4_t c_bl = sample(texture_handle, sampler, uint2_t(v_cell) + uint2_t(0, 1), tex_size, *desc);
                float4_t c_br = sample(texture_handle, sampler, uint2_t(v_cell) + uint2_t(1, 1), tex_size, *desc);
            
                float4_t c_tx = c_tr * offset[0] + c_tl * (1 - offset[0]);
                float4_t c_bx = c_br * offset[0] + c_bl * (1 - offset[0]);
                texel_color = c_bx * offset[1] + c_tx * (1 - offset[1]);
                break;
            }
            case sampler_filter_point:
            default:
            {
                texel_color = sample(texture_handle, sampler, uint2_t(denorm), tex_size, *desc);
                break;
            }
        }
    }
    return texel_color;
}


float4_t pixel_shader_t::textureFetch(uintptr_t texture_handle, const uint3_t& coord)
{
    float4_t texel_color = float4_t();
    if (texture_handle != 0)
    {
        resource_desc_t* desc = (resource_desc_t*)(texture_handle - sizeof(resource_desc_t));
        const uint3_t tex_size = uint3_t(desc->width, desc->height, desc->depth_or_array_size);
        const uint32_t format_size = format_size_bytes(desc->format);
        const uint32_t row_pitch = tex_size[0] * format_size;
        const uint32_t depth = tex_size[1] * row_pitch;
        texel_color = texel_to_color(texel(texture_handle, coord, format_size, row_pitch, depth), desc->format);
    }
    return texel_color;
}


uint3_t pixel_shader_t::texture_size(uintptr_t texture_handle)
{
    if (texture_handle != 0)
    {
        resource_desc_t* desc = (resource_desc_t*)(texture_handle - sizeof(resource_desc_t));
        const uint3_t tex_size = uint3_t(desc->width, desc->height, desc->depth_or_array_size);
        return tex_size;
    }
    return uint3_t(0, 0, 0);
}


float3_t pixel_shader_t::reflect(float3_t incidence, float3_t normal)
{
    // Reflection taken from GLSL references:
    // https://docs.gl/sl4/reflect
    return incidence - 2.0f * dot(normal, incidence) * normal;
}


float3_t pixel_shader_t::refract(float3_t incidence, float3_t normal, float eta)
{
    // Refraction algorithm taken from GLSL references:
    // https://docs.gl/sl4/refract
    float NoI = dot(normal, incidence);
    float k = 1.0f - eta * eta * (1.0f - NoI * NoI);
    return (k < 0.0)
        ? 0.0f
        : eta * incidence - (eta * NoI + sqrtf(k)) * normal;
}


void pixel_shader_t::varying_attribute(uintptr_t offset, data_type type, interpolation interp)
{
        varying_metadata.push_back({ offset, type, interp });
}


// Interpolate the given data with perspective correction.
#define INTERPOLATE_PERSP(type, output, v0, v1, v2, barycentrics)  { \
        type* out = (type*)output; \
        type* d0 = (type*)v0; \
        type* d1 = (type*)v1; \
        type* d2 = (type*)v2; \
        *out = *d0 * barycentrics[0] + *d1 * barycentrics[1] + *d2 * barycentrics[2]; \
    }


void pixel_shader_t::interpolate_varying(uintptr_t varying, uintptr_t v0, uintptr_t v1, uintptr_t v2, const float3_t& barycentrics)
{

    for (uint it = 0; it < varying_metadata.size(); ++it)
    {
        varying_info& info = varying_metadata[it];
        uintptr_t varying_data = varying + info.offset;
        uintptr_t v0_d = v0 + info.offset;
        uintptr_t v1_d = v1 + info.offset;
        uintptr_t v2_d = v2 + info.offset;
        switch (info.type)
        {
            case data_type_float:
                INTERPOLATE_PERSP(float, varying_data, v0_d, v1_d, v2_d, barycentrics);
                break;
            case data_type_float2:
                INTERPOLATE_PERSP(float2_t, varying_data, v0_d, v1_d, v2_d, barycentrics);
                break;
            case data_type_float3:
                INTERPOLATE_PERSP(float3_t, varying_data, v0_d, v1_d, v2_d, barycentrics);
                break;
            case data_type_float4:
            default:
                INTERPOLATE_PERSP(float4_t, varying_data, v0_d, v1_d, v2_d, barycentrics);
                break;
        }
    }
}


void store_color(uintptr_t texel, const float4_t& color, format_t format)
{
    switch (format)
    {
        case format_r8g8b8a8_unorm:
        {
            uint32_t r = (uint32_t)(clamp(color.r, 0.0f, 1.0f) * 255.f);
            uint32_t g = (uint32_t)(clamp(color.g, 0.0f, 1.0f) * 255.f);
            uint32_t b = (uint32_t)(clamp(color.b, 0.0f, 1.0f) * 255.f);
            uint32_t a = (uint32_t)(clamp(color.a, 0.0f, 1.0f) * 255.f);
            uint32_t rgba = (r) | (g << 8) | (b << 16) | (a << 24);
            uint32_t* output = (uint32_t*)texel;
            *output = rgba;
            break;
        }
        case format_r32_float:
        {
            float* output = (float*)texel;
            *output = color.r;
            break;
        }
    }
}


float4_t load_color(uintptr_t texel, format_t format)
{
    float4_t color;
    switch (format)
    {
        case format_r32_float:
        {
            color[0] = *((float*)texel);
            color[1] = color[0];
            color[2] = color[0];
            color[3] = color[0];
            break;
        }
        case format_r8g8b8a8_unorm:
        {
            uint32_t rgba = *((uint32_t*)texel);
            float c_inv = 1.f / 255.f;
            color[0] = (float)(rgba &  0x000000ff)          * c_inv;
            color[1] = (float)((rgba & 0x0000ff00) >> 8)    * c_inv;
            color[2] = (float)((rgba & 0x00ff0000) >> 16)   * c_inv;
            color[3] = (float)((rgba & 0xff000000) >> 24)   * c_inv;
            break;
        }
        case format_r32g32b32a32_float:
        {
            color = *((float4_t*)texel);
            break;
        }
    }
    return color;
}
} // swrast