// 
#include "Context.hpp"
#include "InputAssembly.hpp"
#include "Rasterizer.hpp"
#include "HardwareShader.hpp"
#include "Memory.hpp"
#include "Allocator.hpp"

namespace swrast {

hardware_shader_cache_t shader_cache;
input_assembler_t       assembler;
vertex_transformation_t vertex_transformation;
clipper_t               clipper;
rasterizer_t            rasterizer;
primitive_topology_t    bound_primitive_topology;
allocator_t*            resource_allocator;
front_face_t            winding_order;

error_t initialize()
{
    fbounds3d_t ndc = { };
    ndc.minima = float3_t(-1, -1, -1);
    ndc.maxima = float3_t(1, 1, 1);
    assembler.initialize();
    vertex_transformation.initialize();
    clipper.initialize();
    rasterizer.initialize(ndc);
    resource_allocator = new malloc_allocator_t();
    return result_ok;
}


error_t destroy()
{
    delete resource_allocator;
    assembler.release();
    vertex_transformation.release();
    clipper.release();
    rasterizer.release();
    return result_ok;
}


shader_t create_shader(shader_type_t type, void* src_code, uint32_t size_bytes)
{
    hardware_shader_t* shader_ptr = shader_cache.allocate_shader(type);
    if (shader_ptr)
    {
        switch (type)
        {
            case shader_type_vertex:
            {
                vertex_shader_t* vertex_shader = (vertex_shader_t*)shader_ptr;
                vertex_shader->setup();
            }
            case shader_type_pixel:
            {
            }
        }
        return shader_ptr->id;
    }
    return 0;
}


error_t draw_instanced(uint32_t num_vertices, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance)
{
    vertices_t vertex_pool = assembler.get_available_vertex_pool(UINT16_MAX * instance_count, 
        vertex_transformation.get_vertex_shader()->get_out_vertex_stride());
    // number of vertices called by draw call.
    vertex_pool.num_vertices = num_vertices;

    // transform our vertices with the provided vertex shader.
    vertex_transformation.transform(&vertex_pool, first_vertex, num_vertices);

    // The clipper clips any vertices that won't be in the clip/view volume.
    // If all vertices of the triangle are clipped, then that triangle is considered culled.
    clipper.clip_cull(&vertex_pool);
    
    // Primitive generator creates our triangles.
    // In this case, we can just reinterpret our vertices as triangles.
    uint32_t num_triangles = vertex_pool.num_vertices / 3;

    // finally rasterize onto framebuffer. Triangles are left in clip space,
    // so the rasterizer will convert them into ndc, for which they will then 
    // be projected into screen space.
    rasterizer.raster(num_triangles, vertex_pool, winding_order);
    return result_ok;
}


error_t bind_vertex_shader(vertex_shader_t* shader)
{
    // Find the vertex shader, and bind it to vertex transformer.
    return vertex_transformation.bind_vertex_shader(shader);
}


error_t bind_pixel_shader(pixel_shader_t* shader)
{
    // Find the pixel shader, and bind it to the rasterizer.
    return rasterizer.bind_pixel_shader(shader);
}


error_t set_primitive_topology(primitive_topology_t primitive_topology)
{
    switch (primitive_topology)
    {
        default:
            return result_failed;
    }
    bound_primitive_topology = primitive_topology;
    return result_ok;
}


resource_t allocate_resource(const resource_desc_t& desc)
{
    resource_t res = 0;
    size_t size_bytes = desc.width * desc.height * desc.depth_or_array_size * desc.mip_count * format_size_bytes(desc.format);
    res = (resource_t)resource_allocator->allocate(size_bytes, 1);
    // the memory surface should be initialized to all zeroes.
    memset((void*)res, 0, size_bytes);
    return res;
}


error_t release_resource(resource_t resource)
{
    resource_allocator->free((void*)resource);
    return result_ok;
}


error_t bind_render_targets(uint32_t num_rtvs, resource_t* rtvs, resource_t dsv)
{
    framebuffer_t framebuffer = { };
    for (uint32_t i = 0; i < num_rtvs; ++i)
    {
        framebuffer.bound_render_targets[i] = rtvs[i];
        framebuffer.max_width = 0;
        framebuffer.max_height = 0;
    }
    framebuffer.num_render_targets = num_rtvs;
    framebuffer.bound_depth_stencil = dsv;
    rasterizer.bind_frame_buffer(framebuffer);
    return result_ok;
}


error_t set_viewports(uint32_t count, viewport_t* viewports)
{
    return rasterizer.set_viewports(count, viewports);
}


error_t bind_vertex_buffers(uint32_t num_vbs, resource_t* vbs)
{
    return vertex_transformation.bind_vertex_buffers(num_vbs, vbs);
}


error_t set_front_face(front_face_t front_face)
{
    winding_order = front_face;
    return result_ok;
}


error_t enable_depth(bool enable)
{
    rasterizer.enable_depth(true);
    return result_ok;
}


error_t enable_depth_write(bool enable)
{
    rasterizer.enable_write_depth(enable);
    return result_ok;
}


error_t clear_render_target(uint32_t index, const rect_t& rect, float* rgba)
{
    float4_t clear_color = { rgba[0], rgba[1], rgba[2], rgba[3] };
    return rasterizer.clear_render_target(index, rect, clear_color);
}


input_layout_t create_input_layout(uint32_t num_elements, input_element_desc* descs)
{
    input_layout* layout = assembler.create_input_layout(num_elements, descs);
    return (input_layout_t)layout;
}


error_t set_input_layout(input_layout_t layout)
{
    input_layout* input = (input_layout*)layout;
    vertex_transformation.bind_input_layout(input);
    return result_ok;
}
} // 