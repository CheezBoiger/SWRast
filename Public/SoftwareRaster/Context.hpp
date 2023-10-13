// Software Rasterizer
#pragma once

#include "Core.hpp"
#include "Shader.hpp"

namespace swrast {

SW_EXPORT_DLL error_t       initialize();
SW_EXPORT_DLL error_t       destroy();

SW_EXPORT_DLL resource_t    allocate_resource(const resource_desc_t& desc);
SW_EXPORT_DLL error_t       release_resource(resource_t resource);

SW_EXPORT_DLL error_t       set_viewports(uint32_t count, viewport_t* viewports);

SW_EXPORT_DLL sampler_t     create_sampler(const sampler_desc_t& desc);
SW_EXPORT_DLL error_t       destroy_sampler(sampler_t sampler);

SW_EXPORT_DLL error_t       bind_render_targets(uint32_t num_rtvs, resource_t* rtvs, resource_t dsv);
SW_EXPORT_DLL error_t       bind_depth_stencil(resource_t ds);

SW_EXPORT_DLL error_t       clear_render_target(uint32_t slot, const rect_t& rect, float* rgba);
SW_EXPORT_DLL error_t       clear_depth_stencil(float depth, const rect_t& rect);

SW_EXPORT_DLL error_t       bind_vertex_buffers(uint32_t num_vbs, resource_t* vbs);
SW_EXPORT_DLL error_t       bind_index_buffer(resource_t ib);

SW_EXPORT_DLL error_t       draw_instanced(uint32_t num_vertices, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance);
SW_EXPORT_DLL error_t       draw_indexed_instanced(uint32_t num_indices, uint32_t num_instances, uint32_t first_index, uint32_t vertex_offset, uint32_t first_instance);

SW_EXPORT_DLL shader_t      create_shader(shader_type_t type, void* src_code, uint32_t size_bytes);
SW_EXPORT_DLL error_t       destroy_shader(shader_t shader);

SW_EXPORT_DLL error_t       bind_shader_resource(uint32_t slot, resource_t resource);
SW_EXPORT_DLL error_t       bind_const_buffer(uint32_t slot, resource_t resource);

SW_EXPORT_DLL error_t       bind_vertex_shader(vertex_shader_t* vs);
SW_EXPORT_DLL error_t       bind_pixel_shader(pixel_shader_t* ps);

SW_EXPORT_DLL error_t       map_resource(void** ptr, resource_t resource);
SW_EXPORT_DLL error_t       unmap_resource(resource_t resource);

SW_EXPORT_DLL error_t       enable_depth(bool enable);
SW_EXPORT_DLL error_t       enable_depth_write(bool enable);

SW_EXPORT_DLL error_t       set_cull_mode(cull_mode_t cull_mode);

SW_EXPORT_DLL error_t       set_primitive_topology(primitive_topology_t primitive_topology);
SW_EXPORT_DLL error_t       set_front_face(front_face_t front_face);
SW_EXPORT_DLL error_t       set_depth_compare(compare_op_t compare_op);

SW_EXPORT_DLL input_layout_t create_input_layout(uint32_t num_elements, input_element_desc* descs);
SW_EXPORT_DLL error_t        set_input_layout(input_layout_t layout);
} // SWRast