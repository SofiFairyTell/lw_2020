#ifndef __UTIL_H__
#define __UTIL_H__ 

#include <malloc.h>
#include <math.h>
#include <stdio.h>

/**
  \file 
  ��������������� ���������. 

  ���� �������� ��������������� ��������� ��� ���������� 
  ��������� �������.
*/

/*
  ��� ������ ��� ������������� �������� �������� � ������.
  ��������� ��� ������������� � �������� ��������.
  ������ �������� ���������� size_t
*/
typedef size_t size_t;

/**
  \name ��������� ������
*/

/**
  ���� ������
*/

typedef enum
{
  nl_err_OK                    = 0,	  ///< ������ ���
  nl_err_out_of_memory         = 1,	  ///< ������������ ������
  nl_err_inconsistent_size,		  ///< �������� ������
  nl_err_incorrect_index,		  ///< �������� ������
  nl_err_matrix_must_be_square,		  ///< ������� ������ ���� ����������
  nl_err_matrix_is_singular,		  ///< ������� ���������
  nl_err_matrix_is_not_positive_definite, ///< ������� �� ������������ ������������
  nl_err_IO,				  ///< ������ ��� �����/������
  nl_err_diag_elem_must_be_non_zero,	  ///< ������������ �������� ������ ���� ����������
  nl_err_count,				  ///< ���������� ����� ������������������ ������
} nl_err_enum;

/**
  ��������� �� �������, ������� ���������� � ������ ������������� ������.
  �� ��������� �������� �� ������� ��������� �� ������ � � ������, 
  ���� #critical ������� �� ���� ��� ���������� 
  #nl_exit_when_error ������� �� ����, �������� ��������� ������ ���������.
  � ������ ������������� ������������ ����� �������� ���� ���������� ������.
*/
extern void (*nl_error)(nl_err_enum error, int critical);

/**
  ���� �������� ���� ���������� ������� �� ����, �� � ������ ������������� ������
  ��������� #nl_error ��������� ������ ���������.
*/
extern int nl_exit_when_error;



/**                         
  \name ��������, �������� � ����������� ��������
*/

/**
  ������� ������ ����� \f$n\f$. 
  ������� ��������� ������ ��� ������ � ���������� ��������� �� ����.
  ��� ��������� - double
*/
extern double* nl_dvector_create(size_t n);

/**
  ������� ������ ����� \f$n\f$. 
  ������� ��������� ������ ��� ������ � ���������� ��������� �� ����.
  ��� ��������� - #size_t
*/
extern size_t* nl_xvector_create(size_t n);

/**
  ������� ����� ������� \f$v\f$ ����� \f$n\f$. 
  ������� ��������� ������ ��� ������, �������� �������� � ����������
  ��������� �� ����.
  ��� ��������� - double
*/
extern double* nl_dvector_create_copy(const double* v, size_t n);

/**
  ������� ����� ������� \f$v\f$ ����� \f$n\f$. 
  ������� ��������� ������ ��� ������, �������� �������� � ����������
  ��������� �� ����.
  ��� ��������� - #size_t
*/
extern size_t* nl_xvector_create_copy(size_t* v, size_t n);

/**
  �������� ������ \f$b\f$ � ������ \f$a\f$.
  ��� ��������� - double
*/
extern void nl_dvector_copy(double* a, const double* b, size_t n);

/**
  �������� ������ \f$b\f$ � ������ \f$a\f$.
  ��� ��������� - #size_t
*/
extern void nl_xvector_copy(size_t* a, const size_t* b, size_t n);

/**
  ���������� ������, ������� �������� \f$v\f$ ����� \f$n\f$. 
  ��� ��������� - double
*/
extern void nl_dvector_free(double* v);

/**
  ���������� ������, ������� �������� \f$v\f$ ����� \f$n\f$. 
  ��� ��������� - #size_t
*/
extern void nl_xvector_free(size_t* v);


/**
  \name ���� � ����� ��������
*/

/**
  ��������������� ����� ������� \f$v\f$ ����� \f$n\f$ �� �����.
  ��� ��������� - double
*/
extern void nl_dvector_print(double* v, size_t n, const char* format);

/**
  ��������������� ����� ������� \f$v\f$ ����� \f$n\f$ �� �����.
  ��� ��������� - #size_t
*/
extern void nl_xvector_print(size_t* v, size_t n, const char* format);

/**
  ��������������� ����� ������� \f$v\f$ ����� \f$n\f$ � ���� file.
  ��� ��������� - double
*/
extern void nl_dvector_fprint(FILE* file, const double* v, size_t n, const char* format);

/**
  ��������������� ����� ������� \f$v\f$ ����� \f$n\f$ � ���� file.
  ��� ��������� - size_t
*/
extern void nl_xvector_fprint(FILE* file, const size_t* v, size_t n, const char* format);

/**
  ��������������� ����� ������� \f$v\f$ ����� \f$n\f$ � ���� � ������ \f$filename\f$.
  ��� ��������� - double
*/
extern void nl_dvector_fwrite(const char* filename, double* v, size_t n, const char* format);

/**
  ��������������� ����� ������� \f$v\f$ ����� \f$n\f$ � ���� � ������ \f$filename\f$.
  ��� ��������� - #size_t
*/
extern void nl_xvector_fwrite(const char* filename, size_t* v, size_t n, const char* format);

/**
  ������ ������� � ����������.
  ������� ��������� � ���������� ������ ������� \f$n\f$, ��������� ������ ���
  ������, ��������� �������� � ���������� ��������� �� ������.
  ��� ��������� - double
*/
extern double* nl_dvector_scan(size_t* n);

/**
  ������ ������� � ����������.
  ������� ��������� � ���������� ������ ������� \f$n\f$, ��������� ������ ���
  ������, ��������� �������� � ���������� ��������� �� ������.
  ��� ��������� - #size_t
*/
extern size_t* nl_xvector_scan(size_t* n);

/**
  ������ ������� �� �����.
  ������� ��������� �� ����� \f$file\f$ ������ ������� \f$n\f$, ��������� ������ ���
  ������, ��������� �������� � ���������� ��������� �� ������.
  ��� ��������� - double
*/
extern double* nl_dvector_fscan(FILE* file, size_t* n);

/**
  ������ ������� �� �����.
  ������� ��������� �� ����� \f$file\f$ ������ ������� \f$n\f$, ��������� ������ ���
  ������, ��������� �������� � ���������� ��������� �� ������.
  ��� ��������� - #size_t
*/
extern size_t* nl_xvector_fscan(FILE* file, size_t* n);

/**
  ������ ������� �� �����.
  ������� ��������� �� ����� � ������ \f$filename\f$ ������ ������� \f$n\f$, ���������
  ������ ��� ������, ��������� �������� � ���������� ��������� �� ������.
  ��� ��������� - double 
*/
extern double* nl_dvector_fread(const char* filename, size_t* n);

/**
  ������ ������� �� �����.
  ������� ��������� �� ����� � ������ \f$filename\f$  ������ ������� \f$n\f$, ���������
  ������ ��� ������, ��������� �������� � ���������� ��������� �� ������.
  ��� ��������� - #size_t
*/
extern size_t* nl_xvector_fread(const char* filename, size_t* n);



/**
  \name �������� � ���������
*/

/**
	�������� ��������.
	- ����:
	  - \f$a\f$ - ������
	  - \f$b\f$ - ������
	  - \f$n\f$ - ������ ��������
	- �����:
	  - \f$a = a+b\f$
*/
extern void nl_dvector_add(double* a, const double* b, size_t n);


/**
	��������� �� ������� \f$a\f$ ������� \f$b\f$.
	- ����:
	  - \f$a\f$ - ������
	  - \f$b\f$ - ������
	  - \f$n\f$ - ������ ��������
	- �����:
	  - \f$a = a-b\f$
*/
extern void nl_dvector_sub(double* a, const double* b, size_t n);

/**
  ���������� 1-����� (�������������) �������� ���� ��������.
  - ����:
	- \f$a\f$ - ������ ������� \f$n\f$
	- \f$b\f$ - ������ ������� \f$n\f$
	- \f$n\f$ - ������ �������� \f$a\f$ � \f$b\f$
  - �����:
  - ���������: \f$\|a-b\|_1\f$

*/
extern double nl_dvector_diff_norm1(const double* a, const double* b, size_t n);

/**
  ���������� 2-����� �������� (����������) ���� ��������.
  - ����:
	- \f$a\f$ - ������ ������� \f$n\f$
	- \f$b\f$ - ������ ������� \f$n\f$
	- \f$n\f$ - ������ �������� \f$a\f$ � \f$b\f$
  - �����:
  - ���������: \f$\|a-b\|_2\f$

*/
extern double nl_dvector_diff_norm2(const double* a, const double* b, size_t n);

/**
  ���������� 1-����� (�������������) �������.
  - ����:
	- \f$a\f$ - ������ ������� \f$n\f$
	- \f$n\f$ - ������ ������� \f$a\f$
  - �����:
  - ���������: \f$\|a\|_1\f$

*/
extern double nl_dvector_norm1(const double* a, size_t n);


/**
  ���������� 2-����� (����������) �������.
  - ����:
	- \f$a\f$ - ������ ������� \f$n\f$
	- \f$n\f$ - ������ ������� \f$a\f$
  - �����:
  - ���������: \f$\|a\|_2\f$

*/
extern double nl_dvector_norm2(const double* a, size_t n);


/**
  ���������� \f$\infty\f$-����� (���������) �������.
  - ����:
	- \f$a\f$ - ������ ������� \f$n\f$
	- \f$n\f$ - ������ ������� \f$a\f$
  - �����:
  - ���������: \f$\|a\|_{\infty}\f$

*/
extern double nl_dvector_norm_inf(const double* a, size_t n);

/**
  ���������� ���������� ������������ ��������.
  - ����:
	- \f$a\f$ - ������ ������� \f$n\f$
	- \f$b\f$ - ������ ������� \f$n\f$
	- \f$n\f$ - ������ ������� \f$a\f$
  - �����:
  - ���������: \f$(a,b)\f$

*/
extern double nl_dvector_dot(const double* a, const double* b, size_t n);

/**
  ���������� ������� \f$z = \alpha x + y\f$.
  - ����:
	- \f$alpha\f$ - ����������� ����� \f$x\f$
	- \f$x\f$ - ������ ������� \f$n\f$
	- \f$y\f$ - ������ ������� \f$n\f$
	- \f$n\f$ - ������ ������� \f$a\f$
  - �����:
	- \f$z\f$ - ������ ������� \f$n\f$
*/
extern void nl_dvector_axpy(double alpha, const double *x, const double *y, size_t n, double *z);

/**
  ������������ ��������� �������.
  - ����:
	- \f$a\f$ - ������ ����� \f$n\f$
	- \f$P\f$ - ������������ ����� \f$1,2,\dots,n\f$
	- \f$n\f$ - ����� �������� \f$a\f$, \f$P\f$
  - �����:
	- \f$b\f$ - ������, ������������ �� ��������� ������� \f$a\f$, �������������� �������� \f$P\f$
*/

extern void nl_dvector_permute(const double *a, const size_t *P, size_t n, double *b);



/**
  \name ��������, �������� � ����������� ������
*/

/**
  ������� ������� � ��������� \f$m\times n\f$. 
  ������� ��������� ������ ��� ������� � ���������� ��������� �� ����.
  ��� ��������� - double
*/
extern double** nl_dmatrix_create(size_t m, size_t n);

/**
  ������� ����� ������� \f$A\f$ � ��������� \f$m\times n\f$. 
  ������� ��������� ������ ��� �������, �������� �������� � ����������
  ��������� �� ����.
  ��� ��������� - double
*/
extern double** nl_dmatrix_create_copy(const double** A, size_t m, size_t n);

/**
  �������� \f$m\times n\f$-������ B ������� \f$A\f$.
  ��� �������� ������� \f$A\f$ ������ ���� �������� �����.
  ��� ��������� - double
*/
extern void nl_dmatrix_copy(double** A, const double** B, size_t m, size_t n);

/**
  ���������� ������, ������� �������� \f$A\f$ � ��������� \f$m\times n\f$. 
  ��� ��������� - double
*/
extern void nl_dmatrix_free(double** A, size_t m);

/**
  ����������� ������������� �������.
  ������� \f$A\f$ � ��������� \f$m\times n\f$, �������������� � ���� ������ �����
  ��������� (�� �������), �������������� � �������������,
  �������� � ����������. ��� ��������� - double
*/
extern double** nl_dmatrix_convert(double* A, size_t m, size_t n);



/**
  \name ���� � ����� ������
*/


/**
  ��������������� ����� \f$m\times n\f$ ������� \f$A\f$ �� �����.
  ��� ��������� - double
*/
extern void nl_dmatrix_print(double **A, size_t m, size_t n, const char* format);

/**
  ��������������� ����� \f$m\times n\f$ ������� \f$A\f$ � ���� \f$file\f$.
  ��� ��������� - double
*/
extern void nl_dmatrix_fprint(FILE* file, double** A, size_t m, size_t n, const char* format);

/**
  ��������������� ����� \f$m\times n\f$ ������� \f$A\f$ � ���� � ������ \f$filename\f$.
  ��� ��������� - double
*/
extern void nl_dmatrix_fwrite(const char* filename, double** A, size_t m, size_t n, const char* format);

/**
  ������ ������� � ����������.
  ������� ��������� � ���������� ������� ������� \f$m\f$, \f$n\f$, ��������� ������ ���
  �������, ��������� �������� � ���������� ��������� �� �������.
  ��� ��������� - double
*/
extern double** nl_dmatrix_scan(size_t* m, size_t* n);

/**
  ������ ������� �� �����.
  ������� ��������� �� ����� \f$file\f$ ������� ������� \f$m\f$, \f$n\f$, ��������� ������ ���
  �������, ��������� �������� � ���������� ��������� �� �������.
  ��� ��������� - double
*/
extern double** nl_dmatrix_fscan(FILE* file, size_t* m, size_t* n);

/**
  ������ ������� �� �����.
  ������� ��������� �� ����� � ������ \f$filename\f$ ������� ������� \f$m\f$, \f$n\f$,
  ��������� ������ ��� �������, ��������� �������� � ���������� ���������
  �� �������.
  ��� ��������� - double 
*/
extern double** nl_dmatrix_fread(const char* filename, size_t* m, size_t* n);


/**
  \name �������� � ��������� 
*/

/**
	�������� ������� \f$A\f$ ������� \f$m\times n\f$ �� ������� B \f$n \times k\f$
	��������� � \f$Mul\f$ ������� \f$m\times k\f$. 
*/
extern void nl_dmatrix_mult(size_t m, size_t n, size_t k, double** A, double** B, double** Mul);

/**
	�������� ���� ������.
	- ����:
	  - \f$m\f$ - ����� ����� ������
	  - \f$n\f$ - ����� �������� ������
	  - \f$A\f$ - �������
	  - \f$B\f$ - �������
	- �����:
	  - \f$A = A+B\f$
*/
extern void nl_dmatrix_add(size_t m, size_t n, double** A, double** B);

/**
	�������� �� ������� \f$A\f$ ������� \f$m\times n\f$ ������� B ������� \f$m\times n\f$
	��������� � ������� \f$�\f$.
*/
extern void nl_dmatrix_sub(size_t m, size_t n, double** A, double** B);

/**
  ���������� ��������� ����� �������� ���� ������.
  - ����:
    - \f$A\f$ - ������� ������� \f$m\times n\f$
    - \f$B\f$ - ������� ������� \f$m\times n\f$
    - \f$m\f$, \f$n\f$ - ������� ������ \f$A\f$ � \f$B\f$
  - �����:
  - ���������: \f$\|A-B\|_{\infty}\f$

*/
extern double nl_dmatrix_diff_norm_inf(double** A, double** B, size_t m, size_t n);


/**
  \name �������� � ��������� � ���������
*/


/**
  ��������� �������-������ \f$b\f$ �� ������� \f$A\f$.
  ������� �������� ������ ��� �������������� ������ � ���������� � ���� ���������
  ���������.
  - ����:
       - \f$b\f$ - ������-������
       - \f$A\f$ - �������� �������
       - \f$m\f$ - ����� ����� ������� \f$A\f$
       - \f$n\f$ - ����� �������� ������� \f$A\f$
  - ���������:
       - \f$A\cdot b\f$
*/
extern double* nl_dvector_mult_dmatrix(double* b, double** A, size_t m, size_t n);


/**
  ��������� ������� \f$A\f$ �� ������ \f$b\f$ ������.

  - ����:
       - \f$A\f$ - �������� �������
       - \f$m\f$ - ����� ����� ������� \f$A\f$
       - \f$n\f$ - ����� �������� ������� \f$A\f$
       - \f$b\f$ - ������-�������
  - �����:
       - \f$res = A\cdot b\f$
*/
extern void nl_dmatrix_mult_dvector(double** A, size_t m, size_t n, const double* b, double* res);

extern const char double_format[];
extern const char integer_format[];


#endif
