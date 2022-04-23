#pragma once
#include "mat2x2.hpp"
#include "mat2x3.hpp"
#include "mat2x4.hpp"
#include "mat3x2.hpp"
#include "mat3x3.hpp"
#include "mat3x4.hpp"
#include "mat4x2.hpp"
#include "mat4x3.hpp"
#include "mat4x4.hpp"

namespace myl::math {
	template<std::size_t R, std::size_t C, typename T>
	MYL_NO_DISCARD constexpr typename mat<R, C, T>::transpose_type transpose(const mat<R, C, T>& a_mat) {
		typename mat<R, C, T>::transpose_type transposed;
		for (std::size_t c = 0; c != C; ++c) // Tranverse columns
			for (std::size_t r = 0; r != R; ++r) // Transverse rows
				transposed[r][c] = a_mat[c][r];
		return transposed;
	}

	template<std::size_t R, std::size_t C, typename T>
	MYL_NO_DISCARD constexpr T determinant(const mat<R, C, T>& a_mat);

	template<typename T>
	MYL_NO_DISCARD constexpr T determinant(const mat<2, 2, T>& a_mat) {
		return a_mat[0][0] * a_mat[1][1] - a_mat[0][1] * a_mat[1][0];
	}

	template<typename T>
	MYL_NO_DISCARD constexpr T determinant(const mat<3, 3, T>& a_mat) {
		return
			  a_mat[0][0] * (a_mat[1][1] * a_mat[2][2] - a_mat[2][1] * a_mat[1][2]) // a(ei - fh)
			- a_mat[1][0] * (a_mat[0][1] * a_mat[2][2] - a_mat[2][1] * a_mat[0][2]) // - b(di - fg)
			+ a_mat[2][0] * (a_mat[0][1] * a_mat[1][2] - a_mat[1][1] * a_mat[0][2]); // + c(dh - eg)
	}

	template<typename T>
	MYL_NO_DISCARD constexpr T determinant(const mat<4, 4, T>& a_mat) {
		return
			  a_mat[0][0] * determinant(mat<3, 3, T>(a_mat[1][1], a_mat[1][2], a_mat[1][3], a_mat[2][1], a_mat[2][2], a_mat[2][3], a_mat[3][1], a_mat[3][2], a_mat[3][3])) // 3x3 mat a
			- a_mat[1][0] * determinant(mat<3, 3, T>(a_mat[2][1], a_mat[2][2], a_mat[2][3], a_mat[3][1], a_mat[3][2], a_mat[3][3], a_mat[0][1], a_mat[0][2], a_mat[0][3])) // 3x3 mat b
			+ a_mat[2][0] * determinant(mat<3, 3, T>(a_mat[3][1], a_mat[3][2], a_mat[3][3], a_mat[0][1], a_mat[0][2], a_mat[0][3], a_mat[1][1], a_mat[1][2], a_mat[1][3])) // 3x3 mat c
			- a_mat[3][0] * determinant(mat<3, 3, T>(a_mat[0][1], a_mat[0][2], a_mat[0][3], a_mat[1][1], a_mat[1][2], a_mat[1][3], a_mat[2][1], a_mat[2][2], a_mat[2][3])); // 3x3 mat d
	}

	template<typename T>
	MYL_NO_DISCARD constexpr mat<2, 2, T> inverse(const mat<2, 2, T>& a_mat) {
		return mat<2, 2, T>(a_mat[1][1], -a_mat[0][1], -a_mat[1][0], a_mat[0][0]) / determinant(a_mat);
	}

	template<typename T>
	MYL_NO_DISCARD constexpr mat<3, 3, T> inverse(const mat<3, 3, T>& a_mat) {
		return mat<3, 3, T>(
			 (a_mat[1][1] * a_mat[2][2] - a_mat[1][2] * a_mat[2][1]), //  (ei - hf)
			-(a_mat[0][1] * a_mat[2][2] - a_mat[2][1] * a_mat[0][2]), // -(di - fg)
			 (a_mat[0][1] * a_mat[1][2] - a_mat[0][2] * a_mat[1][1]), //  (dh - ge)
			-(a_mat[1][0] * a_mat[2][2] - a_mat[1][2] * a_mat[2][0]), // -(bi - hc)
			 (a_mat[0][0] * a_mat[2][2] - a_mat[0][2] * a_mat[2][0]), //  (ai - gc)
			-(a_mat[0][0] * a_mat[1][2] - a_mat[0][2] * a_mat[1][0]), // -(ah - gb)
			 (a_mat[1][0] * a_mat[2][1] - a_mat[1][1] * a_mat[2][0]), //  (bf - ec)
			-(a_mat[0][0] * a_mat[2][1] - a_mat[0][1] * a_mat[2][0]), // -(af - dc)
			 (a_mat[0][0] * a_mat[1][1] - a_mat[0][1] * a_mat[1][0])  //  (ae - db)
			) / determinant(a_mat);
	}

	template<typename T>
	MYL_NO_DISCARD constexpr mat<4, 4, T> inverse(const mat<4, 4, T>& a_mat) {
		const T a = a_mat[0][0];
		const T b = a_mat[1][0];
		const T c = a_mat[2][0];
		const T d = a_mat[3][0];
		const T e = a_mat[0][1];
		const T f = a_mat[1][1];
		const T g = a_mat[2][1];
		const T h = a_mat[3][1];
		const T i = a_mat[0][2];
		const T j = a_mat[1][2];
		const T k = a_mat[2][2];
		const T l = a_mat[3][2];
		const T m = a_mat[0][3];
		const T n = a_mat[1][3];
		const T o = a_mat[2][3];
		const T p = a_mat[3][3];

		const T a1 = determinant(mat<3, 3, T>({ f, j, n }, { g, k, o }, { h, l, p }));
		const T b1 = determinant(mat<3, 3, T>({ e, i, m }, { g, k, o }, { h, l, p }));
		const T c1 = determinant(mat<3, 3, T>({ e, i, m }, { f, j, n }, { h, l, p }));
		const T d1 = determinant(mat<3, 3, T>({ e, i, m }, { f, j, n }, { g, k, o }));
		const T e1 = determinant(mat<3, 3, T>({ b, j, n }, { c, k, o }, { d, l, p }));
		const T f1 = determinant(mat<3, 3, T>({ a, i, m }, { c, k, o }, { d, l, p }));
		const T g1 = determinant(mat<3, 3, T>({ a, i, m }, { b, j, n }, { d, l, p }));
		const T h1 = determinant(mat<3, 3, T>({ a, i, m }, { b, j, n }, { c, k, o }));
		const T i1 = determinant(mat<3, 3, T>({ b, f, n }, { c, g, o }, { d, h, p }));
		const T j1 = determinant(mat<3, 3, T>({ a, e, m }, { c, g, o }, { d, h, p }));
		const T k1 = determinant(mat<3, 3, T>({ a, e, m }, { b, f, n }, { d, h, p }));
		const T l1 = determinant(mat<3, 3, T>({ a, e, m }, { b, f, n }, { c, g, o }));
		const T m1 = determinant(mat<3, 3, T>({ b, f, j }, { c, g, k }, { d, h, l }));
		const T n1 = determinant(mat<3, 3, T>({ a, e, i }, { c, g, k }, { d, h, l }));
		const T o1 = determinant(mat<3, 3, T>({ a, e, i }, { b, f, j }, { d, h, l }));
		const T p1 = determinant(mat<3, 3, T>({ a, e, i }, { b, f, j }, { c, g, k }));

		return mat<4, 4, T>(
			{ a1, -b1, c1, -d1 },
			{ -e1, f1, -g1, h1 },
			{ i1, -j1, k1, -l1 },
			{ -m1, n1, -o1, p1 }) / determinant(a_mat);
	}
}