/*
  ������ ������������� ������� �� ������ @optim.h@ 
  ����������� <<������>>-����������:
  $f(x_1, x_2) = (x_2 - x_1^2)^2 + (1 - x_1)^2$
  ��������-������� �������-����   
*/

#include <stdio.h>
#include <math.h>
#include "nl.h"

double func(double *x)
{

  double tmp1, tmp2;

  tmp1 = x[1] - x[0]*x[0];
  tmp2 = 1 - x[0];

  return tmp1 * tmp1 + tmp2 * tmp2;
}

int main(void)
{
  size_t n;
  double *x0, **x, *f, *xbar, *xr, *xe, *xc;
  double f0;
  double tolf, tolx;
  int maxfun, maxiter, rc, nfun, niter;

  n = 2;

  x0 = nl_dvector_create(n);
  x = nl_dmatrix_create(n + 1, n);
  f = nl_dvector_create(n + 1);
  xbar = nl_dvector_create(n);
  xr = nl_dvector_create(n);
  xe = nl_dvector_create(n);
  xc = nl_dvector_create(n);

  x0[0] = -1.2;
  x0[1] = 1;
  tolf = 1.0e-6;
  tolx = 1.0e-6;
  maxfun = 200;
  maxiter = 50;

  opt_nelder_mead(n, func, x0, &f0, 0, x, f, tolf, tolx, maxfun, maxiter, 
    &rc, &nfun, &niter, xbar, xr, xe, xc);

  if (rc)
  {
    printf("\n����� �������� ��� ���������� ����������� �������� �������\n");
    printf("��������� ����������� ����������!\n" );
  }
  else
    printf("\n�������� ����������\n");

  printf("\n���������� ����������� �������� �������: %d\n", nfun);
  printf("����� �������� (���������� ����������� ����������): %d\n",niter);

  printf("\n����������� ����� ��������:\n");
  nl_dvector_print(x0, n, "  %12.6e");

  printf("\n������� ���������� ���������:\n");
  nl_dmatrix_print(x, n + 1, n, "  %12.6e");

  printf("\n�������� ������� � ��������:\n");
  nl_dvector_print(f, n + 1, "  %12.6e");
  
  nl_dvector_free(x0);
  nl_dmatrix_free(x, n + 1);
  nl_dvector_free(f);
  nl_dvector_free(xbar);
  nl_dvector_free(xr);
  nl_dvector_free(xe);
  nl_dvector_free(xc);

  return 0;
}
