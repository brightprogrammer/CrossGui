/**
 * @file Maths.c
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

#include <Anvie/Common.h>
#include <Anvie/CrossGui/Utils/Maths.h>

/* libm */
#include <math.h>

/**************************************************************************************************/
/********************************************* VEC 2F *********************************************/
/**************************************************************************************************/

Vec2f *vec2f_add (Vec2f *res, Vec2f *vec1, Vec2f *vec2) {
    RETURN_VALUE_IF (!res || !vec1 || !vec2, Null, ERR_INVALID_ARGUMENTS);

    *res = (Vec2f) {.x = vec1->x + vec2->x, .y = vec1->y + vec2->y};
    return res;
}

Vec2f *vec2f_sub (Vec2f *res, Vec2f *vec1, Vec2f *vec2) {
    RETURN_VALUE_IF (!res || !vec1 || !vec2, Null, ERR_INVALID_ARGUMENTS);

    *res = (Vec2f) {.x = vec1->x - vec2->x, .y = vec1->y - vec2->y};
    return res;
}

Vec2f *vec2f_normalize (Vec2f *res, Vec2f *vec) {
    RETURN_VALUE_IF (!res || !vec, Null, ERR_INVALID_ARGUMENTS);

    Float32 norm = vec2f_norm (vec);
    RETURN_VALUE_IF (norm == 0.f, Null, "Divide by zero error : vector norm is zero.\n");
    *res = (Vec2f) {.x = vec->x / norm, .y = vec->y / norm};
    return res;
}

Vec2f *vec2f_mul_f32 (Vec2f *res, Vec2f *vec, Float32 scalar) {
    RETURN_VALUE_IF (!res || !vec, Null, ERR_INVALID_ARGUMENTS);

    *res = (Vec2f) {.x = vec->x * scalar, .y = vec->y * scalar};
    return res;
}

Float32 vec2f_dot (Vec2f *vec1, Vec2f *vec2) {
    RETURN_VALUE_IF (!vec1 || !vec2, 0, ERR_INVALID_ARGUMENTS);

    Float32 dot = vec1->x * vec2->x + vec1->y * vec2->y;
    return dot;
}

Float32 vec2f_norm (Vec2f *vec) {
    RETURN_VALUE_IF (!vec, 0, ERR_INVALID_ARGUMENTS);

    Float32 norm = sqrtf (vec->x * vec->x + vec->y * vec->y);
    return norm;
}

/**************************************************************************************************/
/********************************************* VEC 3F *********************************************/
/**************************************************************************************************/

Vec3f *vec3f_add (Vec3f *res, Vec3f *vec1, Vec3f *vec2) {
    RETURN_VALUE_IF (!res || !vec1 || !vec2, Null, ERR_INVALID_ARGUMENTS);

    *res = (Vec3f) {.x = vec1->x + vec2->x, .y = vec1->y + vec2->y, .z = vec1->z + vec2->z};
    return res;
}

Vec3f *vec3f_sub (Vec3f *res, Vec3f *vec1, Vec3f *vec2) {
    RETURN_VALUE_IF (!res || !vec1 || !vec2, Null, ERR_INVALID_ARGUMENTS);

    *res = (Vec3f) {.x = vec1->x - vec2->x, .y = vec1->y - vec2->y, .z = vec1->z - vec2->z};
    return res;
}

Vec3f *vec3f_cross (Vec3f *res, Vec3f *vec1, Vec3f *vec2) {
    RETURN_VALUE_IF (!res || !vec1 || !vec2, Null, ERR_INVALID_ARGUMENTS);

    *res = (Vec3f
    ) {.x = vec1->y * vec2->z - vec1->z * vec2->y,
       .y = -vec1->x * vec2->z + vec1->z * vec2->x,
       .z = vec1->x * vec2->y - vec1->y * vec2->x};
    return res;
}

Vec3f *vec3f_normalize (Vec3f *res, Vec3f *vec) {
    RETURN_VALUE_IF (!res || !vec, Null, ERR_INVALID_ARGUMENTS);

    Float32 norm = vec3f_norm (vec);
    RETURN_VALUE_IF (norm == 0.f, Null, "Divide by zero error : vector norm is zero.\n");
    *res = (Vec3f) {.x = vec->x / norm, .y = vec->y / norm, .z = vec->z / norm};
    return res;
}

Vec3f *vec3f_mul_f32 (Vec3f *res, Vec3f *vec, Float32 scalar) {
    RETURN_VALUE_IF (!res || !vec, Null, ERR_INVALID_ARGUMENTS);

    *res = (Vec3f) {.x = vec->x * scalar, .y = vec->y * scalar, .z = vec->z * scalar};
    return res;
}

Float32 vec3f_dot (Vec3f *vec1, Vec3f *vec2) {
    RETURN_VALUE_IF (!vec1 || !vec2, 0, ERR_INVALID_ARGUMENTS);

    Float32 dot = vec1->x * vec2->x + vec1->y * vec2->y + vec1->z * vec2->z;
    return dot;
}

Float32 vec3f_norm (Vec3f *vec) {
    RETURN_VALUE_IF (!vec, 0, ERR_INVALID_ARGUMENTS);

    Float32 norm = sqrtf (vec->x * vec->x + vec->y * vec->y + vec->z * vec->z);
    return norm;
}

/**************************************************************************************************/
/********************************************* VEC 4F *********************************************/
/**************************************************************************************************/

Vec4f *vec4f_add (Vec4f *res, Vec4f *vec1, Vec4f *vec2) {
    RETURN_VALUE_IF (!res || !vec1 || !vec2, Null, ERR_INVALID_ARGUMENTS);

    *res = (Vec4f
    ) {.x = vec1->x + vec2->x,
       .y = vec1->y + vec2->y,
       .z = vec1->z + vec2->z,
       .w = vec1->w + vec2->w};
    return res;
}

Vec4f *vec4f_sub (Vec4f *res, Vec4f *vec1, Vec4f *vec2) {
    RETURN_VALUE_IF (!res || !vec1 || !vec2, Null, ERR_INVALID_ARGUMENTS);

    *res = (Vec4f
    ) {.x = vec1->x - vec2->x,
       .y = vec1->y - vec2->y,
       .z = vec1->z - vec2->z,
       .w = vec1->w - vec2->w};
    return res;
}

Vec4f *vec4f_normalize (Vec4f *res, Vec4f *vec) {
    RETURN_VALUE_IF (!res || !vec, Null, ERR_INVALID_ARGUMENTS);

    Float32 norm = vec4f_norm (vec);
    RETURN_VALUE_IF (norm == 0.f, Null, "Divide by zero error : vector norm is zero.\n");
    *res = (Vec4f) {.x = vec->x / norm, .y = vec->y / norm, .z = vec->z / norm, .w = vec->w / norm};
    return res;
}

Vec4f *vec4f_mul_f32 (Vec4f *res, Vec4f *vec, Float32 scalar) {
    RETURN_VALUE_IF (!res || !vec, Null, ERR_INVALID_ARGUMENTS);

    *res = (Vec4f
    ) {.x = vec->x * scalar, .y = vec->y * scalar, .z = vec->z * scalar, .w = vec->w * scalar};
    return res;
}

Float32 vec4f_dot (Vec4f *vec1, Vec4f *vec2) {
    RETURN_VALUE_IF (!vec1 || !vec2, 0, ERR_INVALID_ARGUMENTS);

    Float32 dot = vec1->x * vec2->x + vec1->y * vec2->y + vec1->z * vec2->z + vec1->w * vec2->w;
    return dot;
}

Float32 vec4f_norm (Vec4f *vec) {
    RETURN_VALUE_IF (!vec, 0, ERR_INVALID_ARGUMENTS);

    Float32 norm = sqrtf (vec->x * vec->x + vec->y * vec->y + vec->z * vec->z + vec->w * vec->w);
    return norm;
}

/**************************************************************************************************/
/********************************************* MAT 2F ************************* ********************/
/**************************************************************************************************/

#define GET_ELEM(M, r, c) (M->elem[c][r])
#define RES(r, c)         GET_ELEM (res, r, c)
#define M1(r, c)          GET_ELEM (mat1, r, c)
#define M2(r, c)          GET_ELEM (mat2, r, c)
#define ROW(M, r)                                                                                  \
    (Vec2f[]) {                                                                                    \
        { .x = GET_ELEM (M, r, 0), .y = GET_ELEM (M, r, 1) }                                       \
    }
#define COL(M, c) (M->column + c)

/*
 * Layout of Mat2f add opeartion 
 *
 * +-------+-------+     +-------+-------+     +--------+--------+     +---------------+---------------+ 
 * |       |       |     |       |       |     |        |        |     |               |               | 
 * | V1_00 | V1_01 |     | V2_00 | V2_01 |     | RES_00 | RES_01 |     | V1_00 + V2_00 | V1_01 + v2_01 | 
 * |       |       |  +  |       |       |  =  |        |        |  =  |               |               | 
 * | V1_10 | V1_11 |     | V2_10 | V2_11 |     | RES_10 | RES_11 |     | V1_10 + V2_10 | V1_11 + V2_11 | 
 * |       |       |     |       |       |     |        |        |     |               |               | 
 * +-------+-------+     +-------+-------+     +--------+--------+     +---------------+---------------+ 
 *
 * */

Mat2f *mat2f_add (Mat2f *res, Mat2f *mat1, Mat2f *mat2) {
    RETURN_VALUE_IF (!res || !mat1 || !mat2, Null, ERR_INVALID_ARGUMENTS);

    RETURN_VALUE_IF (
        (!vec2f_add (COL (res, 0), COL (mat1, 0), COL (mat2, 0)) ||
         !vec2f_add (COL (res, 1), COL (mat1, 1), COL (mat2, 1))),
        Null,
        "Addition operation failed"
    );

    return res;
}

Mat2f *mat2f_sub (Mat2f *res, Mat2f *mat1, Mat2f *mat2) {
    RETURN_VALUE_IF (!res || !mat1 || !mat2, Null, ERR_INVALID_ARGUMENTS);

    RETURN_VALUE_IF (
        (!vec2f_sub (COL (res, 0), COL (mat1, 0), COL (mat2, 0)) ||
         !vec2f_sub (COL (res, 1), COL (mat1, 1), COL (mat2, 1))),
        Null,
        "Subtraction operation failed"
    );

    return res;
}

/*
 * Layout of Mat2f mul opeartion
 *
 *          COL0   COL1
 *       +-------+-------+     +-------+-------+     +--------+--------+     +-------------------------------------+-------------------------------------+ 
 *       |       |       |     |       |       |     |        |        |     |                                     |                                     | 
 *  ROW0 | V1_00 | V1_01 |     | V2_00 | V2_01 |     | RES_00 | RES_01 |     | dot({v1_00, v1_01}, {v2_00, v2_10}) | dot({v1_00, v1_01}, {v2_01, v2_11}) |
 *       |       |       |     |       |       |     |        |        |     |                                     |                                     |
 *       +-------+-------+  *  +-------+-------+  =  +--------+--------+  =  +-------------------------------------+-------------------------------------+ 
 *       |       |       |     |       |       |     |        |        |     |                                     |                                     |
 *  ROW1 | V1_10 | V1_11 |     | V2_10 | V2_11 |     | RES_10 | RES_11 |     | dot({v1_10, v1_11}, {v2_00, v2_10}) | dot({v1_10, v1_11}, {v2_01, v2_11}) |
 *       |       |       |     |       |       |     |        |        |     |                                     |                                     |
 *       +-------+-------+     +-------+-------+     +--------+--------+     +-------------------------------------+-------------------------------------+ 
 *
 * */

Mat2f *mat2f_mul (Mat2f *res, Mat2f *mat1, Mat2f *mat2) {
    RETURN_VALUE_IF (!res || !mat1 || !mat2, Null, ERR_INVALID_ARGUMENTS);

    /* HACK: clang format you bitch! */

    /* clang-format off */
    *res = (Mat2f) {
        .column = {
            {
                .x = vec2f_dot (ROW (mat1, 0), COL (mat2, 0)),
                .y = vec2f_dot (ROW (mat1, 1), COL (mat2, 0))
            },
            {
                .x = vec2f_dot (ROW (mat1, 0), COL (mat2, 1)),
                .y = vec2f_dot (ROW (mat1, 1), COL (mat2, 1))
            }
        }
    };
    /* clang-format on */

    return res;
}

/*
 * Layout of Mat2f transpose opeartion
 *
 * +-------+-------+ T    +--------+--------+     +-------+-------+ 
 * |       |       |      |        |        |     |       |       | 
 * | V1_00 | V1_01 |      | RES_00 | RES_01 |     | V1_00 | V1_10 | 
 * |       |       |      |        |        |     |       |       |
 * +-------+-------+   =  +--------+--------+  =  +-------+-------+ 
 * |       |       |      |        |        |     |       |       |
 * | V1_10 | V1_11 |      | RES_10 | RES_11 |     | V1_01 | V1_11 |
 * |       |       |      |        |        |     |       |       |
 * +-------+-------+      +--------+--------+     +-------+-------+ 
 *
 * */

Mat2f *mat2f_transpose (Mat2f *res, Mat2f *mat) {
    RETURN_VALUE_IF (!res || !mat, Null, ERR_INVALID_ARGUMENTS);

    Mat2f *mat1 = mat;

    *res = (Mat2f) {
        .column = {{.x = M1 (0, 0), .y = M1 (0, 1)}, {.x = M1 (1, 0), .y = M1 (1, 1)}}
    };

    return res;
}

/*
 * Layout of Mat2f inverse opeartion                    ( transpose of  )
 *                                                      (cofactor matrix)             (adjoint matrix)
 * +-------+-------+ -1   +--------+--------+          +--------+--------+ T         +--------+--------+
 * |       |       |      |        |        |          |        |        |           |        |        | 
 * | V1_00 | V1_01 |      | RES_00 | RES_01 |          |  V1_11 | -V1_10 |           |  V1_11 | -V1_01 | 
 * |       |       |      |        |        |     1    |        |        |      1    |        |        |
 * +-------+-------+   =  +--------+--------+  = --- * +--------+--------+   = --- * +--------+--------+ 
 * |       |       |      |        |        |    det   |        |        |     det   |        |        |
 * | V1_10 | V1_11 |      | RES_10 | RES_11 |          | -V1_01 |  V1_00 |           | -V1_10 |  V1_00 |
 * |       |       |      |        |        |          |        |        |           |        |        |
 * +-------+-------+      +--------+--------+          +--------+--------+           +--------+--------+ 
 *
 * */


Mat2f *mat2f_invert (Mat2f *res, Mat2f *mat) {
    RETURN_VALUE_IF (!res || !mat, Null, ERR_INVALID_ARGUMENTS);

    Float32 det = mat2f_det (mat);
    RETURN_VALUE_IF (
        det == 0.f,
        Null,
        "Division by zero : determinant of given matrix is zero. Not Invertible.\n"
    );

    Mat2f *mat1 = mat;

    RES (0, 0) = M1 (1, 1) / det;
    RES (1, 0) = -M1 (1, 0) / det;
    RES (0, 1) = -M1 (0, 1) / det;
    RES (1, 1) = M1 (0, 0) / det;

    return res;
}

Vec2f *mat2f_mul_vec (Vec2f *res, Mat2f *mat, Vec2f *vec) {
    RETURN_VALUE_IF (!res || !mat || !vec, Null, ERR_INVALID_ARGUMENTS);

    *res = (Vec2f) {.x = vec2f_dot (ROW (mat, 0), vec), .y = vec2f_dot (ROW (mat, 1), vec)};

    return res;
}

Mat2f *mat2f_mul_f32 (Mat2f *res, Mat2f *mat, Float32 scale) {
    RETURN_VALUE_IF (!res || !mat, Null, ERR_INVALID_ARGUMENTS);

    RETURN_VALUE_IF (
        (!vec2f_mul_f32 (COL (res, 0), COL (mat, 0), scale) ||
         !vec2f_mul_f32 (COL (res, 1), COL (mat, 1), scale)),
        Null,
        "Failed to scale given matrix\n"
    );

    return res;
}

Float32 mat2f_det (Mat2f *mat) {
    RETURN_VALUE_IF (!mat, 0, ERR_INVALID_ARGUMENTS);

    Mat2f *mat1 = mat;
    return M1 (0, 0) * M1 (1, 1) - M1 (0, 1) * M1 (1, 0);
}

#undef ROW

/**************************************************************************************************/
/********************************************* MAT 3F *********************************************/
/**************************************************************************************************/

#define ROW(M, r)                                                                                  \
    (Vec3f[]) {                                                                                    \
        { .x = GET_ELEM (M, r, 0), .y = GET_ELEM (M, r, 1), .z = GET_ELEM (M, r, 2) }              \
    }

Mat3f *mat3f_add (Mat3f *res, Mat3f *mat1, Mat3f *mat2) {
    RETURN_VALUE_IF (!res || !mat1 || !mat2, Null, ERR_INVALID_ARGUMENTS);

    RETURN_VALUE_IF (
        (!vec3f_add (COL (res, 0), COL (mat1, 0), COL (mat2, 0)) ||
         !vec3f_add (COL (res, 1), COL (mat1, 1), COL (mat2, 1)) ||
         !vec3f_add (COL (res, 2), COL (mat1, 2), COL (mat2, 2))),
        Null,
        "Addition operation failed"
    );

    return res;
}

Mat3f *mat3f_sub (Mat3f *res, Mat3f *mat1, Mat3f *mat2) {
    RETURN_VALUE_IF (!res || !mat1 || !mat2, Null, ERR_INVALID_ARGUMENTS);

    RETURN_VALUE_IF (
        (!vec3f_sub (COL (res, 0), COL (mat1, 0), COL (mat2, 0)) ||
         !vec3f_sub (COL (res, 1), COL (mat1, 1), COL (mat2, 1)) ||
         !vec3f_sub (COL (res, 2), COL (mat1, 2), COL (mat2, 2))),
        Null,
        "Subtraction operation failed"
    );

    return res;
}

Mat3f *mat3f_mul (Mat3f *res, Mat3f *mat1, Mat3f *mat2) {
    RETURN_VALUE_IF (!res || !mat1 || !mat2, Null, ERR_INVALID_ARGUMENTS);

    /* clang-format off */
    *res = (Mat3f) {
        .column = {
            {
                .x = vec3f_dot (ROW (mat1, 0), COL (mat2, 0)),
                .y = vec3f_dot (ROW (mat1, 1), COL (mat2, 0)),
                .z = vec3f_dot (ROW (mat1, 2), COL (mat2, 0))
            }, 
            {
                .x = vec3f_dot (ROW (mat1, 0), COL (mat2, 1)),
                .y = vec3f_dot (ROW (mat1, 1), COL (mat2, 1)),
                .z = vec3f_dot (ROW (mat1, 2), COL (mat2, 1))
            },
            {
                .x = vec3f_dot (ROW (mat1, 0), COL (mat2, 2)),
                .y = vec3f_dot (ROW (mat1, 1), COL (mat2, 2)),
                .z = vec3f_dot (ROW (mat1, 2), COL (mat2, 2))
            }
        }
    };
    /* clang-format on */

    return res;
}

Mat3f *mat3f_transpose (Mat3f *res, Mat3f *mat) {
    RETURN_VALUE_IF (!res || !mat, Null, ERR_INVALID_ARGUMENTS);

    Mat3f *mat1 = mat;

    /* clang-format off */
    *res = (Mat3f) {
        .column = {
            {.x = M1 (0, 0), .y = M1 (0, 1), .z = M1 (0, 2)},
            {.x = M1 (1, 0), .y = M1 (1, 1), .z = M1 (1, 2)},
            {.x = M1 (2, 0), .y = M1 (2, 1), .z = M1 (2, 2)},
        }
    };
    /* clang-format on */

    return res;
}

Mat3f *mat3f_invert (Mat3f *res, Mat3f *mat) {
    RETURN_VALUE_IF (!res || !mat, Null, ERR_INVALID_ARGUMENTS);

    Float32 det = mat3f_det (mat);
    RETURN_VALUE_IF (
        det == 0.f,
        Null,
        "Division by zero : determinant of given matrix is zero. Not Invertible.\n"
    );

    Mat3f *mat1 = mat;

    RETURN_VALUE_IF (
        (!vec3f_cross (COL (res, 0), COL (mat1, 1), COL (mat1, 2)) ||
         !vec3f_cross (COL (res, 1), COL (mat1, 2), COL (mat1, 0)) ||
         !vec3f_cross (COL (res, 2), COL (mat1, 0), COL (mat1, 1))),
        Null,
        "Failed to compute cofactor matrix.\n"
    );

    return mat3f_transpose (res, mat3f_mul_f32 (res, res, 1.f / det));
}

Vec3f *mat3f_mul_vec (Vec3f *res, Mat3f *mat, Vec3f *vec) {
    RETURN_VALUE_IF (!res || !mat || !vec, Null, ERR_INVALID_ARGUMENTS);

    *res = (Vec3f
    ) {.x = vec3f_dot (ROW (mat, 0), vec),
       .y = vec3f_dot (ROW (mat, 1), vec),
       .z = vec3f_dot (ROW (mat, 2), vec)};

    return res;
}

Mat3f *mat3f_mul_f32 (Mat3f *res, Mat3f *mat, Float32 scale) {
    RETURN_VALUE_IF (!res || !mat, Null, ERR_INVALID_ARGUMENTS);

    RETURN_VALUE_IF (
        (!vec3f_mul_f32 (COL (res, 0), COL (mat, 0), scale) ||
         !vec3f_mul_f32 (COL (res, 1), COL (mat, 1), scale) ||
         !vec3f_mul_f32 (COL (res, 2), COL (mat, 2), scale)),
        Null,
        "Failed to scale given matrix\n"
    );

    return res;
}

Float32 mat3f_det (Mat3f *mat) {
    RETURN_VALUE_IF (!mat, 0, ERR_INVALID_ARGUMENTS);

    Mat3f *mat1 = mat;
    return M1 (0, 0) * (M1 (1, 1) * M1 (2, 2) - M1 (1, 2) * M1 (2, 1)) -
           M1 (0, 1) * (M1 (1, 0) * M1 (2, 2) - M1 (1, 2) * M1 (2, 0)) +
           M1 (0, 2) * (M1 (1, 0) * M1 (2, 1) - M1 (1, 1) * M1 (2, 0));
}

#undef ROW

/**************************************************************************************************/
/********************************************* MAT 4F *********************************************/
/**************************************************************************************************/

#define ROW(M, r)                                                                                  \
    (Vec4f[]) {                                                                                    \
        {                                                                                          \
            .x = GET_ELEM (M, r, 0), .y = GET_ELEM (M, r, 1), .z = GET_ELEM (M, r, 2),             \
            .w = GET_ELEM (M, r, 3)                                                                \
        }                                                                                          \
    }

Mat4f *mat4f_add (Mat4f *res, Mat4f *mat1, Mat4f *mat2) {
    RETURN_VALUE_IF (!res || !mat1 || !mat2, Null, ERR_INVALID_ARGUMENTS);

    RETURN_VALUE_IF (
        (!vec4f_add (COL (res, 0), COL (mat1, 0), COL (mat2, 0)) ||
         !vec4f_add (COL (res, 1), COL (mat1, 1), COL (mat2, 1)) ||
         !vec4f_add (COL (res, 2), COL (mat1, 2), COL (mat2, 2)) ||
         !vec4f_add (COL (res, 3), COL (mat1, 3), COL (mat2, 3))),
        Null,
        "Addition operation failed"
    );

    return res;
}

Mat4f *mat4f_sub (Mat4f *res, Mat4f *mat1, Mat4f *mat2) {
    RETURN_VALUE_IF (!res || !mat1 || !mat2, Null, ERR_INVALID_ARGUMENTS);

    RETURN_VALUE_IF (
        (!vec4f_sub (COL (res, 0), COL (mat1, 0), COL (mat2, 0)) ||
         !vec4f_sub (COL (res, 1), COL (mat1, 1), COL (mat2, 1)) ||
         !vec4f_sub (COL (res, 2), COL (mat1, 2), COL (mat2, 2)) ||
         !vec4f_sub (COL (res, 3), COL (mat1, 3), COL (mat2, 3))),
        Null,
        "Subtraction operation failed"
    );

    return res;
}

Mat4f *mat4f_mul (Mat4f *res, Mat4f *mat1, Mat4f *mat2) {
    RETURN_VALUE_IF (!res || !mat1 || !mat2, Null, ERR_INVALID_ARGUMENTS);

    /* clang-format off */
    *res = (Mat4f) {
        .column = {
            {
                .x = vec4f_dot (ROW (mat1, 0), COL (mat2, 0)),
                .y = vec4f_dot (ROW (mat1, 1), COL (mat2, 0)),
                .z = vec4f_dot (ROW (mat1, 2), COL (mat2, 0)),
                .w = vec4f_dot (ROW (mat1, 3), COL (mat2, 0))
            }, 
            {
                .x = vec4f_dot (ROW (mat1, 0), COL (mat2, 1)),
                .y = vec4f_dot (ROW (mat1, 1), COL (mat2, 1)),
                .z = vec4f_dot (ROW (mat1, 2), COL (mat2, 1)),
                .w = vec4f_dot (ROW (mat1, 3), COL (mat2, 1))
            },
            {
                .x = vec4f_dot (ROW (mat1, 0), COL (mat2, 2)),
                .y = vec4f_dot (ROW (mat1, 1), COL (mat2, 2)),
                .z = vec4f_dot (ROW (mat1, 2), COL (mat2, 2)),
                .w = vec4f_dot (ROW (mat1, 3), COL (mat2, 2))
            },
            {
                .x = vec4f_dot (ROW (mat1, 0), COL (mat2, 3)),
                .y = vec4f_dot (ROW (mat1, 1), COL (mat2, 3)),
                .z = vec4f_dot (ROW (mat1, 2), COL (mat2, 3)),
                .w = vec4f_dot (ROW (mat1, 3), COL (mat2, 3))
            }

        }
    };
    /* clang-format on */

    return res;
}

Mat4f *mat4f_transpose (Mat4f *res, Mat4f *mat) {
    RETURN_VALUE_IF (!res || !mat, Null, ERR_INVALID_ARGUMENTS);

    Mat4f *mat1 = mat;

    /* clang-format off */
    *res = (Mat4f) {
        .column = {
            {.x = M1 (0, 0), .y = M1 (0, 1), .z = M1 (0, 2), .w = M1 (0, 3)},
            {.x = M1 (1, 0), .y = M1 (1, 1), .z = M1 (1, 2), .w = M1 (1, 3)},
            {.x = M1 (2, 0), .y = M1 (2, 1), .z = M1 (2, 2), .w = M1 (2, 3)},
            {.x = M1 (3, 0), .y = M1 (3, 1), .z = M1 (3, 2), .w = M1 (3, 3)},
        }
    };
    /* clang-format on */

    return res;
}

Mat4f *mat4f_invert (Mat4f *res, Mat4f *mat) {
    RETURN_VALUE_IF (!res || !mat, Null, ERR_INVALID_ARGUMENTS);

    /* following algorithm is referenced from here :
     * https://github.com/HandmadeMath/HandmadeMath/blob/43836aa3a6226682b594a6ba818baf16c0538b81/HandmadeMath.h#L1701 
     * 
     * I used this one because it computes determinant while computing inverse,
     * means it's reusing precomputed data. Haven't tested it though for FLOP count.
     * */

    Vec3f C01, C23, B10, B32, T0, T1;
    RETURN_VALUE_IF (
        (!vec3f_cross (&C01, &COL (mat, 0)->xyz, &COL (mat, 1)->xyz) ||
         !vec3f_cross (&C23, &COL (mat, 2)->xyz, &COL (mat, 3)->xyz) ||
         !vec3f_sub (
             &B10,
             vec3f_mul_f32 (&T0, &COL (mat, 0)->xyz, COL (mat, 1)->w),
             vec3f_mul_f32 (&T1, &COL (mat, 1)->xyz, COL (mat, 0)->w)
         ) ||
         !vec3f_sub (
             &B32,
             vec3f_mul_f32 (&T0, &COL (mat, 2)->xyz, COL (mat, 3)->w),
             vec3f_mul_f32 (&T1, &COL (mat, 3)->xyz, COL (mat, 2)->w)
         )),
        Null,
        "Failed to compute determinant.\n"
    );

    Float32 det = vec3f_dot (&C01, &B32) + vec3f_dot (&C23, &B10);
    RETURN_VALUE_IF (
        det == 0.f,
        Null,
        "Division by zero : determinant of given matrix is zero. Not Invertible. Failure in step "
        "1.\n"
    );

    Float32 inv_det = 1 / det;

    RETURN_VALUE_IF (
        (!vec3f_mul_f32 (&C01, &C01, inv_det) || !vec3f_mul_f32 (&C23, &C23, inv_det) ||
         !vec3f_mul_f32 (&B10, &B10, inv_det) || !vec3f_mul_f32 (&B32, &B32, inv_det)),
        Null,
        "Failed to compute determinant. Failure in step 2\n"
    );

    /* clang-format off */
    /* NOTE: here I'm not checking whether add op will fail or not, assuming it'll go smoothly */
    return mat4f_transpose (
        res,
        &(Mat4f) {
            .column = {
                {
                    .xyz = *vec3f_add (
                                &T0,
                                vec3f_cross (&T0, &COL (mat, 1)->xyz, &B32),
                                vec3f_mul_f32 (&T1, &C23, COL (mat, 1)->w)
                            ),
                     ._ignore_w = -vec3f_dot (&COL (mat, 1)->xyz, &C23)
                },
                {
                    .xyz = *vec3f_sub (
                                &T0,
                                vec3f_cross (&T0, &B32, &COL (mat, 0)->xyz),
                                vec3f_mul_f32 (&T1, &C23, COL (mat, 0)->w)
                            ),
                     ._ignore_w = -vec3f_dot (&COL (mat, 0)->xyz, &C23)
                },
                {
                    .xyz = *vec3f_add (
                                &T0,
                                vec3f_cross (&T0, &COL (mat, 3)->xyz, &B10),
                                vec3f_mul_f32 (&T1, &C01, COL (mat, 3)->w)
                            ),
                     ._ignore_w = -vec3f_dot (&COL (mat, 3)->xyz, &C01)
                },
                {
                    .xyz = *vec3f_add (
                                &T0,
                                vec3f_cross (&T0, &B10, &COL (mat, 2)->xyz),
                                vec3f_mul_f32 (&T1, &C23, COL (mat, 2)->w)
                            ),
                     ._ignore_w = -vec3f_dot (&COL (mat, 2)->xyz, &C01)
                },
            }
        }
    );
    /* clang-format on */

    return res;
}

Vec4f *mat4f_mul_vec (Vec4f *res, Mat4f *mat, Vec4f *vec) {
    RETURN_VALUE_IF (!res || !mat || !vec, Null, ERR_INVALID_ARGUMENTS);

    *res = (Vec4f
    ) {.x = vec4f_dot (ROW (mat, 0), vec),
       .y = vec4f_dot (ROW (mat, 1), vec),
       .z = vec4f_dot (ROW (mat, 2), vec),
       .w = vec4f_dot (ROW (mat, 3), vec)};

    return res;
}

Mat4f *mat4f_mul_f32 (Mat4f *res, Mat4f *mat, Float32 scale) {
    RETURN_VALUE_IF (!res || !mat, Null, ERR_INVALID_ARGUMENTS);

    RETURN_VALUE_IF (
        (!vec4f_mul_f32 (COL (res, 0), COL (mat, 0), scale) ||
         !vec4f_mul_f32 (COL (res, 1), COL (mat, 1), scale) ||
         !vec4f_mul_f32 (COL (res, 2), COL (mat, 2), scale) ||
         !vec4f_mul_f32 (COL (res, 3), COL (mat, 3), scale)),
        Null,
        "Failed to scale given matrix\n"
    );

    return res;
}

Float32 mat4f_det (Mat4f *mat) {
    RETURN_VALUE_IF (!mat, 0, ERR_INVALID_ARGUMENTS);

    Mat4f *mat1 = mat;

    /* Mij represents the matrix generated by skipping i-th and j-th column,
     * meaning i-1 and j-1 indices columns and rows respectively. */

    Mat3f M11 = {
        {COL (mat, 1)->yzw, COL (mat, 2)->yzw, COL (mat, 3)->yzw}
    };

    Mat3f M21 = {
        {(Vec3f) {.x = M1 (0, 1), .y = M1 (2, 1), .z = M1 (3, 1)},
         (Vec3f) {.x = M1 (0, 2), .y = M1 (2, 2), .z = M1 (3, 2)},
         (Vec3f) {.x = M1 (0, 3), .y = M1 (2, 3), .z = M1 (3, 3)}}
    };

    Mat3f M31 = {
        {(Vec3f) {.x = M1 (0, 1), .y = M1 (1, 1), .z = M1 (3, 1)},
         (Vec3f) {.x = M1 (0, 2), .y = M1 (1, 2), .z = M1 (3, 2)},
         (Vec3f) {.x = M1 (0, 3), .y = M1 (1, 3), .z = M1 (3, 3)}}
    };

    Mat3f M41 = {
        {COL (mat, 1)->xyz, COL (mat, 2)->xyz, COL (mat, 3)->xyz}
    };

    return vec4f_dot (
        COL (mat, 0),
        &(Vec4f
        ) {.x = mat3f_det (&M11),
           .y = mat3f_det (&M21),
           .z = mat3f_det (&M31),
           .w = mat3f_det (&M41)}
    );
}
