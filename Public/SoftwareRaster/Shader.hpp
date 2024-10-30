//
#pragma once

#include "Core.hpp"
#include "Math.hpp"

#include <vector>

namespace swrast {


typedef uint32_t shader_t;

struct i_shader_t
{
    shader_t id;
};


// Vertex shader implementation.
class SW_EXPORT_DLL vertex_shader_t : public i_shader_t
{
public:
    virtual ~vertex_shader_t() { }

    virtual void setup() = 0;

    // Execution handle for the shader.
    // REQUIRED: output a vertex, with at least one position in clip space.
    virtual void execute(uintptr_t in_vertex_ptr, uintptr_t out_vertex, uint32_t vert_id) = 0;

    uint32_t get_out_vertex_stride() const { return out_vertex_stride_bytes; }
    uint32_t get_out_pos_offset_bytes() const { return out_pos_offset_bytes; }
    
protected:
    // Sets the output vertex information. Should provide the stride in bytes of the output vertex attribs,
    // and the out_pos_offset_bytes should provide the offset of the position that needs to be present in the vertex.
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
class SW_EXPORT_DLL pixel_shader_t : public i_shader_t
{
public:

    enum data_type
    {
        data_type_float,
        data_type_float2,
        data_type_float3,
        data_type_float4,
    };

    enum interpolation
    {
        interpolation_none,
        interpolation_linear,
        interpolation_perspective
    };

    virtual ~pixel_shader_t() { }

    virtual void setup() = 0;

    // Should output the color. Ideally we want to pass in the 
    // screen space coordinates, which might be used for other processes.
    // We will also want to pass any vertex attributes that might need to be 
    // used for texturing as well.
    virtual float4_t execute(uintptr_t varying_address) = 0;

    uint32_t get_varying_stride_bytes() const { return in_varying_stride_bytes; }
    uint32_t get_position_offset_bytes() const { return in_pos_offset_bytes; }

    // Defines the varying attributes for each pixel.
    void varying_attribute(uintptr_t offset, data_type type, interpolation interp);

    // TODO: This function should more or less be transparent, the user should just be able to provide the metadata information of 
    // the given varying information, so they should NOT be the one's to define this. Figure out a good way to make this 
    // more convenient.
    void interpolate_varying(uintptr_t varying, uintptr_t v0, uintptr_t v1, uintptr_t v2, const float3_t& barycentrics);

protected:
    void set_varying_info(uint32_t in_stride_bytes, uint32_t in_pos_offset_bytes)
    {
        this->in_varying_stride_bytes = in_stride_bytes;
        this->in_pos_offset_bytes = in_pos_offset_bytes;
    }

    // Simple texel fetch.
    // accesses [0, size-1]
    float4_t textureFetch(uintptr_t texture_handle, const uint3_t& coord);

    // Sample a texture with a sampler.
    // If texture is 1d, 2d, 3d = float3 coord.
    float4_t texture(uintptr_t texture_handle, const sampler_desc_t& sampler, const float3_t& tex_coord);

    // Get texture size [width, height, depth]
    uint3_t texture_size(uintptr_t texture_handle);

    // Reflection function.
    float3_t reflect(float3_t incidence, float3_t normal);

    float3_t refract(float3_t incidence, float3_t normal, float eta);

    // Input varying stride, in bytes. This is the information of each varying package that is passed to each shader invocation.
    uint32_t in_varying_stride_bytes;

    // Input position offset, in bytes. This is the offset of the vertex output, that position is located in. 
    // The position should always be a float4_t type, that is in raster space (screen space.)
    uint32_t in_pos_offset_bytes;

    // Varying information that is used to determine what and how-to interpolate data.
    struct varying_info
    {
        uintptr_t       offset;
        data_type       type;
        interpolation   interp;
    };

    std::vector<varying_info> varying_metadata;
};

} // swrast