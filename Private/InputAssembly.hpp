//
#pragma once
#include "Context.hpp"
#include "Math.hpp"
#include "HardwareShader.hpp"
#include "Memory.hpp"


namespace swrast {


// Vertices for pipeline.
struct vertices_t
{
    // actual buffer of vertices. We are currently supporting positions.
    float4_t*   vertices;
    // current number of vertices generated.
    uint32_t    num_vertices;
    uint32_t    max_vertices;
    // Allocates a new vertex in the vertices cache. 
    // Returns failure if the vertex cache is full!
    float4_t*  allocate_vertex();
};

// Input assembler is initially supposed to generate the given output vertex buffer that will be used 
// throughout the pipeline. The generated vertices would also ideally need to support multiple attributes,
// like color, texcoord, normals.
class input_assembler_t
{
public:
    input_assembler_t()
    { }

    error_t initialize()
    {
        return result_ok;
    }

    error_t release();

    // Allocate a vertex pool for rendering. Will also contain 
    // additional memory for added triangles during clipping.
    vertices_t get_available_vertex_pool(uint32_t num_vertices_limit);
private:
    memory_pool_t pool;
};


// Vertex transformation handles transformation of vertices
// within the vertex allocation. This must reference the object space vertex buffer,
// as it will be used to generate the needed vertices to clip space.
class vertex_transformation_t
{
public:
    error_t initialize()
    {
        return result_ok;
    }

    //
    error_t release()
    {
        return result_ok;
    }

    // Bind our vertex buffers. Must be in object space.
    error_t bind_vertex_buffers(uint32_t num_vbs, resource_t* resource);

    // Calls to generate and transform our object space vertices into clip space.
    // The input vertex buffers must be in object space, and the output transformation
    // must be in clip space.
    error_t transform(vertices_t* in_vertices, uint32_t first_vertex, uint32_t num_vertices);

    // Bind the vertex shader to be used for transformation.
    error_t bind_vertex_shader(vertex_shader_t* shader) { vertex_shader = shader; return result_ok; }

private:
    resource_t vertex_buffers[16];
    uint32_t num_vertex_buffers;
    vertex_shader_t* vertex_shader;
};
} // swrast