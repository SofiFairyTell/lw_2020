/*
  ������ ������������� ������� �� ������ @sparse.h@
  ����� ������--������� ������� ����������� ������� 
  $Ax=b$
  $  A=  \left(  \begin{array}{rrrrr}     3 & 1 &   &   &   \\     1 & 3 & 1 &   &   \\       & 1 & 3 & 1 &   \\       &   & 1 & 3 & 1 \\       &   &   & 1 & 3 \\  \end{array}  \right)  ,\quad  b=  \left(  \begin{array}{r}     4  \\     5  \\     5  \\     5  \\     4  \\  \end{array}  \right)  $
*/
#include <stdlib.h>
#include <string.h> 

#include "nl.h"

int main()
{
  double A[] = {1, 1, 1, 1, 1, 1, 1, 1};
  size_t  I[] = {0, 1, 2, 3, 1, 2, 3, 4};
  size_t  J[] = {1, 2, 3, 4, 0, 1, 2, 3};
  double D[] = {3, 3, 3, 3, 3};
  size_t *IA, *JA; 
  double *AN;
  double b[] = {4, 5, 5, 5, 4};
  double x[5];
  int it;

  sp_create(5, 8, &IA, &JA, &AN);
  sp_convert(8, A, I, J, 5, IA, JA, AN);

  printf("����� ������-�������\n");
  printf("\n������� A (��� ���������):\n");
  sp_print_list(IA, JA, AN, 5, 5, 0, 0);
  printf("\n��������� ������� A:\n");
  nl_dvector_print(D, 5, 0);

  printf("\n������ b:\n");
  nl_dvector_print(b, 5, 0);

  it = sp_gauss_seidel(IA, JA, AN, D, b, 5, 1, 1e-3, 20, x);
  
  printf("\n������� ������� Ax = b:\n");
  nl_dvector_print(x, 5, 0);

  printf("\n����� ��������: %i \n", it);

  sp_free(IA, JA, AN);

  return 0;
}
