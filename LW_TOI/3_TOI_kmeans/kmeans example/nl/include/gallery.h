#ifndef __GALLERY_H__
#define __GALLERY_H__

/**
  \file
  ��������� �������.

  ���� �������� �������, ������������ ����� ������������� � ����������� �������.
*/

/**
  \example xlaplace.c
*/

/**	
  ��������� ����������� ������������ ������� S.
    - ����:
      - \f$n\f$ - ������ �������
      - \f$nz\f$ - ����� ��������� ���������
      -	\f$M\f$ - ��������������� ������ ����� \f$n\f$
      -	\f$IS\f$ - ������ ����� \f$n + 1\f$
      -	\f$JS\f$ - ������ ����� \f$nz\f$
    - �����:
      -	\f$IS\f$, \f$JS\f$ - ����������� ������� ������� � ������� RR(U)U
*/
extern size_t gall_rand_sym(
	size_t n,
	size_t nz,
	size_t *IS,
	size_t *JS,
	size_t *M);

/**
  ������� �������� ������������� ��������� ��������.

  - ����:  
     - \f$m\f$, \f$n\f$  - ������ �����
  - �����: 
     - \f$IA\f$, \f$JA\f$, \f$AN\f$ - ����������� ������������� ������� � RR(O)U-�������
*/
extern void gall_laplace(size_t m, size_t n, size_t *IA, size_t *JA, double *AN);

/**
  ������� ������������� ������� �������� ������������� ��������� ��������.

  - ����:  
     - \f$m\f$, \f$n\f$  - ������ �����
  - �����: 
     - \f$N\f$ - ������� �������
     - \f$nz\f$ - ����� ��������� ��������� � RR(O)U-�������
*/
extern void gall_laplace_size(size_t m, size_t n, size_t *N,	size_t *nz);

/**
  ������� �������� ������������� ��������� ��������.

  - ����:  
     - \f$m\f$, \f$n\f$  - ������ �����
  - �����: 
     - \f$IA\f$, \f$JA\f$, \f$AN\f$, \f$AD\f$ - ����������� ������������� ������� � RR(U)U-�������
*/
extern void gall_laplace_sym(size_t m, size_t n, size_t *IA, size_t *JA, double *AN, double *AD);

/**
  ������� ������������� ������� �������� ������������� ��������� ��������.

  - ����:  
     - \f$m\f$, \f$n\f$  - ������ �����
  - �����: 
     - \f$N\f$ - ������� �������
     - \f$nz\f$ - ����� ��������� ��������� � RR(U)U-�������
*/
extern void gall_laplace_sym_size(size_t m, size_t n, size_t *N,	size_t *nz);
#endif
