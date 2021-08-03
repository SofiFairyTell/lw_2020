/*    
  ������ ������������� ������� �� ������ @qr.h@
  ������� ������� �������� ��������� $Ax=b$ 
  � ������� QR-����������,
  $A =   \left(  \begin{array}{rrr}     0.1 & 0.5 & 0.6 \\       0.2 & 0.7 & 0.9 \\      0.3 & 1.1 & 1.3 \\   \end{array}  \right)  $, $\quad b =    \left(   \begin{array}{r}      1.2 \\        1.8 \\       2.7 \\    \end{array}   \right)  $.
*/

#include "nl.h"

int main()
{
  double **A, *b, *t;
  size_t n = 3;

  A = nl_dmatrix_create(n, n);
  b = nl_dvector_create(n);
  t = nl_dvector_create(n);

  A[0][0] = .1; A[0][1] =  .5; A[0][2] =  .6;   b[0] = 1.2;
  A[1][0] = .2; A[1][1] =  .7; A[1][2] =  .9;   b[1] = 1.8;
  A[2][0] = .3; A[2][1] = 1.1; A[2][2] = 1.3;   b[2] = 2.7;

  printf("������� A:\n");
  nl_dmatrix_print(A, n, n, NULL);

  qr_decomp(A, n, n, t);

  printf("\nQR-����������:\n");
  nl_dmatrix_print(A, n, n, NULL);

  printf("\n������ t:\n");
  nl_dvector_print(t, n, NULL);

  printf("\n������ b:\n");
  nl_dvector_print(b, n, NULL);

  qr_solve (A, n, t, b);

  printf("\n������� ������� Ax = b:\n");
  nl_dvector_print(b, n, NULL);

  nl_dmatrix_free(A, n);
  nl_dvector_free(b);
  nl_dvector_free(t);

  return 0;
}
