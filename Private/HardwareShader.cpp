//
#include "HardwareShader.hpp"
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
} // swrast