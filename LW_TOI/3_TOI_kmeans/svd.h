#pragma once
#ifndef __SVD_H__
#define __SVD_H__

//  ����������� ���������� �������.
extern void svd_decomp(double** A, size_t m, size_t n, double* w, int matu, double** U, int
	matv, double** V, size_t *ierr);
//�������������� ��������������� ����������� ����������� �����.
extern void svd_correct(double *w, size_t n, double rel_err);
//  ���������� ������������� ����� ��������������� �� ������ ������������ ����������
extern double svd_cond(double *w, size_t n);
//  ���������� ����������� ������������� ������� �������� ��������� �� ������ ������������ ����������.
extern void svd_least_squares(double **U, double *w, double **V, size_t m, size_t n, double *b, double *x);

#endif
