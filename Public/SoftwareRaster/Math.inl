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
mat4x4_t<type> mat4x4_t<type>::operator*(type scalar) const
{
    mat4x4_t<type> ans = *this;
    ans[0]  *= scalar;
    ans[1]  *= scalar;
    ans[2]  *= scalar;
    ans[3]  *= scalar;
     
    ans[4]  *= scalar;
    ans[5]  *= scalar;
    ans[6]  *= scalar;
    ans[7]  *= scalar;
    
    ans[8]  *= scalar;
    ans[9]  *= scalar;
    ans[10] *= scalar;
    ans[11] *= scalar;
    
    ans[12] *= scalar;
    ans[13] *= scalar;
    ans[14] *= scalar;
    ans[15] *= scalar;
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
mat4x4_t<type> scale(mat4x4_t<type>& origin, const vec3_t<type>& s)
{
    mat4x4_t<type> o = origin;
    o[0]  *= s.x;
    o[5]  *= s.y;
    o[10] *= s.z;
    return o;
}


template<typename type>
mat4x4_t<type> transpose(const mat4x4_t<type>& t)
{
    mat4x4_t<type> ans = t;
    ans[1]          = t[4];
    ans[2]          = t[8];
    ans[3]          = t[12];
    ans[7]          = t[13];
    ans[9]          = t[6];
    ans[11]         = t[14];
    ans[12]         = t[3];
    ans[13]         = t[7];
    ans[14]         = t[11];
    ans[8]          = t[2];
    ans[4]          = t[1];
    ans[6]          = t[9];
    return ans;
}


template<typename type>
mat4x4_t<type> adjugate(const mat4x4_t<type>& lh)
{
    mat4x4_t<type> cM;
    // Handles the case of finding our Transpose-Cofactor matrix.
    // This will then be used to locate our inverse.
    //
    cM[0] =     lh[5] * lh[10] * lh[15] + 
                lh[6] * lh[11] * lh[13] +
                lh[7] * lh[9]  * lh[14] -
                lh[7] * lh[10] * lh[13] -
                lh[6] * lh[9]  * lh[15] -
                lh[5] * lh[11] * lh[14];
    cM[1] = -(  lh[1] * lh[10] * lh[15] +
                lh[2] * lh[11] * lh[13] +
                lh[3] * lh[9]  * lh[14] -
                lh[3] * lh[10] * lh[13] -
                lh[2] * lh[9]  * lh[15] -
                lh[1] * lh[11] * lh[14] );
    cM[2] =     lh[1] * lh[6]  * lh[15] +
                lh[2] * lh[7]  * lh[13] +
                lh[3] * lh[5]  * lh[14] -
                lh[3] * lh[6]  * lh[13] -
                lh[2] * lh[5]  * lh[15] -
                lh[1] * lh[7]  * lh[14];
    cM[3] = -(  lh[1] * lh[6]  * lh[11] +
                lh[2] * lh[7]  * lh[9]  +
                lh[3] * lh[5]  * lh[10] -
                lh[3] * lh[6]  * lh[9]  -
                lh[2] * lh[5]  * lh[11] -
                lh[1] * lh[7]  * lh[10] );
    cM[4] = -(  lh[4] * lh[10] * lh[15] +
                lh[6] * lh[11] * lh[12] +
                lh[7] * lh[8]  * lh[14] -
                lh[7] * lh[10] * lh[12] -
                lh[6] * lh[8]  * lh[15] -
                lh[4] * lh[11] * lh[14] ); 
    cM[5] =     lh[0] * lh[10] * lh[15] + 
                lh[2] * lh[11] * lh[12] +
                lh[3] * lh[8]  * lh[14] -
                lh[3] * lh[10] * lh[12] -
                lh[2] * lh[8]  * lh[15] -
                lh[0] * lh[11] * lh[14];
    cM[6] = -(  lh[0] * lh[6]  * lh[15] +
                lh[2] * lh[7]  * lh[12] +
                lh[3] * lh[4]  * lh[14] -
                lh[3] * lh[6]  * lh[12] -
                lh[2] * lh[4]  * lh[15] -
                lh[0] * lh[7]  * lh[14] );
    cM[7] =     lh[0] * lh[6]  * lh[11] +
                lh[2] * lh[7]  * lh[8]  +
                lh[3] * lh[4]  * lh[10] -
                lh[3] * lh[6]  * lh[8]  -
                lh[2] * lh[4]  * lh[11] -
                lh[0] * lh[7]  * lh[10];
    cM[8] =     lh[4] * lh[9]  * lh[15] +
                lh[5] * lh[11] * lh[12] +
                lh[7] * lh[8]  * lh[13] -
                lh[7] * lh[9]  * lh[12] -
                lh[5] * lh[8]  * lh[15] -
                lh[4] * lh[11] * lh[13];
    cM[9] = -(  lh[0] * lh[9]  * lh[15] +
                lh[1] * lh[11] * lh[12] +
                lh[3] * lh[8]  * lh[13] -
                lh[3] * lh[9]  * lh[12] -
                lh[1] * lh[8]  * lh[15] -
                lh[0] * lh[11] * lh[13] );
    cM[10] =    lh[0] * lh[5]  * lh[15] +
                lh[1] * lh[7]  * lh[12] +
                lh[3] * lh[4]  * lh[13] -
                lh[3] * lh[5]  * lh[12] -
                lh[1] * lh[4]  * lh[15] -
                lh[0] * lh[7]  * lh[13];
    cM[11] = -( lh[0] * lh[5]  * lh[11] +
                lh[1] * lh[7]  * lh[8]  +
                lh[3] * lh[4]  * lh[9]  -
                lh[3] * lh[5]  * lh[8]  -
                lh[1] * lh[4]  * lh[11] -
                lh[0] * lh[7]  * lh[9] );
    cM[12] = -( lh[4] * lh[9]  * lh[14] +
                lh[5] * lh[10] * lh[12] +
                lh[6] * lh[8]  * lh[13] -
                lh[6] * lh[9]  * lh[12] -
                lh[5] * lh[8]  * lh[14] -
                lh[4] * lh[10] * lh[13] ); 
    cM[13] =    lh[0] * lh[9]  * lh[14] +
                lh[1] * lh[10] * lh[12] +
                lh[2] * lh[8]  * lh[13] -
                lh[2] * lh[9]  * lh[12] -
                lh[1] * lh[8]  * lh[14] -
                lh[0] * lh[10] * lh[13];
    cM[14] = -( lh[0] * lh[5]  * lh[14] +
                lh[1] * lh[6]  * lh[12] +
                lh[2] * lh[4]  * lh[13] -
                lh[2] * lh[5]  * lh[12] -
                lh[1] * lh[4]  * lh[14] -
                lh[0] * lh[6]  * lh[13] );
    cM[15] =    lh[0] * lh[5]  * lh[10] +
                lh[1] * lh[6]  * lh[8]  +
                lh[2] * lh[4]  * lh[9]  -
                lh[2] * lh[5]  * lh[8]  -
                lh[1] * lh[4]  * lh[10] -
                lh[0] * lh[6]  * lh[9];

    return cM;
}


template<typename type>
type determinant(const mat4x4_t<type>& lh)
{
    return lh[0] * (    lh[5] * (lh[10] * lh[15] - lh[11] * lh[14]) -
                        lh[6] * (lh[9] * lh[15] - lh[11] * lh[13]) +
                        lh[7] * (lh[9] * lh[14] - lh[10] * lh[13])
                    ) -
          lh[1] * (     lh[4] * (lh[10] * lh[15] - lh[11] * lh[14]) -
                        lh[6] * (lh[8] * lh[15] - lh[11] * lh[12]) +
                        lh[7] * (lh[8] * lh[14] - lh[10] * lh[12])
                    ) +
          lh[2] * (     lh[4] * (lh[9] * lh[15] - lh[11] * lh[13]) -
                        lh[5] * (lh[8] * lh[15] - lh[11] * lh[12]) +
                        lh[7] * (lh[8] * lh[13] - lh[9] * lh[12])
                    ) -
          lh[3] * (     lh[4] * (lh[9] * lh[14] - lh[10] * lh[13]) -
                        lh[5] * (lh[8] * lh[14] - lh[10] * lh[12]) +
                        lh[6] * (lh[8] * lh[13] - lh[9] * lh[12]) );
}


template<typename type>
mat4x4_t<type> inverse(const mat4x4_t<type>& t)
{
    type det         = determinant<type>(t);
    type denom       = static_cast<type>(0);
    if (det == 0.f)
        return identity<type>();

    mat4x4_t<type> adj = adjugate<type>(t);

    denom = static_cast<type>(1) / det;
    return adj * denom;
}


template<typename type>
type dot(const vec3_t<type>& l, const vec3_t<type>& r)
{
    return (l[0] * r[0]) + (l[1] * r[1]) + (l[2] * r[2]);
}


template<typename type>
type dot(const vec4_t<type>& l, const vec4_t<type>& r)
{
    return (l[0] * r[0]) + (l[1] * r[1]) + (l[2] * r[2]) + (l[3] * r[3]);
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


template<typename type>
type clamp(type v, type mmin, type mmax)
{
    return ((v < mmin) ? mmin : (v > mmax ? mmax : v));
}


template<typename type>
vec2_t<type> clamp(const vec2_t<type>& v, const vec2_t<type>& mmin, const vec2_t<type>& mmax)
{
    return vec2_t<type>(clamp(v[0], mmin[0], mmax[0]), clamp(v[1], mmin[1], mmax[1]));
}


template<typename type>
vec3_t<type> clamp(const vec3_t<type>& v, const vec3_t<type>& mmin, const vec3_t<type>& mmax)
{
    return vec3_t<type>(clamp(v[0], mmin[0], mmax[0]), clamp(v[1], mmin[1], mmax[1]), clamp(v[2], mmin[2], mmax[2]));
}


template<typename type>
vec2_t<type> floor(const vec2_t<type>& o)
{
    return vec2_t<type>(floorf(o[0]), floorf(o[1]));
}


template<typename type>
vec3_t<type> floor(const vec3_t<type>& o)
{
    return vec3_t<type>(floorf(o[0]), floorf(o[1]), floorf(o[2]));
}
} // swrast