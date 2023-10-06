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


// Vertex shader implementation.
class vertex_shader_t : public hardware_shader_t
{
public:
    // Must output a vertex, in clip space.
    float4_t execute(float4_t* in_vert, uint32_t vert_id) 
    { 
        float4_t output = *in_vert;
        return  output;
    }
};


// Pixel shader implementation.
class pixel_shader_t : public hardware_shader_t
{
public:
    // Should output the color. Ideally we want to pass in the 
    // screen space coordinates, which might be used for other processes.
    // We will also want to pass any vertex attributes that might need to be 
    // used for texturing as well.
    float4_t execute(float w0, float w1, float w2) 
    {
        float3_t c0 = {1, 0, 0};
        float3_t c1 = {0, 1, 0};
        float3_t c2 = {0, 0, 1};
        float r = w0 * c0[0] + w1 * c1[0] + w2 * c2[0];
        float g = w0 * c0[1] + w1 * c1[1] + w2 * c2[1];
        float b = w0 * c0[2] + w1 * c1[2] + w2 * c2[2];
        return float4_t(r, g, b, 1); 
    }
};



class hardware_shader_cache_t
{
public:
    shader_t            allocate_shader(shader_type_t type);
    hardware_shader_t*  get_hardware_shader(shader_t shader_id);
    error_t             destroy_shader(shader_t shader_id);
private:
    std::unordered_map<shader_t, hardware_shader_t*> shader_cache;
};
} // swrast