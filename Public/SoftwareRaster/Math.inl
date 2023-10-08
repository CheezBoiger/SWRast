//

namespace swrast {

template<typename type>
mat4x4_t<type> mat4x4_t<type>::operator*(const mat4x4_t<type>& rh) const
{
    mat4x4_t<type> ans;
    ans[0]  = m[0]  * rh[0] + m[1]  * rh[4] + m[2]  * rh[8]  + m[3]  * rh[12];
    ans[1]  = m[0]  * rh[1] + m[1]  * rh[5] + m[2]  * rh[9]  + m[3]  * rh[13];
    ans[2]  = m[0]  * rh[2] + m[1]  * rh[6] + m[2]  * rh[10] + m[3]  * rh[14];
    ans[3]  = m[0]  * rh[3] + m[1]  * rh[7] + m[2]  * rh[11] + m[3]  * rh[15];
    
    ans[4]  = m[4]  * rh[0] + m[5]  * rh[4] + m[6]  * rh[8]  + m[7]  * rh[12];
    ans[5]  = m[4]  * rh[1] + m[5]  * rh[5] + m[6]  * rh[9]  + m[7]  * rh[13];
    ans[6]  = m[4]  * rh[2] + m[5]  * rh[6] + m[6]  * rh[10] + m[7]  * rh[14];
    ans[7]  = m[4]  * rh[3] + m[5]  * rh[7] + m[6]  * rh[11] + m[7]  * rh[15];

    ans[8]  = m[8]  * rh[0] + m[9]  * rh[4] + m[10] * rh[8]  + m[11] * rh[12];
    ans[9]  = m[8]  * rh[1] + m[9]  * rh[5] + m[10] * rh[9]  + m[11] * rh[13];
    ans[10] = m[8]  * rh[2] + m[9]  * rh[6] + m[10] * rh[10] + m[11] * rh[14];
    ans[11] = m[8]  * rh[3] + m[9]  * rh[7] + m[10] * rh[11] + m[11] * rh[15];

    ans[12] = m[12] * rh[0] + m[13] * rh[4] + m[14] * rh[8]  + m[15] * rh[12];
    ans[13] = m[12] * rh[1] + m[13] * rh[5] + m[14] * rh[9]  + m[15] * rh[13];
    ans[14] = m[12] * rh[2] + m[13] * rh[6] + m[14] * rh[10] + m[15] * rh[14];
    ans[15] = m[12] * rh[3] + m[13] * rh[7] + m[14] * rh[11] + m[15] * rh[15];
    return ans;
}


template<typename type>
mat4x4_t<type> mat4x4_t<type>::operator+(const mat4x4_t<type>& rh) const
{
    mat4x4_t<type> ans;
    ans[0] =  m[0]  + rh[0];
    ans[1] =  m[1]  + rh[1];
    ans[2] =  m[2]  + rh[2];
    ans[3] =  m[3]  + rh[3];
    ans[4] =  m[4]  + rh[4];
    ans[5] =  m[5]  + rh[5];
    ans[6] =  m[6]  + rh[6];
    ans[7] =  m[7]  + rh[7];
    ans[8] =  m[8]  + rh[8];
    ans[9] =  m[9]  + rh[9];
    ans[10] = m[10] + rh[10];
    ans[11] = m[11] + rh[11];
    ans[12] = m[12] + rh[12];
    ans[13] = m[13] + rh[13];
    ans[14] = m[14] + rh[14];
    ans[15] = m[15] + rh[15];
    return ans;
}

template<typename type>
mat4x4_t<type> mat4x4_t<type>::operator-(const mat4x4_t<type>& rh) const
{
    mat4x4_t<type> ans;
    ans[0] =  m[0]  - rh[0];
    ans[1] =  m[1]  - rh[1];
    ans[2] =  m[2]  - rh[2];
    ans[3] =  m[3]  - rh[3];
    ans[4] =  m[4]  - rh[4];
    ans[5] =  m[5]  - rh[5];
    ans[6] =  m[6]  - rh[6];
    ans[7] =  m[7]  - rh[7];
    ans[8] =  m[8]  - rh[8];
    ans[9] =  m[9]  - rh[9];
    ans[10] = m[10] - rh[10];
    ans[11] = m[11] - rh[11];
    ans[12] = m[12] - rh[12];
    ans[13] = m[13] - rh[13];
    ans[14] = m[14] - rh[14];
    ans[15] = m[15] - rh[15];
    return ans;
}


template<typename type>
mat4x4_t<type> rotate(mat4x4_t<type>& origin, const vec3_t<type>& ax, type radians)
{
    type cosine          = cosf(radians);
    type sine            = sinf(radians);
    type oneMinusCosine  = 1.0f - cosine;

    vec3_t<type> axis  = normalize(ax);
    
    // Solves Rodrigues' Rotation Formula, applied to the final result. 
    mat4x4_t<type> rotator = 
        {
            cosine + (axis.x * axis.x) * oneMinusCosine,      
            oneMinusCosine * axis.y * axis.x + axis.z * sine, 
            axis.z * axis.x * oneMinusCosine - axis.y * sine, 
            0,
            axis.x * axis.y * oneMinusCosine - axis.z * sine, 
            cosine + (axis.y * axis.y) * oneMinusCosine,      
            axis.z * axis.y * oneMinusCosine + axis.x * sine, 
            0,
            axis.x * axis.z * oneMinusCosine + axis.y * sine, 
            axis.y * axis.z * oneMinusCosine - axis.x * sine, 
            cosine + (axis.z * axis.z) * oneMinusCosine,      
            0,
            0,                                                
            0,                                                
            0,                                                
            1        
        };
    
    return origin * rotator;
}

template<typename type>
mat4x4_t<type> translate(mat4x4_t<type>& origin, const vec3_t<type>& t)
{
    mat4x4_t<type> m = origin;
    m[12] += m[0] * t[0];
    m[13] += m[5] * t[1];
    m[14] += m[10] * t[2];
    return m;
}

template<typename type>
mat4x4_t<type> perspective_lh_aspect(type fov, type aspect, type ne, type fa)
{
    mat4x4_t<type> persp;
    type tanFov = tanf(fov * 0.5f);
    type y = 1.0f / tanFov;
    type x = y / aspect;
    persp[0]    = x;
    persp[5]    = y;
    persp[10]   = fa / (fa - ne);
    persp[11]   = 1.0f;
    persp[14]   = -ne * fa / (fa - ne);
    return persp;
}


template<typename type>
vec4_t<type> operator*(const vec4_t<type>& lh, const mat4x4_t<type>& rh)
{
    vec4_t<type> res;
    res[0] = rh[0]  * lh[0] + rh[4]  * lh[1] + rh[8]  * lh[2] + rh[12]  * lh[3];
    res[1] = rh[1]  * lh[0] + rh[5]  * lh[1] + rh[9]  * lh[2] + rh[13]  * lh[3];
    res[2] = rh[2]  * lh[0] + rh[6]  * lh[1] + rh[10] * lh[2] + rh[14] * lh[3];
    res[3] = rh[3] * lh[0] + rh[7] * lh[1] + rh[11] * lh[2] + rh[15] * lh[3];
    return res;
}


template<typename type>
type dot(const vec3_t<type>& l, const vec3_t<type>& r)
{
    return (l[0] * r[0]) + (l[1] * r[1]) + (l[2] * r[2]);
}


template<typename type>
type length(const vec3_t<type>& o)
{
    return sqrtf(dot(o, o));
}


template<typename type>
vec3_t<type> normalize(const vec3_t<type>& o)
{
    type mag = length(o);
    type inv = static_cast<type>(1) / mag;
    return o * inv;
}


template<typename type>
mat4x4_t<type> identity()
{
    return mat4x4_t<type>(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );
}
} // swrast