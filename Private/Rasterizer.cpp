//
#include "Rasterizer.hpp"

namespace swrast {


error_t clipper_t::clip_cull(vertices_t* inout_vertex_pool)
{
    for (uint32_t vert_id = 0; vert_id < inout_vertex_pool->num_vertices; ++vert_id)
    {
        // Vertices must statisfy the following equation, otherwise they are considered clipped.
        // if all vertices of a triangle are clipped, we consider it culled.
        // -wp < xp <= wp
        // -wp < yp <= wp
        // 0 < zp <= wp
    }
    return result_ok;
}


bool is_pass_depth_test(compare_op_t op, float dest_depth, float source_depth)
{
    switch (op)
    {
        case compare_op_equal:
            return dest_depth == source_depth;
        case compare_op_greater:
            return source_depth > dest_depth; 
        case compare_op_greater_equal:
            return source_depth >= dest_depth;
        case compare_op_less:
            return source_depth < dest_depth;
        case compare_op_less_equal:
            return source_depth <= dest_depth;
        default:
            break;
    }
    return true;
}


float4_t rasterizer_t::clip_to_ndc(float4_t clip)
{    
    // Perspective division is done in order to project to ndc space.
    float3_t ndc = float3_t(clip) / clip.w;
    return float4_t(ndc.x, ndc.y, ndc.z, 1.0f / clip.w);
}


float4_t rasterizer_t::ndc_to_screen(float4_t ndc_coord)
{
    const float width = m_viewports[0].width;
    const float height = m_viewports[0].height;
    const float x = m_viewports[0].x;
    const float y = m_viewports[0].y;
    const float f = m_viewports[0].far;
    const float n = m_viewports[0].near;
    // Relies on viewport transformation, in order to project our normalized device coordinates
    // to screen coordinates.
    return float4_t
        (
            (width / 2) * ndc_coord.x + (x + width / 2),
            (height / 2) * ndc_coord.y + (y + height / 2),
            ((f - n) / 2) * ndc_coord.z + ((f + n) / 2),
            ndc_coord.w
        );
}


error_t rasterizer_t::raster(uint32_t num_triangles, vertices_t& vertices, front_face_t winding_order)
{
    const bool depth_enabled = m_depth_enabled;
    const bool depth_write_enabled = depth_enabled && m_depth_write_enabled;

    // Convert our triangle from clip space to ndc space.
    for (uint32_t tri_id = 0; tri_id < num_triangles; ++tri_id)
    {
        float4_t& v0 = vertices.get_vertex_position(tri_id * 3 + 0);
        float4_t& v1 = vertices.get_vertex_position(tri_id * 3 + 1);
        float4_t& v2 = vertices.get_vertex_position(tri_id * 3 + 2);
        v0 = clip_to_ndc(v0);
        v1 = clip_to_ndc(v1);
        v2 = clip_to_ndc(v2);

        // From ndc space, we project to raster space (screen space.)
        // our w is left as is (1 / w)
        v0 = ndc_to_screen(v0);
        v1 = ndc_to_screen(v1);
        v2 = ndc_to_screen(v2);
    }

    // NDC triangles convert to raster space, and are rasterized!
    for (uint32_t tri_id = 0; tri_id < num_triangles; ++tri_id)
    {
        // Triangles should be in raster space. (except 1 / w)
        float4_t v0_s = vertices.get_vertex_position(tri_id * 3 + 0);
        float4_t v1_s = vertices.get_vertex_position(tri_id * 3 + 1);
        float4_t v2_s = vertices.get_vertex_position(tri_id * 3 + 2);

        float area = winding_order == front_face_clockwise 
                                        ? edge_function(v0_s, v2_s, v1_s) 
                                        : edge_function(v0_s, v1_s, v2_s);
        front_face_t current_order = winding_order;
        // Negative area is flipped.
        switch (cull_mode)
        {
            case cull_mode_back:
            {
                // Reverse the facing direction, when looking at back face.
                area *= -1;
                current_order = winding_order == front_face_clockwise ? front_face_counter_clockwise : front_face_clockwise;
                break;
            }
            case cull_mode_none:
            {
                if (area < 0)
                {
                    area *= -1;
                    current_order = winding_order == front_face_clockwise ? front_face_counter_clockwise : front_face_clockwise;
                }
                break;
            }
            case cull_mode_front_and_back:
            {
                if (area > 0) area *= -1;
                break;
            }
            // Do nothing.
            default:
                break;
        }

        // Cull if area is negative.
        if (area < 0)
            continue;

        // We use raster space to calculate the bounding box of the 
        // triangle on screen, to which here we then perform the actual rasterization.
        ibounds2d_t bounds = calculate_bounding_volume2d(v0_s, v1_s, v2_s);

        // This is not the most optimal way to rasterize a triangle, but it beats
        // traversing the entire framebuffer, in order to check for shaded fragments 
        // that are covered.
        for (int32_t y_s = bounds.minima.y; y_s < bounds.maxima.y; ++y_s)
        {
            for (int32_t x_s = bounds.minima.x; x_s < bounds.maxima.x; ++x_s)
            {
                float2_t p = { (float)x_s + 0.5f, (float)y_s + 0.5f };
                float w0 = 0.f;
                float w1 = 0.f;
                float w2 = 0.f;
                switch (current_order)
                {
                    case front_face_counter_clockwise:                
                    {
                        w0 = edge_function(v1_s, v2_s, p); 
                        w1 = edge_function(v2_s, v0_s, p);
                        w2 = edge_function(v0_s, v1_s, p);
                        break;
                   }
                    case front_face_clockwise:
                    {
                        w0 = edge_function(v2_s, v1_s, p); 
                        w1 = edge_function(v0_s, v2_s, p);
                        w2 = edge_function(v1_s, v0_s, p);
                        break;
                    }
                }
                // rasterize!
                if (w0 >= 0 && w1 >= 0 && w2 >= 0)
                {
                    // Barycentric coordinates are calculated
                    w0 /= area;
                    w1 /= area;
                    w2 /= area;

                    // linearly interpolate z and w
                    float w_inv = 1.f / (w0 * v0_s.w + w1 * v1_s.w + w2 * v2_s.w);
                    float z = 1.f / (v0_s.z * w0 + v1_s.z * w1 + v2_s.z * w2);
                    if (m_depth_enabled)
                    {
                        float dest_value = rop.read_depth_stencil(m_bound_framebuffer, m_viewports[0], x_s, y_s);
                        if (!is_pass_depth_test(depth_compare, dest_value, z))
                        {
                            // Failed depth test, don't write to pixel.
                            continue;
                        }
                    }

                    // Perspective correction on our barycentrics.
                    float3_t persp_b = float3_t
                        (
                            w_inv * v0_s.w * w0, 
                            w_inv * v1_s.w * w1, 
                            w_inv * v2_s.w * w2
                        );

                    uintptr_t attrib_v0 = vertices.get_vertex(tri_id * 3 + 0);
                    uintptr_t attrib_v1 = vertices.get_vertex(tri_id * 3 + 1);
                    uintptr_t attrib_v2 = vertices.get_vertex(tri_id * 3 + 2);
                    m_bound_pixel_shader->interpolate_varying(attrib_v0, attrib_v1, attrib_v2, persp_b);
                    
                    // execute the bound pixel shader. This should probably be optimized!
                    float4_t output = m_bound_pixel_shader ? m_bound_pixel_shader->execute() : float4_t(0, 0, 0, 0);
                    // Finally, store the shaded pixel into the framebuffer.
                    rop.shade_to_output(m_bound_framebuffer, 0, m_viewports[0], x_s, y_s, output);
                    if (m_depth_write_enabled)
                    {
                        rop.write_to_depth_stencil(m_bound_framebuffer, m_viewports[0], x_s, y_s, z);
                    }
                }
            }
        }
    }
    return result_ok;
}


float rasterizer_t::edge_function(const float2_t& a, const float2_t& b, const float2_t& c)
{
    return (c[0] - a[0]) * (b[1] - a[1]) - (c[1] - a[1]) * (b[0] - a[0]); 
}


ibounds2d_t rasterizer_t::calculate_bounding_volume2d(const float2_t& a, const float2_t& b, const float2_t& c)
{
    ibounds2d_t bounds;
    bounds.maxima.x = (int32_t)clamp<float>(maximum<float>(maximum<float>(a.x, b.x), c.x), 0.f, m_viewports[0].width);
    bounds.maxima.y = (int32_t)clamp<float>(maximum<float>(maximum<float>(a.y, b.y), c.y), 0.f, m_viewports[0].height);
    bounds.minima.x = (int32_t)clamp<float>(minimum<float>(minimum<float>(a.x, b.x), c.x), 0.f, m_viewports[0].width);
    bounds.minima.y = (int32_t)clamp<float>(minimum<float>(minimum<float>(a.y, b.y), c.y), 0.f, m_viewports[0].height);
    return bounds;
}


error_t rasterizer_t::set_viewports(uint32_t num_viewports, viewport_t* viewports)
{
    for (uint32_t i = 0; i < num_viewports; ++i)
    {
        m_viewports[i] = viewports[i];
    }
    return result_ok;
}


error_t render_output_t::shade_to_output(framebuffer_t& framebuffer, uint32_t index, const viewport_t& viewport, uint32_t x, uint32_t y, const float4_t& color)
{
    resource_t render_target = framebuffer.bound_render_targets[index];
    resource_desc_t* desc = (resource_desc_t*)(render_target - sizeof(resource_desc_t));
    uintptr_t format_size = format_size_bytes(desc->format);
    uintptr_t ptr = render_target;
    // Address must be based on the format of the render target, for this we need to know ahead of time, what
    // that will be.
    // TODO: Need to perform the render output based on the byte stride format. Not the other way around!
    const uintptr_t row_pitch = viewport.width * format_size;
    uintptr_t address = ptr + (x * format_size + (y * row_pitch));

    switch (desc->format)
    {
        case format_r8g8b8a8_unorm:
        {
            ((uint8_t*)address)[0] = color.r * 255.f;
            ((uint8_t*)address)[1] = color.g * 255.f;
            ((uint8_t*)address)[2] = color.b * 255.f;
            ((uint8_t*)address)[3] = color.a * 255.f;
            break;
        }
        default:
            // Format is unknown, likely going to need to be skipped?
            break;
    }
    return result_ok;
}


error_t render_output_t::write_to_depth_stencil(framebuffer_t& framebuffer, const viewport_t& viewport, uint32_t x_s, uint32_t y_s, float depth)
{
    resource_t depth_stencil = framebuffer.bound_depth_stencil;
    resource_desc_t* desc = (resource_desc_t*)(depth_stencil - sizeof(resource_desc_t));
    uintptr_t format_size = format_size_bytes(desc->format);
    if (depth_stencil)
    {
        uintptr_t base_address = depth_stencil;
        const uintptr_t row_pitch = viewport.width * format_size;
        uintptr_t address = base_address + (x_s * format_size + (y_s * row_pitch));
        switch (desc->format)
        {
            case format_r32_float:
                *((float*)address) = depth; 
                break;
            default:
                // skipped?
                break;
        }
    }
    return result_failed;
}


float render_output_t::read_depth_stencil(const framebuffer_t& framebuffer, const viewport_t& viewport, uint32_t x_s, uint32_t y_s)
{
    const resource_t depth_stencil = framebuffer.bound_depth_stencil;
    resource_desc_t* desc = (resource_desc_t*)(depth_stencil - sizeof(resource_desc_t));
    uintptr_t format_size = format_size_bytes(desc->format);
    float value = 0.f;
    if (depth_stencil)
    {
        const uintptr_t base_address = depth_stencil;
        const uintptr_t row_pitch = viewport.width * format_size;
        const uintptr_t address = base_address + (x_s * format_size + (y_s * row_pitch));
        switch (desc->format)
        {
            case format_r32_float:
                value = *((const float*)address); 
                break;
            default:
                // skipped?
                break;
        }
    }
    return value;
}


error_t render_output_t::clear_render_target(framebuffer_t& framebuffer, uint32_t index, const rect_t& rect, const float4_t& clear_color)
{
    resource_t rt = framebuffer.bound_render_targets[index];
    resource_desc_t* resource_desc = (resource_desc_t*)(rt - sizeof(resource_desc_t));
    uint32_t format_size = format_size_bytes(resource_desc->format);
    uint32_t r = (uint32_t)(clear_color.r * 255.f);
    uint32_t g = (uint32_t)(clear_color.g * 255.f);
    uint32_t b = (uint32_t)(clear_color.b * 255.f);
    uint32_t a = (uint32_t)(clear_color.a * 255.f);
    uint32_t rgba = (r) | (g << 8) | (b << 16) | (a << 24);
    // TODO: This needs to be configurable! Render target row_pitch needs to be the max width size!
    uint32_t row_pitch = resource_desc->width * format_size;
    for (uint32_t y = rect.y; y < rect.y + rect.height; ++y)
    {
        for (uint32_t x = rect.x; x < rect.x + rect.width; ++x)
        {
            uint32_t* output = (uint32_t*)(((uintptr_t)rt) + (x * format_size) + (y * row_pitch));
            *output = rgba;
        }
    }
    return result_ok;
}


error_t render_output_t::clear_depth_stencil(framebuffer_t& framebuffer, const rect_t& rect, float depth)
{   
    resource_t ds = framebuffer.bound_depth_stencil;
    resource_desc_t* resource_desc = (resource_desc_t*)(ds - sizeof(resource_desc_t));
    uint32_t format_size = format_size_bytes(resource_desc->format);
    // TODO: This needs to be configurable! Render target row_pitch needs to be the max width size!
    uint32_t row_pitch = resource_desc->width * format_size;
    for (uint32_t y = rect.y; y < rect.y + rect.height; ++y)
    {
        for (uint32_t x = rect.x; x < rect.x + rect.width; ++x)
        {
            uint32_t* output = (uint32_t*)(((uintptr_t)ds) + (x * format_size) + (y * row_pitch));
            *output = depth;
        }
    }
    return result_ok;
    return result_ok;
}
} // swrast