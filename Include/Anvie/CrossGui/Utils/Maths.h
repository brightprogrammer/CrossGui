/**
 * @file Maths.h
 * @date Wed, 15th May 2024
 * @author Siddharth Mishra (admin@brightprogrammer.in)
 * @copyright Copyright 2024 Siddharth Mishra
 * @copyright Copyright 2024 Anvie Labs
 *
 * Copyright 2024 Siddharth Mishra, Anvie Labs
 * 
 * Redistribution and use in source and binary forms, with or without modification, are permitted 
 * provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions
 *    and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions
 *    and the following disclaimer in the documentation and/or other materials provided with the
 *    distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse
 *    or promote products derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * */

#ifndef ANVIE_CROSSGUI_MATHS_H
#define ANVIE_CROSSGUI_MATHS_H

#include <Anvie/Types.h>

#if 0
#    define __MATH_TYPE_ALIGN__(N) __attribute__ ((aligned (N)))
#else
#    define __MATH_TYPE_ALIGN__(N)
#endif

typedef union __MATH_TYPE_ALIGN__ (8) Vec2f {
#define GEN_VEC2F(nx, ny)                                                                          \
    struct {                                                                                       \
        Float32 nx, ny;                                                                            \
    }

    GEN_VEC2F (x, y);
    GEN_VEC2F (u, v);
    GEN_VEC2F (width, height);

    Float32 data[2];

#undef GEN_VEC2F
} Vec2f;

Vec2f  *vec2f_add (Vec2f *res, Vec2f *vec1, Vec2f *vec2);
Vec2f  *vec2f_sub (Vec2f *res, Vec2f *vec1, Vec2f *vec2);
Vec2f  *vec2f_normalize (Vec2f *res, Vec2f *vec);
Vec2f  *vec2f_mul_f32 (Vec2f *res, Vec2f *vec, Float32 scalar);
Float32 vec2f_dot (Vec2f *vec1, Vec2f *vec2);
Float32 vec2f_norm (Vec2f *vec);

typedef union __MATH_TYPE_ALIGN__ (16) Vec3f {
#define GEN_VEC3F(nx, ny, nz)                                                                      \
    struct {                                                                                       \
        Float32 nx, ny, nz;                                                                        \
    };                                                                                             \
                                                                                                   \
    struct {                                                                                       \
        Vec2f   nx##ny;                                                                            \
        Float32 _ignore_##nz;                                                                      \
    };                                                                                             \
    struct {                                                                                       \
        Float32 _ignore_##nx;                                                                      \
        Vec2f   ny##nz;                                                                            \
    }

    GEN_VEC3F (x, y, z);
    GEN_VEC3F (r, g, b);
    GEN_VEC3F (width, height, depth);

    Float32 data[3];

#undef GEN_VEC3F
} Vec3f;

Vec3f  *vec3f_add (Vec3f *res, Vec3f *vec1, Vec3f *vec2);
Vec3f  *vec3f_sub (Vec3f *res, Vec3f *vec1, Vec3f *vec2);
Vec3f  *vec3f_cross (Vec3f *res, Vec3f *vec1, Vec3f *vec2);
Vec3f  *vec3f_normalize (Vec3f *res, Vec3f *vec);
Vec3f  *vec3f_mul_f32 (Vec3f *res, Vec3f *vec, Float32 scalar);
Float32 vec3f_dot (Vec3f *vec1, Vec3f *vec2);
Float32 vec3f_norm (Vec3f *vec);

typedef union __MATH_TYPE_ALIGN__ (16) Vec4f {
#define GEN_VEC4F(nx, ny, nz, nw)                                                                  \
    struct {                                                                                       \
        Float32 nx, ny, nz, nw;                                                                    \
    };                                                                                             \
                                                                                                   \
    struct {                                                                                       \
        Vec3f   nx##ny;                                                                            \
        Float32 _ignore2_##nz##nw;                                                                 \
    };                                                                                             \
    struct {                                                                                       \
        Float32 _ignore2_##nx;                                                                     \
        Vec3f   ny##nz;                                                                            \
        Float32 _ignore2_##nw;                                                                     \
    };                                                                                             \
    struct {                                                                                       \
        Float32 _ignore2_##nx##ny;                                                                 \
        Vec3f   nz##nw;                                                                            \
    };                                                                                             \
                                                                                                   \
    struct {                                                                                       \
        Vec3f   nx##ny##nz;                                                                        \
        Float32 _ignore_##nw;                                                                      \
    };                                                                                             \
    struct {                                                                                       \
        Float32 _ignore_##nx;                                                                      \
        Vec3f   ny##nz##nw;                                                                        \
    }

    GEN_VEC4F (x, y, z, w);
    GEN_VEC4F (r, g, b, a);

    Float32 data[4];

#undef GEN_VEC4F
} Vec4f;

Vec4f  *vec4f_add (Vec4f *res, Vec4f *vec1, Vec4f *vec2);
Vec4f  *vec4f_sub (Vec4f *res, Vec4f *vec1, Vec4f *vec2);
Vec4f  *vec4f_normalize (Vec4f *res, Vec4f *vec);
Vec4f  *vec4f_mul_f32 (Vec4f *res, Vec4f *vec, Float32 scalar);
Float32 vec4f_dot (Vec4f *vec1, Vec4f *vec2);
Float32 vec4f_norm (Vec4f *vec);

typedef union Mat2f {
    Vec2f   column[2];
    Float32 elem[2][2];
} Mat2f;

Mat2f  *mat2f_add (Mat2f *res, Mat2f *mat1, Mat2f *mat2);
Mat2f  *mat2f_sub (Mat2f *res, Mat2f *mat1, Mat2f *mat2);
Mat2f  *mat2f_mul (Mat2f *res, Mat2f *mat1, Mat2f *mat2);
Mat2f  *mat2f_transpose (Mat2f *res, Mat2f *mat);
Mat2f  *mat2f_invert (Mat2f *res, Mat2f *mat);
Vec2f  *mat2f_mul_vec (Vec2f *res, Mat2f *mat, Vec2f *vec);
Mat2f  *mat2f_mul_f32 (Mat2f *res, Mat2f *mat, Float32 scale);
Float32 mat2f_det (Mat2f *mat);

typedef union Mat3f {
    Vec3f   column[3];
    Float32 elem[3][3];
} Mat3f;

Mat3f  *mat3f_add (Mat3f *res, Mat3f *mat1, Mat3f *mat2);
Mat3f  *mat3f_sub (Mat3f *res, Mat3f *mat1, Mat3f *mat2);
Mat3f  *mat3f_mul (Mat3f *res, Mat3f *mat1, Mat3f *mat2);
Mat3f  *mat3f_transpose (Mat3f *res, Mat3f *mat);
Mat3f  *mat3f_invert (Mat3f *res, Mat3f *mat);
Vec3f  *mat3f_mul_vec (Vec3f *res, Mat3f *mat, Vec3f *vec);
Mat3f  *mat3f_mul_f32 (Mat3f *res, Mat3f *mat, Float32 scale);
Float32 mat3f_det (Mat3f *mat);

typedef union Mat4f {
    Vec4f   column[4];
    Float32 elem[4][4];
} Mat4f;

Mat4f  *mat4f_add (Mat4f *res, Mat4f *mat1, Mat4f *mat2);
Mat4f  *mat4f_sub (Mat4f *res, Mat4f *mat1, Mat4f *mat2);
Mat4f  *mat4f_mul (Mat4f *res, Mat4f *mat1, Mat4f *mat2);
Mat4f  *mat4f_transpose (Mat4f *res, Mat4f *mat);
Mat4f  *mat4f_invert (Mat4f *res, Mat4f *mat);
Vec4f  *mat4f_mul_vec (Vec4f *res, Mat4f *mat, Vec4f *vec);
Mat4f  *mat4f_mul_f32 (Mat4f *res, Mat4f *mat, Float32 scale);
Float32 mat4f_det (Mat4f *mat);

#undef __MATH_TYPE_ALIGN__

#endif // ANVIE_CROSSGUI_MATHS_H
