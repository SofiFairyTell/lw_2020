/*    
  ������ ������������� ������� �� ������ @chol.h@
  ������� ������������ ������������ ������������ 
  ������� $Ax=b$ � ������� ���������� ���������,
  $A =   \left(  \begin{array}{rrr}     0.1 & 0.2 & 0.3 \\       0.2 & 0.7 & 0.9 \\      0.3 & 0.9 & 1.3 \\   \end{array}  \right)  $, $\quad b =    \left(   \begin{array}{r}      0.6 \\        1.8 \\       2.5 \\    \end{array}   \right)  $.
*/

#include "nl.h"

int main()
{
  double **A, *b;
  size_t n = 3;

  A = nl_dmatrix_create(n, n);
  b = nl_dvector_create(n);

  A[0][0] = .1; A[0][1] = .2; A[0][2] =  .3;   b[0] =  .6;
  A[1][0] = .2; A[1][1] = .7; A[1][2] =  .9;   b[1] = 1.8;
  A[2][0] = .3; A[2][1] = .9; A[2][2] = 1.3;   b[2] = 2.5;

  printf("������� A:\n");
  nl_dmatrix_print(A, n, n, NULL);

  chol_decomp(A, n); 

  printf("\n���������� ���������:\n");
  printf("\n(� ������ ����������� �����):\n");
  nl_dmatrix_print(A, n, n, NULL);

  printf("\n������ b:\n");
  nl_dvector_print(b, n, NULL);

  chol_solve(A, n, b);

  printf("\n������� ������� Ax = b:\n");
  nl_dvector_print(b, n, NULL);

  nl_dmatrix_free(A, n);
  nl_dvector_free(b);

  return 0;
}
