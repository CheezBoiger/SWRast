//
#pragma once
#include <cstdint>
#include <cmath>

#define SW_CONST_PI                3.141592653589793238462643383279502884197169399375
#define SW_CONST_PI_HALF           1.57079632679489661923   // pi/2
#define SW_CONST_PI_QUARTER        0.785398163397448309616 // pi/4
#define SW_CONST_2_PI              6.283185307 // 2 * pi
#define SW_CONST_TOLERANCE         0.0001     // 
#define SW_EPSILON                 0.0000001 // 
#define SW_E                       2.71828182845904523536   // e

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
    
    template<typename o>
    vec2_t(const vec2_t<o>& other)
        : x(static_cast<type>(other[0]))
        , y(static_cast<type>(other[1]))
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

    template<typename o>
    vec3_t(const vec3_t<o>& other)
        : x(static_cast<type>(other[0]))
        , y(static_cast<type>(other[1]))
        , z(static_cast<type>(other[2]))
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
    vec4_t(const vec3_t<type>& comp3, type w = static_cast<type>(0))
        : x(comp3[0]), y(comp3[1]), z(comp3[2]), w(w) 
    { }

    template<typename o>
    vec4_t(const vec4_t<o>& other)
        : x(static_cast<type>(other[0]))
        , y(static_cast<type>(other[1]))
        , z(static_cast<type>(other[2]))
        , w(static_cast<type>(other[3]))
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

    mat4x4_t(type a00 = static_cast<type>(0), type a01 = static_cast<type>(0), type a02 = static_cast<type>(0), type a03 = static_cast<type>(0),
             type a10 = static_cast<type>(0), type a11 = static_cast<type>(0), type a12 = static_cast<type>(0), type a13 = static_cast<type>(0),
             type a20 = static_cast<type>(0), type a21 = static_cast<type>(0), type a22 = static_cast<type>(0), type a23 = static_cast<type>(0),
             type a30 = static_cast<type>(0), type a31 = static_cast<type>(0), type a32 = static_cast<type>(0), type a33 = static_cast<type>(0)) 
    {
        m[0]  = a00; m[1]  = a01; m[2]  = a02; m[3]  = a03;
        m[4]  = a10; m[5]  = a11; m[6]  = a12; m[7]  = a13;
        m[8]  = a20; m[9]  = a21; m[10] = a22; m[11] = a23;
        m[12] = a30; m[13] = a31; m[14] = a32; m[15] = a33;
    }

    mat4x4_t(const vec4_t<type>& row0,
             const vec4_t<type>& row1,
             const vec4_t<type>& row2,
             const vec4_t<type>& row3)
    {
        this->row0 = row0;
        this->row1 = row1;
        this->row2 = row2;
        this->row3 = row3;
    }

    mat4x4_t<type> operator+(const mat4x4_t<type>& rh) const;
    mat4x4_t<type> operator-(const mat4x4_t<type>& rh) const;
    mat4x4_t<type> operator*(const mat4x4_t<type>& rh) const;
    
    // Performs a column major order multiplication.
    vec4_t<type>  operator*(const vec4_t<type>& rh) const;

    type operator[](uint32_t index) const { return m[index]; }
    type& operator[](uint32_t index) { return m[index]; }
};

template<typename type>
vec4_t<type> operator*(const vec4_t<type>& lh, const mat4x4_t<type>& rh);

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

template<typename type>
vec3_t<type> cross(const vec3_t<type>& l, const vec3_t<type>& r);

template<typename type>
type dot(const vec3_t<type>& l, const vec3_t<type>& r);

template<typename type>
type dot(const vec4_t<type>& l, const vec4_t<type>& r);

template<typename type>
type length(const vec4_t<type>& o);

template<typename type>
type length(const vec3_t<type>& o);

template<typename type>
vec4_t<type> normalize(const vec4_t<type>& o);
template<typename type>
vec3_t<type> normalize(const vec3_t<type>& o);

template<typename type>
mat4x4_t<type> perspective_lh_aspect(type fov, type aspect, type ne, type fa);

template<typename type>
mat4x4_t<type> rotate(mat4x4_t<type>& origin, const vec3_t<type>& axis, type radians);

template<typename type>
mat4x4_t<type> translate(mat4x4_t<type>& origin, const vec3_t<type>& t);

template<typename type>
mat4x4_t<type> scale(mat4x4_t<type>& origin, const vec3_t<type>& s);

template<typename type>
mat4x4_t<type> identity();

template<typename type>
type clamp(type v, type mmin, type mmax);

template<typename type>
vec2_t<type> clamp(const vec2_t<type>& v, const vec2_t<type>& mmin, const vec2_t<type>& mmax); 

template<typename type>
vec2_t<type> floor(const vec2_t<type>& v);

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


namespace {
float deg_to_rad(float deg)
{
    return deg * ((float)SW_CONST_PI / 180.0f);
}
}
// Triangle structure.
struct triangle_t
{
    float4_t v0, v1, v2;
};
} // swrast

#include "Math.inl"