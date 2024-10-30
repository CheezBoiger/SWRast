//
#include "InputAssembly.hpp"
#include <memory>

namespace swrast {


size_t format_size_bytes(format_t format)
{
    switch (format)
    {
        case format_r16g16_float:
        case format_r32_float:
        case format_r8g8b8a8_unorm:
        case format_r11g11b10_float:
            return 4ull;

        case format_r16g16b16a16_float:
        case format_r32g32_float:
            return 8ull;

        case format_r32g32b32a32_float:
            return 16ull;

        case format_r32g32b32_float:
            return 12ull;

        case format_r8_unorm:
            return 1ull;
    }
    return 1ull;
}



uint32_t vertices_t::allocate_vertex()
{
    // Check if we have enough memory for new vertex.
    // Otherwise return null if we have run out!
    if (num_vertices < max_vertices)
    {
        uint32_t index = max_vertices++;
        return index; 
    }
    return -1;
}


uintptr_t vertices_t::get_vertex(uint32_t index)
{
    return vertices_base + index * vertex_stride;
}


float4_t& vertices_t::get_vertex_position(uint32_t index)
{
    return *(float4_t*)(get_vertex(index) + pos_offset);
}


vertices_t input_assembler_t::get_available_vertex_pool(uint32_t num_vertices_limit, uint32_t vertex_size_bytes)
{
    uint32_t limit_bytes = num_vertices_limit * vertex_size_bytes;
    if (limit_bytes > vertex_pool.get_memory_size_bytes())
    {
        vertex_pool.preallocate(limit_bytes);
    }
    vertices_t vertices = { };
    vertices.vertices_base = vertex_pool.get_base_address();
    vertices.num_vertices = 0;
    vertices.max_vertices = vertex_pool.get_memory_size_bytes() / vertex_size_bytes;
    return vertices;
}


error_t input_assembler_t::release()
{
    vertex_pool.release();
    return result_ok;
}


error_t vertex_transformation_t::transform(vertices_t* in_vertices, uint32_t first_vertex, uint32_t num_vertices)
{
    // Set information about the vertex stream.
    in_vertices->pos_offset = vertex_shader->get_out_pos_offset_bytes();
    in_vertices->vertex_stride = vertex_shader->get_out_vertex_stride();
    // for each drawing vertex, we will invoke the vertex shader. This might be able to be done in 
    // a certain batch, rather than individually!
    // The number of vertex buffers bound will also determine the number of input slots to use for each of them.
    for (uint input_slot_index = 0; input_slot_index < input_layout->num_vbs; ++input_slot_index)
    {
        // Get the vertex buffer bases, for which we will begin reading from.
        uintptr_t vb_base = vertex_buffers[input_slot_index];
        for (uint32_t vert_id = 0; vert_id < num_vertices; ++vert_id)
        {
            // Get the input vertex to read, and output vertex to write to.
            uintptr_t output_offset_vertex = vert_id * in_vertices->vertex_stride;
            uintptr_t in_vertex_ptr = vb_base + (first_vertex + vert_id) * input_layout->input_slots[input_slot_index].stride_bytes;
            uintptr_t out_vertex_ptr = in_vertices->vertices_base + output_offset_vertex;
            // execute the vertex shader.
            vertex_shader->execute(in_vertex_ptr, out_vertex_ptr, vert_id);
        }
    }
    return result_ok;
}


error_t vertex_transformation_t::transform_indexed(vertices_t* in_vertices, uint32_t first_index, uint32_t first_vertex, uint32_t num_indices)
{
    uintptr_t ib_base = index_buffer;
    in_vertices->pos_offset = vertex_shader->get_out_pos_offset_bytes();
    in_vertices->vertex_stride = vertex_shader->get_out_vertex_stride();
    for (uint input_slot_index = 0; input_slot_index < input_layout->num_vbs; ++input_slot_index)
    {
        // Similar to the tranform(), number of vbs bound determines the input slot to use.
        uintptr_t vb_base = vertex_buffers[input_slot_index];
        for (uint index_id = 0; index_id < num_indices; ++index_id)
        {
            uintptr_t output_offset_vertex = index_id * in_vertices->vertex_stride;
            // Instead of using vertex id like transform, we instead use the index value from the index buffer.
            uint32_t index = *((uint32_t*)(ib_base + (first_index + index_id) * 4));
            uintptr_t in_vertex_ptr = vb_base + ((first_vertex + index) * input_layout->input_slots[input_slot_index].stride_bytes);
            uintptr_t out_vertex_ptr = in_vertices->vertices_base + output_offset_vertex;
            vertex_shader->execute(in_vertex_ptr, out_vertex_ptr, index_id);
        }
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



input_layout* input_assembler_t::create_input_layout(uint32_t num_elements, input_element_desc* descs)
{
    input_layout* layout = new input_layout();
    layout->input_slots[0].stride_bytes = 0;
    for (uint32_t element_i = 0; element_i < num_elements; ++element_i)
    {
        input_element_desc& desc = descs[element_i];
        uint32_t index = desc.input_slot;
        uint32_t size_bytes = format_size_bytes(desc.format);
        layout->input_slots[index].stride_bytes += size_bytes;

        // The maximum index is usually the number of expected vbs.
        layout->num_vbs = maximum<uint, uint, uint>(layout->num_vbs, index + 1);
    }
    return layout;
}
} // swrast