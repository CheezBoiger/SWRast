//
#include "Context.hpp"
#include "Math.hpp"
#include "InputAssembly.hpp"
#include "Allocator.hpp"
#include <cstdint>

namespace swrast {


class render_output_t;

struct framebuffer_t
{
    resource_t bound_render_targets[8];
    resource_t bound_depth_stencil;
    uint32_t num_render_targets;
    uint32_t max_width;
    uint32_t max_height;
};

// Render output ideally handles how we should be outputting to our 
// render target (the format and size must be taken into account.)
class render_output_t
{
public:

    // shade the framebuffer render target.
    //
    // index = render target index.   
    // x_s = x pixel in screen space.
    // y_s = y pixel in screen space.
    // color = output color to color the pixel.
    //
    error_t shade_to_output(framebuffer_t& framebuffer, uint32_t index, const viewport_t& viewport, uint32_t x_s, uint32_t y_s, const float4_t& color);

    // Write to the depth stencil buffer, if one is bounded. This must also mean that the 
    // config to handle depth testing, must be enabled.
    error_t write_to_depth_stencil(framebuffer_t& framebuffer, const viewport_t& viewport, uint32_t x_s, uint32_t y_s, float depth);
    float read_depth_stencil(const framebuffer_t& framebuffer, const viewport_t& viewport, uint32_t x_s, uint32_t y_s);
    // Clear a render target in the framebuffer.
    error_t clear_render_target(framebuffer_t& framebuffer, uint32_t index, const rect_t& rect, const float4_t& clear_color);
    // Clear depth stencil in the frame buffer.
    error_t clear_depth_stencil(framebuffer_t& framebuffer, const rect_t& rect, float depth);
private:
    
};


// Clipper takes in pimritives in clipspace, and discards (or culls) any primitives that are not 
// visible within the viewspace of the camera. Since we mostly utilize triangles in modern rendering, 
// any primitives that are partially within the view volume, will have two special cases:
//      1. If two vertices of the triangle are outside the viewing volume, then we go ahead 
//         and trim both outside vertices to be at the edge of the viewing volume.
//      2. If one vertex is outside the viewing volume, we must temporarily split the triangle
//         into two (effectively adding one extra triangle) which will 
//
// Of course, there are multiple other special cases we need to consider as well, such as when triangles 
// are on the corner of our view volume. These will have their own special cases.
class clipper_t
{
public:
    // initialize the clipper's clip space.
    error_t initialize()
    {
        return result_ok; 
    }

    error_t release() 
    {
        return result_ok; 
    }

    // vertices passed, must be in clip space
    // post processes vertices in clip space, culling any vertices not in the viewing volume, or 
    // clipping any vertices that aren't visible.
    error_t clip_cull(vertices_t* inout_vertex_pool);
private:
    // the viewing volume in normalized device coordinates.
    fbounds3d_t ndc;

    // triangle clip holds onto the plane mask of which a triangle is within. the encoding of this is stored
    // in an 8 bit variable, but we only use 4 bits to represent 5 planes.
    // 
    // 0000 = inside view volume.
    // 1000 = top plane.
    // 0100 = bottom plane.
    // 0010 = left plane.
    // 0001 = right plane.
    struct triangle_clip_t
    {
        uint8_t plane_mask;
    };
};


// framebuffer tile.
struct tile_t
{
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
    uint32_t tile_id;
};


// Rasterizer performs the actual work of projecting the clip space vertices into pixel space, or screen space.
// Should handle typical needs such as perspective projection, vertex interpolation with barycentrics, and 
// invoking the given pixel shader.
class rasterizer_t 
{
public:
    error_t initialize(const fbounds3d_t& ndc) { ndc_space = ndc; return result_ok; }
    error_t release() { return result_ok; }

    // Bind a framebuffer to this rasterizer.
    error_t bind_frame_buffer(framebuffer_t framebuffer) { m_bound_framebuffer = framebuffer; return result_ok; }

    // Perform rasterization with the given input triangles.
    // Triangles must be in clip space. Perspective projection will be conducted in here.
    error_t raster(uint32_t num_triangles, vertices_t& vertices, front_face_t winding_order);

    // Bind the pixel shader, this is invoked per pixel.
    error_t bind_pixel_shader(pixel_shader_t* shader) { m_bound_pixel_shader = shader; return result_ok; }
    
    error_t set_viewports(uint32_t num_viewports, viewport_t* viewports);
    void enable_depth(bool enable) { m_depth_enabled = enable; }
    void enable_write_depth(bool enable) { m_depth_write_enabled = enable;  }

    error_t clear_render_target(uint32_t index, const rect_t& rect, const float4_t& clear_color) { return rop.clear_render_target(m_bound_framebuffer, index, rect, clear_color); }
    error_t clear_depth_stencil(const rect_t& rect, float depth) { return rop.clear_depth_stencil(m_bound_framebuffer, rect, depth); }
    render_output_t& get_rop() { return rop; }

    void set_depth_compare_op(compare_op_t compare_op) { depth_compare = compare_op; }
    void set_cull_mode(cull_mode_t cull) { cull_mode = cull; }

private:

    // Allocate a varying struct.
    uintptr_t allocate_varying();

    void allocate_varying_heap(size_t size_bytes);    

    // Projects ndc coordinates to screen coordinates.
    float4_t ndc_to_screen(float4_t ndc_coord);
    float4_t clip_to_ndc(float4_t clip);

    // calculate the bounding volume of a triangle, with the given 3 points in screen space.
    ibounds2d_t calculate_bounding_volume2d(const float2_t& a, const float2_t& b, const float2_t& c);

    // Find the edge bounds of the triangle. This calculates if a point is within
    // the area of the triangle.
    float edge_function(const float2_t& a, const float2_t& b, const float2_t& c);
    
    fbounds3d_t     ndc_space;
    render_output_t rop;
    framebuffer_t   m_bound_framebuffer;
    pixel_shader_t* m_bound_pixel_shader;
    viewport_t      m_viewports[8];
    compare_op_t    depth_compare = compare_op_less;
    cull_mode_t     cull_mode = cull_mode_none;
    bool            m_depth_enabled = false;
    bool            m_depth_write_enabled = false;
    linear_allocator_t per_pixel_varying_allocator;
    const uint64_t  varying_max_size_bytes = SWRAST_MAX_VARYING_SIZE_BYTES;

    // triangle bin, holds triangles that pertain to this bin.
    struct triangle_bin_t
    {
        triangle_t* triangles;
        uint32_t    num_triangles;
        uint16_t    bin_id;
    };
};
} // swrast