//
#pragma once
#include "Context.hpp"
#include "Math.hpp"
#include "HardwareShader.hpp"
#include "Memory.hpp"
#include <map>

namespace swrast {


extern size_t format_size_bytes(format_t format);

struct input_buffer_desc
{
    uint32_t stride_bytes;
};

struct input_layout
{
    uint32_t num_vbs;
    input_buffer_desc input_slots[16];
};

// Vertices for pipeline.
struct vertices_t
{
    // actual buffer of vertices. We are currently supporting positions.
    uintptr_t   vertices_base;
    // current number of vertices generated.
    uint32_t    num_vertices;
    // Maximum allowed vertices allocated.
    uint32_t    max_vertices;
    // The stride bytes of each vertex. Must be consistent!
    uint32_t    vertex_stride;
    // Offset position.
    uint32_t    pos_offset;
    // Allocates a new vertex in the vertices cache. 
    // Returns index of vertex if available, or -1 if the vertex cache is full!
    // 
    uint32_t  allocate_vertex();
    uintptr_t get_vertex(uint32_t index);
    float4_t& get_vertex_position(uint32_t index);
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
    vertices_t get_available_vertex_pool(uint32_t num_vertices_limit, uint32_t vertex_size_bytes);

    input_layout* create_input_layout(uint32_t num_descs, input_element_desc* descs);
private:
    memory_pool_t                       vertex_pool;
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
    error_t bind_input_layout(input_layout* layout) { input_layout = layout; return result_ok; }

    // Calls to generate and transform our object space vertices into clip space.
    // The input vertex buffers must be in object space, and the output transformation
    // must be in clip space.
    error_t transform(vertices_t* in_vertices, uint32_t first_vertex, uint32_t num_vertices);

    // Bind the vertex shader to be used for transformation.
    error_t bind_vertex_shader(vertex_shader_t* shader) { vertex_shader = shader; return result_ok; }
    
    vertex_shader_t* get_vertex_shader() { return vertex_shader; }

private:
    input_layout* input_layout;
    resource_t vertex_buffers[16];
    uint32_t num_vertex_buffers;
    vertex_shader_t* vertex_shader;
};
} // swrast