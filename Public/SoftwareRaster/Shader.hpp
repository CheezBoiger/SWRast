//
#pragma once

#include "Context.hpp"
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
    float4_t texture2d(uint64_t texture, const float2_t& tex_coord)
    {
        return float4_t();
    }

    uint32_t in_varying_stride_bytes;
    uint32_t in_pos_offset_bytes;
};

} // swrast