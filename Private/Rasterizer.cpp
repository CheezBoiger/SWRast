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
    float w_inv = 1.f / clip.w;
    float3_t ndc = float3_t(clip) * w_inv;
    return float4_t(ndc.x, ndc.y, ndc.z, w_inv);
}


float4_t rasterizer_t::ndc_to_screen(float4_t ndc_coord)
{
    const float width   = (float)m_viewports[0].width;
    const float height  = (float)m_viewports[0].height;
    const float x       = (float)m_viewports[0].x;
    const float y       = (float)m_viewports[0].y;
    const float f       = m_viewports[0].far;
    const float n       = m_viewports[0].near;
    // Relies on viewport transformation, in order to project our normalized device coordinates
    // to screen coordinates.
    return float4_t
        (
            floorf((width / 2) * ndc_coord.x + (x + width / 2)) + 0.5,
            floorf((height / 2) * ndc_coord.y + (y + height / 2)) + 0.5,
            ((f - n) / 2) * ndc_coord.z + ((f + n) / 2),
            ndc_coord.w
        );
}

static void calculate_winding_order(cull_mode_t cull_mode, front_face_t& out_face, float& out_area)
{
    // Negative area is flipped.
    switch (cull_mode)
    {
        case cull_mode_back:
        {
            // Reverse the facing direction, when looking at back face.
            out_area *= -1;
            out_face = out_face == front_face_clockwise ? front_face_counter_clockwise : front_face_clockwise;
            break;
        }
        case cull_mode_none:
        {
            if (out_area < 0)
            {
                out_area *= -1;
                out_face = out_face == front_face_clockwise ? front_face_counter_clockwise : front_face_clockwise;
            }
            break;
        }
        case cull_mode_front_and_back:
        {
            if (out_area > 0) out_area *= -1;
            break;
        }
        // Do nothing.
        default:
            break;
    }
}


error_t rasterizer_t::raster(uint32_t num_triangles, vertices_t& vertices, front_face_t winding_order)
{
    const bool depth_enabled = m_depth_enabled;
    const bool depth_write_enabled = depth_enabled && m_depth_write_enabled;

    // Check varying allocation pool.
    allocate_varying_heap(m_viewports[0].width * m_viewports[0].height * varying_max_size_bytes);

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
        
        // Manage the winding order, which affects the area of the triangle.
        calculate_winding_order(cull_mode, current_order, area);

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

                    // 
                    uintptr_t varying_address = allocate_varying();

                    m_bound_pixel_shader->interpolate_varying(varying_address, attrib_v0, attrib_v1, attrib_v2, persp_b);
                    
                    // execute the bound pixel shader. This should probably be optimized!
                    float4_t output = m_bound_pixel_shader ? m_bound_pixel_shader->execute(varying_address) : float4_t(0, 0, 0, 0);

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
    store_color(texel(render_target, uint2_t(x, y), format_size, row_pitch, 0), color, desc->format);
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
        store_color(texel(depth_stencil, uint2_t(x_s, y_s), format_size, row_pitch, 0), float4_t(depth, depth, depth, depth), desc->format);
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
        value = load_color(texel(depth_stencil, uint2_t(x_s, y_s), format_size, row_pitch, 0), desc->format).x;
    }
    return value;
}


error_t render_output_t::clear_render_target(framebuffer_t& framebuffer, uint32_t index, const rect_t& rect, const float4_t& clear_color)
{
    resource_t rt = framebuffer.bound_render_targets[index];
    resource_desc_t* resource_desc = (resource_desc_t*)(rt - sizeof(resource_desc_t));
    uint32_t format_size = format_size_bytes(resource_desc->format);
    // TODO: This needs to be configurable! Render target row_pitch needs to be the max width size!
    uint32_t row_pitch = resource_desc->width * format_size;
    uint32_t depth = resource_desc->height * row_pitch;
    for (uint32_t y = rect.y; y < rect.y + rect.height; ++y)
    {
        for (uint32_t x = rect.x; x < rect.x + rect.width; ++x)
        {
            store_color(texel(rt, uint2_t(x, y), format_size, row_pitch, depth), clear_color, resource_desc->format); 
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
    uint32_t z_depth = resource_desc->height * row_pitch;
    for (uint32_t y = rect.y; y < rect.y + rect.height; ++y)
    {
        for (uint32_t x = rect.x; x < rect.x + rect.width; ++x)
        {
            store_color(texel(ds, uint2_t(x, y), format_size, row_pitch, z_depth), float4_t(depth, depth, depth, depth), resource_desc->format); 
        }
    }
    return result_ok;
    return result_ok;
}


uintptr_t rasterizer_t::allocate_varying()
{
    uintptr_t varying_ptr = (uintptr_t)per_pixel_varying_allocator.allocate(varying_max_size_bytes, 4);
    return varying_ptr;
}


void rasterizer_t::allocate_varying_heap(size_t size_bytes)
{
    const uint64_t size = per_pixel_varying_allocator.get_memory_pool_size_bytes();
    if (size_bytes > size)
    {
        per_pixel_varying_allocator.resize_pool(size_bytes);
    }
    per_pixel_varying_allocator.reset();
}
} // swrast