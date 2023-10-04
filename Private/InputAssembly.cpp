//
#include "InputAssembly.hpp"
#include <memory>

namespace swrast {


float4_t* vertices_t::allocate_vertex()
{
    // Check if we have enough memory for new vertex.
    // Otherwise return null if we have run out!
    if (num_vertices < max_vertices)
    {
        float4_t* allocated_vertex = &vertices[max_vertices++];
        return allocated_vertex; 
    }
    return nullptr;
}


vertices_t input_assembler_t::get_available_vertex_pool(uint32_t num_vertices_limit)
{
    uint32_t limit_bytes = num_vertices_limit * sizeof(float4_t);
    if (limit_bytes > pool.get_memory_size_bytes())
    {
        pool.preallocate(limit_bytes);
    }
    vertices_t vertices = { };
    vertices.vertices = (float4_t*)pool.get_base_address();
    vertices.num_vertices = 0;
    vertices.max_vertices = pool.get_memory_size_bytes() / sizeof(float4_t);
    return vertices;
}


error_t input_assembler_t::release()
{
    pool.release();
    return result_ok;
}


error_t vertex_transformation_t::transform(vertices_t* in_vertices, uint32_t first_vertex, uint32_t num_vertices)
{
    // Get the vertex buffer bases, for which we will begin reading from.
    float4_t* vb_base = (float4_t*)vertex_buffers[0];
    // for each drawing vertex, we will invoke the vertex shader. This might be able to be done in 
    // a certain batch, rather than individually!
    for (uint32_t vert_id = 0; vert_id < num_vertices; ++vert_id)
    {
        // execute the vertex shader.
        float4_t out = vertex_shader->execute(&vb_base[first_vertex + vert_id], vert_id);
        in_vertices->vertices[vert_id] = out;
    }
    return result_ok;
}


error_t vertex_transformation_t::bind_vertex_buffers(uint32_t num_vbs, resource_t* resource)
{
    num_vertex_buffers = num_vbs;
    for (uint32_t i = 0; i < num_vbs; ++i)
    {
        vertex_buffers[i] = resource[i];
    }
    return result_ok;
}
} // swrast