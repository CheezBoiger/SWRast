//
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
typedef uint32_t uint;
typedef uint32_t view_t;
typedef uint64_t input_layout_t;

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
    format_r32g32b32_float,
    format_r32g32_float,
};


enum compare_op_t
{
    compare_op_none,
    compare_op_equal,
    compare_op_less,
    compare_op_less_equal,
    compare_op_greater,
    compare_op_greater_equal
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
    float near;
    float far;
};

struct rect_t
{
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
};

enum view_type_t
{
    view_type_render_target,
    view_type_shader_resource,
    view_type_unordered_access
};


struct view_desc_t
{
    view_type_t view_type;
    resource_t  resource;
    format_t    format;
};


enum front_face_t
{
    front_face_counter_clockwise,
    front_face_clockwise
};


enum cull_mode_t
{
    cull_mode_none,
    cull_mode_front,
    cull_mode_back,
    cull_mode_front_and_back
};


struct input_element_desc
{
    format_t format;
    uint32_t input_slot;
    uint32_t offset;
};


SW_EXPORT_DLL size_t format_size_bytes(format_t format);
} // swrast