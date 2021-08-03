/*
  ������ ������������� ������� �� ������ @sparse.h@
  ����� ����������� ���������� $Ax=b$
  $  A=  \left(  \begin{array}{rrrrr}     33 & 1 &   &   &   \\     1 & 2 & 1 &   &   \\       & 1 & 2 & 1 &   \\       &   & 1 & 2 & 1 \\       &   &   & 1 & 2 \\  \end{array}  \right)  ,\quad  b=  \left(  \begin{array}{r}     34  \\     4  \\     4  \\     4  \\     3  \\  \end{array}  \right)  $
*/
#include <stdlib.h>
#include <string.h> 

#include "nl.h"

int main()
{
  size_t n = 5;
  size_t nz = 4;
  double A[] = {1, 1, 1, 1};
  size_t I[] = {0, 1, 2, 3};
  size_t J[] = {1, 2, 3, 4};
  double AD[] = {33, 2, 2, 2, 2};
  size_t *IA, *JA;
  double *AN;
  double *b;
  double x[] = {0, 0, 0, 0, 0};
  int it;

  double xx[] = {1, 1, 1, 1, 1};

  sp_create(n, nz, &IA, &JA, &AN);
  sp_convert(nz, A, I, J, n, IA, JA, AN);

  b = nl_dvector_create(n);

  sp_mult_col_sym(IA, JA, AN, AD, xx, n, b);

  printf("����� ����������� ����������\n");
  printf("\n������� A:\n");
  sp_print_list_sym(IA, JA, AN, AD, n, NULL, NULL);

  printf("\n������ b:\n");
  nl_dvector_print(b, n, NULL);

  it = sp_conj_sym(IA, JA, AN, AD, b, n, 1e-3, 20, x);
  
  printf("\n������� ������� Ax = b:\n");
  nl_dvector_print(x, n, 0);

  printf("\n����� �������� = %i \n", it);

  sp_free(IA, JA, AN);

  nl_dvector_free(b);

  return 0;
}
