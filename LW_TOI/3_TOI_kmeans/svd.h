#pragma once
#ifndef __SVD_H__
#define __SVD_H__

//  —ингул€рное разложение матрицы.
extern void svd_decomp(double** A, size_t m, size_t n, double* w, int matu, double** U, int
	matv, double** V, size_t *ierr);
//јвтоматическое корректирование вычисленных сингул€рных чисел.
extern void svd_correct(double *w, size_t n, double rel_err);
//  ¬ычисление спектрального числа обусловленности на основе сингул€рного разложени€
extern double svd_cond(double *w, size_t n);
//  Ќахождение нормального псевдорешени€ системы линейных уравнений на основе сингул€рного разложени€.
extern void svd_least_squares(double **U, double *w, double **V, size_t m, size_t n, double *b, double *x);

#endif
