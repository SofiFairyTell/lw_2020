#include <stdlib.h>
#include <string.h> 

#include "nl.h"

/*
  ������ ������������� ������� �� ������ @mda.h@
  ����� ����������� �������
*/

int main()
{

  double *A, *SN, *SD, *BN, *BD, *UN, *UD, *DINV, *x, *b, *pb;  
  size_t *I, *J, *IA, *JA, *IS, *JS, *IB, *JB, *D, *P, *IP, *M, *L, *IU, *JU;
  size_t n, nz, k, unz;  

  // ���������� ����������������� ����� ������� A

  n = 5;
  nz = n - 1;
  A = nl_dvector_create(nz);
  I = nl_xvector_create(nz);
  J = nl_xvector_create(nz);
  for (k = 0; k < nz; k++)
    A[k] = 1.;
  for (k = 0; k < nz; k++)
  {
    I[k] = 0;
    J[k] = k + 1;
  }

  sp_create_sym(n, nz, &IS, &JS, &SN, &SD);
  sp_convert(nz, A, I, J, n, IS, JS, SN);
  //sp_order(IS, JS, SN, n); //��� ����� � ����� ������ ����� ���������. �� �� ���� ������.

  printf("��������� �������� ����������������� ����� ������� A:\n");
  sp_print_list(IS, JS, SN, n, n, NULL, NULL);

  // ���������� ������������ ����� ������� A

  SD[0] = n;
  for(k = 1; k < n; k++) 
    SD[k] = 1.;

  printf("\n������������ �������� ������� A:\n");
  nl_dvector_print(SD, n, 0);

  // ������������ ������������� � ������ � �������� �������� MDA

  mda_create(n, nz, &IA, &JA, &D, &P, &IP, &M, &L);
  mda_convert(n, IS, JS, IA, JA, D, P, IP);
  unz = mda_order(n, IA, JA, M, L, D, P, IP);

  printf("\n������������, ��������� ������� MDA:\n");
  nl_xvector_print(P, n, NULL);
  printf("\n�������� ������������:\n");
  nl_xvector_print(IP, n, NULL);

  // ��������� ������������ � ������� A:
  
  sp_create_sym(n, nz, &IB, &JB, &BN, &BD);
  sp_permute_sym(n, IS, JS, IB, JB, SN, SD, BN, BD, IP);

  printf("\n��������������� �������� ����� ������������:\n");
  sp_print_list(IB, JB, BN, n, n, NULL, NULL);
  printf("\n������������ �������� ����� ������������:\n");
  nl_dvector_print(BD, n, NULL);

  // ������������� � ��������� ����������

  DINV = nl_dvector_create(n);
  sp_create_sym(n, unz, &IU, &JU, &UN, &UD);
  
  sp_chol_symb(IB, JB, n, IU, JU, unz);
  sp_chol_num(IB, JB, BN, BD, IU, JU, n, UN, DINV);

  printf("\n����������������� ����� ������� U:\n");
  sp_print_list(IU, JU, UN, n, n, NULL, NULL);

  printf("\n��������, �������� ������������ ���������");
  printf(" ������� D:\n");
  nl_dvector_print(DINV, n, NULL);

  // ���������� �������

  x = nl_dvector_create(n);
  for(k = 0; k < n; k++) 
    x[k] = k;

  b = nl_dvector_create(n);
  sp_mult_col_sym(IS, JS, SN, SD, x, n, b);
  printf("\n������ ����� �������:\n");
  nl_dvector_print(b, n, NULL);

  // ������ �������

  pb = nl_dvector_create(n);
  nl_dvector_permute(b, P, n, pb);

  sp_chol_solve(IU, JU, UN, DINV, pb, n, x);
  nl_dvector_permute(x, IP, n, b);

  printf("\n�������:\n");
  nl_dvector_print(b, n, NULL);

  // ����������� ������

  nl_dvector_free(A);
  nl_xvector_free(I);
  nl_xvector_free(J);
  sp_free_sym(IS, JS, SN, SD);
  mda_free(IA, JA, D, P, IP, M, L);
  sp_free_sym(IB, JB, BN, BD);
  nl_dvector_free(DINV);
  sp_free_sym(IU, JU, UN, UD);
  nl_dvector_free(x);
  nl_dvector_free(b);
  nl_dvector_free(pb);

  return 0;

}
