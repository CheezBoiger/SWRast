//
#pragma once
#include <cstdint>


namespace swrast {

template<typename type>
struct vec2_t
{
    union 
    {
        struct { type x, y; };
        struct { type r, g; };
        struct { type s, t; };
    };
    vec2_t(type x = static_cast<type>(0), type y = static_cast<type>(0))
        : x(x), y(y)
    { }
    type& operator[](size_t i) { return (&x)[i]; }
    type operator[](size_t i) const { return (&x)[i]; }
    vec2_t operator+(const vec2_t& rh) const { return vec2_t(x + rh.x, y + rh.y); }
    vec2_t operator-(const vec2_t& rh) const { return vec2_t(x - rh.x, y - rh.y); }
    vec2_t operator*(const vec2_t& rh) const { return vec2_t(x * rh.x, y * rh.y); }
    vec2_t operator/(const vec2_t& rh) const { return vec2_t(x / rh.x, y / rh.y); }
    vec2_t operator+(type scalar) const { return vec2_t(x + scalar, y + scalar); }
    vec2_t operator-(type scalar) const { return vec2_t(x - scalar, y - scalar); }
    vec2_t operator*(type scalar) const { return vec2_t(x * scalar, y * scalar); }
    vec2_t operator/(type scalar) const { return vec2_t(x / scalar, y / scalar); }
};


template<typename type>
struct vec3_t
{
    union 
    {
        struct { type x, y, z; };
        struct { type r, g, b; };
        struct { type s, t, r; };
    };
    vec3_t(type x = static_cast<type>(0), type y = static_cast<type>(0), type z = static_cast<type>(0))
        : x(x), y(y), z(z)
    { }
    type& operator[](size_t i) { return (&x)[i]; }
    type operator[](size_t i) const { return (&x)[i]; }
    vec3_t operator+(const vec3_t& rh) const { return vec3_t(x + rh.x, y + rh.y, z + rh.z); }
    vec3_t operator-(const vec3_t& rh) const { return vec3_t(x - rh.x, y - rh.y, z - rh.z); }
    vec3_t operator*(const vec3_t& rh) const { return vec3_t(x * rh.x, y * rh.y, z * rh.z); }
    vec3_t operator/(const vec3_t& rh) const { return vec3_t(x / rh.x, y / rh.y, z / rh.z); }
    vec3_t operator+(type scalar) const { return vec3_t(x + scalar, y + scalar, z + scalar); }
    vec3_t operator-(type scalar) const { return vec3_t(x - scalar, y - scalar, z - scalar); }
    vec3_t operator*(type scalar) const { return vec3_t(x * scalar, y * scalar, z * scalar); }
    vec3_t operator/(type scalar) const { return vec3_t(x / scalar, y / scalar, z / scalar); }

    template<typename t>
    operator vec2_t<t> () 
    {
        return vec2_t<t>(x, y);
    }
};


template<typename type>
struct vec4_t
{
    union 
    {
        struct { type x, y, z, w; };
        struct { type r, g, b, a; };
        struct { type s, t, r, q; };
    };
    vec4_t(type x = static_cast<type>(0), type y = static_cast<type>(0), type z = static_cast<type>(0), type w = static_cast<type>(0))
        : x(x), y(y), z(z), w(w)
    { }
    type& operator[](size_t i) { return (&x)[i]; }
    type operator[](size_t i) const { return (&x)[i]; }
    vec4_t operator+(const vec4_t& rh) const { return vec4_t(x + rh.x, y + rh.y, z + rh.z, w + rh.w); }
    vec4_t operator-(const vec4_t& rh) const { return vec4_t(x - rh.x, y - rh.y, z - rh.z, w - rh.w); }
    vec4_t operator*(const vec4_t& rh) const { return vec4_t(x * rh.x, y * rh.y, z * rh.z, w * rh.w); }
    vec4_t operator/(const vec4_t& rh) const { return vec43_t(x / rh.x, y / rh.y, z / rh.z, w / rh.w); }
    vec4_t operator+(type scalar) const { return vec4_t(x + scalar, y + scalar, z + scalar, w + scalar); }
    vec4_t operator-(type scalar) const { return vec4_t(x - scalar, y - scalar, z - scalar, w - scalar); }
    vec4_t operator*(type scalar) const { return vec4_t(x * scalar, y * scalar, z * scalar, w * scalar); }
    vec4_t operator/(type scalar) const { return vec4_t(x / scalar, y / scalar, z / scalar, w / scalar); }

    template<typename type>
    operator vec2_t<type> ()
    {
        return vec2_t<type>(x, y);
    }

    template<typename type>
    operator vec3_t<type> ()
    {
        return vec3_t<type>(x, y, z);
    }
};


template<typename type>
vec3_t<type> operator+(type scalar, const vec3_t<type>& rh)
{
    return rh + scalar;
}


template<typename type>
vec4_t<type> operator+(type scalar, const vec4_t<type>& rh)
{
    return rh + scalar;
}


template<typename type>
vec3_t<type> operator*(type scalar, const vec3_t<type>& rh)
{
    return rh * scalar;
}


template<typename type>
vec4_t<type> operator*(type scalar, const vec4_t<type>& rh)
{
    return rh * scalar;
}



template<typename type>
struct mat3x3_t
{
    union
    {
        struct { type m[9]; };
        struct { vec3_t<type> row0, row1, row2; };
    };
};


template<typename type>
struct mat4x4_t
{
    union
    {
        struct { type m[16]; };
        struct { vec4_t<type> row0, row1, row2, row3; };
    };

    mat4x4_t() { }
};


template<typename type>
struct axis_aligned_bounds2d_t
{
    vec2_t<type> minima;
    vec2_t<type> maxima;
};


template<typename type>
struct axis_aligned_bounds3d_t
{
    vec3_t<type> minima;
    vec3_t<type> maxima;
};

template<typename type>
bool intersects(const axis_aligned_bounds2d_t<type>& a, const axis_aligned_bounds2d_t<type>& b);
template<typename type> 
bool intersects(const axis_aligned_bounds3d_t<type>& a, const axis_aligned_bounds3d_t<type>& b);
template<typename type>
bool inside(const axis_aligned_bounds2d_t<type>& a, const axis_aligned_bounds2d_t<type>& b);
template<typename type>
bool inside(const axis_aligned_bounds3d_t<type>& a, const axis_aligned_bounds3d_t<type>& b);
template<typename type_result, typename type0, typename type1>
type_result maximum(const type0& a, const type1& b)
{
    return static_cast<type_result>(a > b ? a : b);
}
template<typename type_result, typename type0, typename type1>
type_result minimum(const type0& a, const type1& b)
{
    return static_cast<type_result>(a > b ? b : a);
}

typedef axis_aligned_bounds2d_t<float>      fbounds2d_t;
typedef axis_aligned_bounds2d_t<uint32_t>   ubounds2d_t;
typedef axis_aligned_bounds2d_t<int32_t>    ibounds2d_t; 

typedef axis_aligned_bounds3d_t<float>      fbounds3d_t;
typedef axis_aligned_bounds3d_t<uint32_t>   ubounds3d_t;
typedef axis_aligned_bounds3d_t<int32_t>    ibounds3d_t; 

typedef vec2_t<double>      double2_t;
typedef vec2_t<float>       float2_t;
typedef vec2_t<uint32_t>    uint2_t;
typedef vec2_t<uint16_t>    short2_t;
typedef vec2_t<int32_t>     int2_t;
typedef vec2_t<int16_t>     ushort2_t;
typedef vec2_t<uint8_t>     ubyte2_t;
typedef vec2_t<int8_t>      byte2_t;

typedef vec3_t<double>      double3_t;
typedef vec3_t<float>       float3_t;
typedef vec3_t<uint32_t>    uint3_t;
typedef vec3_t<uint16_t>    short3_t;
typedef vec3_t<int32_t>     int3_t;
typedef vec3_t<int16_t>     ushort3_t;
typedef vec3_t<uint8_t>     ubyte3_t;
typedef vec3_t<int8_t>      byte3_t;

typedef vec4_t<double>      double4_t;
typedef vec4_t<float>       float4_t;
typedef vec4_t<uint32_t>    uint4_t;
typedef vec4_t<uint16_t>    short4_t;
typedef vec4_t<int32_t>     int4_t;
typedef vec4_t<int16_t>     ushort4_t;
typedef vec4_t<uint8_t>     ubyte4_t;
typedef vec4_t<int8_t>      byte4_t;

typedef mat4x4_t<float>     float4x4_t;
typedef mat4x4_t<uint32_t>  uint4x4_t;
typedef mat4x4_t<int32_t>   int4x4_t;

typedef mat3x3_t<float>     float3x3_t;
typedef mat3x3_t<uint32_t>  uint3x3_t;
typedef mat3x3_t<int32_t>   int3x3_t;

// Triangle structure.
struct triangle_t
{
    float4_t v0, v1, v2;
};
} // swrast