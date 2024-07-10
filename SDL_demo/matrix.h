#pragma once

using namespace std;
using matrix = vector<vector<double>>;

matrix matrix_multiply(matrix &A, matrix &B);
void matrix_multiply(matrix &A, matrix &B, matrix &result);
matrix matrix_init(int x, int y);
matrix multiply(matrix &A);
vector<double> get_col(matrix A, int c);
double mul_vect(vector<double> &A, vector<double> &B);
void set_angle(matrix &rotate_matrix, float angle);
matrix translation(float x_zero_offset, float y_zero_offset);
matrix dilatation(float scale_x, float scale_y);
matrix rotation(float angle);
matrix transform(float &x, float &y, float x_zero_offset, float y_zero_offset, float angle, float scale_x, float scale_y);