//
#pragma once

#include "Context.hpp"
#include "Math.hpp"

#include <unordered_map>

namespace swrast {


struct hardware_shader_t 
{
    shader_t id;
};


//// Vertex shader implementation.
//class vertex_shader_t : public hardware_shader_t
//{
//public:
//
//    struct out_vert_t
//    {
//        float4_t pos;
//        float4_t color;
//    };
//
//    error_t setup()
//    {
//        this->out_vertex_stride_bytes = sizeof(out_vert_t);
//        this->out_pos_offset_bytes = 0;
//        return result_ok;
//    }
//
//    // Must output a vertex, in clip space.
//    void execute(uintptr_t in_vertex_ptr, uintptr_t out_vertex, uint32_t vert_id) 
//    { 
//        float4_t output = *(float4_t*)in_vertex_ptr;
//        out_vert_t* out = (out_vert_t*)out_vertex;
//        out->pos = output;
//        out->color = float4_t(1, 1, 1, 1);
//    }
//
//    uint32_t get_out_vertex_stride() const { return out_vertex_stride_bytes; }
//    uint32_t get_out_pos_offset_bytes() const { return out_pos_offset_bytes; }
//    
//protected:
//    uint32_t out_vertex_stride_bytes;
//    uint32_t out_pos_offset_bytes;
//};
//
//
//// Pixel shader implementation.
//class pixel_shader_t : public hardware_shader_t
//{
//public:
//
//    void setup()
//    {
//    }
//    // Should output the color. Ideally we want to pass in the 
//    // screen space coordinates, which might be used for other processes.
//    // We will also want to pass any vertex attributes that might need to be 
//    // used for texturing as well.
//    float4_t execute(float w0, float w1, float w2) 
//    {
//        float3_t c0 = {1, 0, 0};
//        float3_t c1 = {0, 1, 0};
//        float3_t c2 = {0, 0, 1};
//        float r = w0 * c0[0] + w1 * c1[0] + w2 * c2[0];
//        float g = w0 * c0[1] + w1 * c1[1] + w2 * c2[1];
//        float b = w0 * c0[2] + w1 * c1[2] + w2 * c2[2];
//        return float4_t(r, g, b, 1); 
//    }
//};



class hardware_shader_cache_t
{
public:
    hardware_shader_t*  allocate_shader(shader_type_t type);
    hardware_shader_t*  get_hardware_shader(shader_t shader_id);
    error_t             destroy_shader(shader_t shader_id);
private:
    std::unordered_map<shader_t, hardware_shader_t*> shader_cache;
};
} // swrast