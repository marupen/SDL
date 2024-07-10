#include "stdafx.h"

double mul_vect(vector<double> &A, vector<double> &B)
{
	double res = 0;
	for (int i = 0; i < A.size(); i++)
		res += A[i] * B[i];
	return res;
}

vector<double> get_col(matrix A, int c)
{
	vector<double> col = vector<double>(A.size());
	for (int i = 0; i < A.size(); i++)
		col[i] = A[i][c];
	return col;
}

void get_col(matrix &A, int c, vector<double> &res)
{
	for (int i = 0; i < A.size(); i++)
		res[i] = A[i][c];
}

matrix matrix_multiply(matrix &A, matrix &B)
{
	matrix C;
	C = matrix_init(A.size(), B[0].size());

	for (int i = 0; i < A.size(); i++) {
		for (int j = 0; j < B[i].size(); j++) {
			C[i][j] = mul_vect(A[i], get_col(B, j));
		}
	}
	return C;
}

void matrix_multiply(matrix &A, matrix &B, matrix &result)
{
	vector<double> col = vector<double>(A.size());
	for (int i = 0; i < A.size(); i++) {
		for (int j = 0; j < B[i].size(); j++) {
			get_col(B, j, col);
			result[i][j] = mul_vect(A[i], col);
		}
	}
}

matrix matrix_init(int x, int y)
{
	matrix R;
	for (int i = 0; i < x; i++)
	{
		R.push_back(vector<double>(y));
		for (int j = 0; j < y; j++)
		{
			if (i == j)
				R[i][j] = 1;
			else
				R[i][j] = 0;
		}
	}
	return R;
}

void matrix_e(matrix &A)
{
	for (int i = 0; i < A.size(); i++) {
		for (int j = 0; j < A[0].size(); j++) {
			if (i == j)
				A[i][j] = 1;
			else
				A[i][j] = 0;
		}
	}
}

void set_angle(matrix &rotate_matrix, float angle)
{
	rotate_matrix[0][0] = cos(angle);
	rotate_matrix[0][1] = -sin(angle);
	rotate_matrix[1][0] = sin(angle);
	rotate_matrix[1][1] = cos(angle);
}

matrix translation(float x_zero_offset, float y_zero_offset) {
	matrix translate_matrix = matrix_init(3, 3);
	translate_matrix[0][2] = -x_zero_offset;
	translate_matrix[1][2] = -y_zero_offset;
	return translate_matrix;
}

matrix dilatation(float scale_x, float scale_y) {
	matrix dilatate_matrix = matrix_init(3, 3);
	dilatate_matrix[0][0] = scale_x;
	dilatate_matrix[1][1] = scale_y;
	return dilatate_matrix;
}

matrix rotation(float angle) {
	matrix rotate_matrix = matrix_init(3, 3);
	rotate_matrix[0][0] = cos(angle);
	rotate_matrix[0][1] = sin(angle);
	rotate_matrix[1][0] = -sin(angle);
	rotate_matrix[1][1] = cos(angle);
	return rotate_matrix;
}

matrix transform(float &x, float &y, float x_zero_offset, float y_zero_offset, float angle, float scale_x, float scale_y) {
	matrix coord = matrix_init(3, 1);
	coord[0][0] = x;
	coord[1][0] = y;
	coord[2][0] = 1;
	matrix transform_matrix;
	//matrix A = matrix_multiply(rotation(angle), translation(x_zero_offset, y_zero_offset));
	//transform_matrix = matrix_multiply(dilatation(scale_x, scale_y), A);
	matrix new_coord = matrix_multiply(translation(x_zero_offset, y_zero_offset), coord);
	x = new_coord[0][0];
	y = -new_coord[1][0];

	return new_coord;

	/*matrix_multiply(coord, translation(x_zero_offset, y_zero_offset));
	transform_matrix = matrix_multiply(transform_matrix, rotation(angle));
	transform_matrix = matrix_multiply(transform_matrix, dilatation(scale_x, scale_y));*/
}