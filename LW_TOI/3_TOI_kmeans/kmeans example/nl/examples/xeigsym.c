/*
  ������������ �������� ����������� ��������.
  $QR$-��������.
*/

#include "nl.h"

int main()
{
    size_t n = 5;
    size_t rc;
    double **A, *d, *e;

    A = nl_dmatrix_create(n, n);
    d = nl_dvector_create(n);
    e = nl_dvector_create(n);
    
    A[0][0] = 11;
    A[1][0] = 10; A[1][1] = 14; 
    A[2][0] = 18; A[2][1] = 41; A[2][2] = 17; 
    A[3][0] = 11; A[3][1] = 12; A[3][2] = 23; A[3][3] = 11; 
    A[4][0] = 21; A[4][1] = 13; A[4][2] = 23; A[4][3] = 17; A[4][4] = 17; 
    
    printf("\n������� A (������ ����������� �����):\n");
    nl_dmatrix_print(A, n, n, NULL);

    eig_tridiag_reduction(A, n, 1, d, e);

    printf("\n���������������� ���\n");
    printf("���������:\n");
    nl_dvector_print(d, n, NULL);

    printf("\n������������ = ������������:\n");
    nl_dvector_print(e, n, NULL);

    printf("\n������� �������� Q:\n");
    nl_dmatrix_print(A, n, n, NULL);

    eig_tridiag(d, e, n, 1, A, &rc);

    printf("\n����������� �����:\n");
    nl_dvector_print(d, n, NULL);

    printf("\n����������� �������:\n");
    nl_dmatrix_print(A, n, n, NULL);

    nl_dmatrix_free(A, n);
    nl_dvector_free(d);
    nl_dvector_free(e);
    
    return 0;
}
