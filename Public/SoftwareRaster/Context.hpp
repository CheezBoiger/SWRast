// Software Rasterizer
#pragma once

#include <cstdint>


#if defined(_WIN64)
#define SW_EXPORT_DLL __declspec(dllexport)
#define SW_IMPORT_DLL __declspec(dllimport)
#else
#define SW_EXPORT_DLL
#define SW_IMPORT_DLL
#endif

namespace swrast {

typedef uint32_t error_t;
typedef uint64_t resource_t;
typedef uint32_t sampler_t;
typedef uint32_t shader_t;
typedef uint32_t uint;

enum error_result_t
{
    result_ok,
    result_failed = -999,
};

enum resource_type_t
{
    resource_type_buffer,
    resource_type_texture1d,
    resource_type_texture2d,
    resource_type_texture3d,
    resource_type_texture1darray,
    resource_type_texture2darray,
    resource_type_texturecube
};


enum shader_type_t
{
    shader_type_vertex,
    shader_type_pixel
};


enum resource_usage_t
{
    usage_none = (0),
    usage_vertex_buffer = (1 << 0),
    usage_index_buffer = (1 << 1),
    usage_constant_buffer = (1 << 2),
    usage_shader_resource = (1 << 3),
    usage_unordered_access = (1 << 4),
    usage_render_target = (1 << 5),
    usage_depth_stencil = (1 << 6)
};


enum primitive_topology_t
{
    primitive_topology_trianglelist,
    primitive_topology_points,
    primitive_topology_lines,
};


enum format_t
{
    format_unknown,
    format_r8_unorm,
    format_r32_float,
    format_r8g8b8a8_unorm,
    format_r16g16_float,
    format_r32g32b32a32_float,
    format_r16g16b16a16_float,
    format_r11g11b10_float,
    format_r32g32b32_float
};

struct resource_desc_t
{
    resource_type_t     type;
    format_t            format;
    uint32_t            width;
    uint32_t            height;
    uint32_t            depth_or_array_size;
    uint32_t            mip_count;
    resource_usage_t    usage;
};


struct viewport_t
{
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
};


enum front_face_t
{
    front_face_clockwise,
    front_face_counter_clockwise
};

SW_EXPORT_DLL error_t       initialize();
SW_EXPORT_DLL error_t       destroy();

SW_EXPORT_DLL resource_t    allocate_resource(const resource_desc_t& desc);
SW_EXPORT_DLL error_t       release_resource(resource_t resource);

SW_EXPORT_DLL error_t       set_viewports(uint32_t count, viewport_t* viewports);

SW_EXPORT_DLL sampler_t     create_sampler();
SW_EXPORT_DLL error_t       destroy_sampler(sampler_t sampler);

SW_EXPORT_DLL error_t       bind_render_targets(uint32_t num_rtvs, resource_t* rtvs);
SW_EXPORT_DLL error_t       bind_depth_stencil(resource_t ds);

SW_EXPORT_DLL error_t       clear_render_target(uint32_t slot, float* rgba);
SW_EXPORT_DLL error_t       clear_depth_stencil(float* bounds);

SW_EXPORT_DLL error_t       bind_vertex_buffers(uint32_t num_vbs, resource_t* vbs);
SW_EXPORT_DLL error_t       bind_index_buffer(resource_t ib);

SW_EXPORT_DLL error_t       draw_instanced(uint32_t num_vertices, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance);

SW_EXPORT_DLL shader_t      create_shader(shader_type_t type, void* src_code, uint32_t size_bytes);
SW_EXPORT_DLL error_t       destroy_shader(shader_t shader);

SW_EXPORT_DLL error_t       bind_shader_resource(uint32_t slot, resource_t resource);
SW_EXPORT_DLL error_t       bind_const_buffer(uint32_t slot, resource_t resource);

SW_EXPORT_DLL error_t       bind_vertex_shader(shader_t vs);
SW_EXPORT_DLL error_t       bind_pixel_shader(shader_t ps);

SW_EXPORT_DLL error_t       map_resource(void** ptr, resource_t resource);
SW_EXPORT_DLL error_t       unmap_resource(resource_t resource);

SW_EXPORT_DLL error_t       enable_depth(bool enable);
SW_EXPORT_DLL error_t       enable_depth_write(bool enable);

SW_EXPORT_DLL error_t       set_primitive_topology(primitive_topology_t primitive_topology);
SW_EXPORT_DLL error_t       set_front_face(front_face_t front_face);
} // SWRast