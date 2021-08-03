#include <math.h>
#include <assert.h>
#include "util.h"
#include "svd.h"

#define SVD_SIGN(a, b) (((b) >= 0) ? fabs(a):-fabs(a))

void svd_decomp(double** A, size_t m, size_t n, double* w, int matu, double** U, 
  int matv, double** V, size_t *ierr)
{
  size_t i, j, k, l, ii, i1, kk, k1, ll, l1, mn, its;
  double c, f, g, h, s, x, y, z, scale, anorm, tmp;
  double *rv1;

  rv1 = nl_dvector_create(n);
  (*ierr) = 0;

    for (i = 0; i < m; i++)
      for (j = 0; j < n; j++)
        U[i][j] = A[i][j];

  // 1 ����
  // ���������� � ����������������� ���� ����������� �����������
  
  g = 0.0;
  scale = 0.0;
  anorm = 0.0;

  for (i = 0; i < n; i++)
  {
     l = i + 1;
     rv1[i] = scale * g;
     g = 0.0;
     s = 0.0;
     scale = 0.0;
     if (i < m) 
     {
       for (k = i; k < m; k++)
         scale += fabs(U[k][i]);

       if (scale != 0.0) 
       {

          for (k = i; k < m; k++)
          {
             U[k][i] /= scale;
             s += U[k][i] * U[k][i];
          }
       

          f = U[i][i];
          g = -SVD_SIGN(sqrt(s),f);
          h = f * g - s;
          U[i][i] = f - g;
          if (i != n - 1) 
          {
            for (j = l; j < n; j++)
            {
               s = 0.0;

               for (k = i; k < m; k++)
                  s += U[k][i] * U[k][j];

               f = s / h;

               for (k = i; k < m; k++)
                  U[k][j] += f * U[k][i];
            }
          }

          for (k = i; k < m; k++)
            U[k][i] = scale * U[k][i];

       }
     }
     
     w[i] = scale * g;
     g = 0.0;
     s = 0.0;
     scale = 0.0;

     if (i < m && i != n - 1) 
     {
       for (k = l; k < n; k++)
         scale += fabs(U[i][k]);

       if (scale != 0.0)
       {

         for (k = l; k < n; k++)
         {
            U[i][k] /= scale;
            s += U[i][k] * U[i][k];
         }
         

         f = U[i][l];
         g = -SVD_SIGN(sqrt(s),f);
         h = f * g - s;
         U[i][l] = f - g;

         for (k = l; k < n; k++)
           rv1[k] = U[i][k] / h;

         if (i != m - 1) 
           for (j = l; j < m; j++)
           {
              s = 0.0;

              for (k = l; k < n; k++)
                s += U[j][k] * U[i][k];

              for (k = l; k < n; k++)
                 U[j][k] += s * rv1[k];
           }

         for (k = l; k < n; k++)
           U[i][k] *= scale;
       }
    }
    if (anorm < (tmp = fabs(w[i]) + fabs(rv1[i])))
      anorm = tmp;
  }
  
  // ���������� �������������� ��������������
  
  if (matv) 
  {

    for (ii = n; ii > 0; ii--)
    {
       i = ii - 1;

       if (i != n - 1)
       {
         if (g != 0.0)
         {
           for (j = l; j < n; j++)
             V[j][i] = (U[i][j] / U[i][l]) / g; 
                // ������� ������� ������� ��������� �������� ����

           for (j = l; j < n; j++)
           {
              s = 0.0;

              for (k = l; k < n; k++)
                s += U[i][k] * V[k][j];

              for (k = l; k < n; k++)
                 V[k][j] += s * V[k][i];
           }
           
         }

         for (j = l; j < n; j++)
         {
            V[i][j] = 0.0;
            V[j][i] = 0.0;
         }
         
       }
       V[i][i] = 1.0;
       g = rv1[i];
       l = i;
    }

  }

  // ���������� ������������� ��������������

  if (matu) 
  {

    mn = n;
    if (m < n) mn = m; // mn = min(m, n)

    for (l = mn; l > 0; l--)
    {
       i = l - 1;
       g = w[i];
       if (i != n - 1) 
         for (j = l; j < n; j++)
           U[i][j] = 0.0;

       if (g == 0.0) 
       {
         for (j = i; j < m; j++)
           U[j][i] = 0.0;
       }
       else
       {
         if (i != mn - 1)
         {
           for (j = l; j < n; j++)
           {
              s = 0.0;

              for (k = l; k < m; k++)
                s += U[k][i] * U[k][j];
              f = (s / U[i][i]) / g; 
                // ������� ������� ������� ��������� �������� ����

              for (k = i; k < m; k++)
                 U[k][j] += f * U[k][i];
           }
         }
         for (j = i; j < m; j++)
           U[j][i] /= g;

       }

       U[i][i] += 1.0;
    }

  }
     
 // 2 ����
 // ���������� ���������������� ������� � ������������� ���� 

 for (kk = n; kk > 0; kk--)
 {
   k = kk - 1;
   k1 = k - 1;
   its = 0;

 // �������� ����������� �����������

splitting_test:

   for (ll = k + 1; ll > 0; ll--)
   {   
      l = ll - 1;
      l1 = l - 1;
      if (fabs(rv1[l]) + anorm == anorm) goto convergence_test;
        // rv1[0] ������ ����� ����, �������
        // ������ ����� ����� ����� �� ����� 
      if (fabs(w[l1]) + anorm == anorm) break;
   }
   // ���� l ������ ��� 0, �� rv1[l] ������������� ������� ��������

   c = 0.0;
   s = 1.0;

   for (i = l; i <= k; i++)
   {
      f = s * rv1[i];
      rv1[i] *= c;

      if (fabs(f) + anorm == anorm) goto convergence_test;

      g = w[i];
      h = sqrt(f * f + g * g);
      assert(h > 0);
      w[i] = h;
      c = g / h;
      s = -f / h;
      if (matu) 
        for (j = 0; j < m; j++)
        {
           y = U[j][l1];
           z = U[j][i];
           U[j][l1] = y * c + z * s;
           U[j][i] = -y * s + z * c;
        }
   }


// �������� ����������

convergence_test:   

   z = w[k];

   if (l != k) 
   {
     // ����� ���������� �� ������� �������� ������ ������� 2
     if (its == 30) 
     {
        (*ierr) = k;  
        break;
     }
     its++;
     x = w[l];
     y = w[k1];
     g = rv1[k1];
     h = rv1[k];
     f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2.0 * h * y);
     g = sqrt(f * f + 1.0);
     f = ((x - z) * (x + z) + h * (y / (f + SVD_SIGN(g, f)) - h)) / x;
     
     // QR-��������������
     c = 1.0;
     s = 1.0;

     for (i1 = l; i1 <= k1; i1++)
     {
        i = i1 + 1;
        g = rv1[i];
        y = w[i];
        h = s * g;
        g = c * g;
        z = sqrt(f * f + h * h);
        rv1[i1] = z;
        c = f / z;
        s = h / z;
        f = x * c + g * s;
        g = -x * s + g * c;
        h = y * s;
        y = y * c;
        if (matv) 
          for (j = 0; j < n; j++)
          {
             x = V[j][i1];
             z = V[j][i];
             V[j][i1] = x * c + z * s;
             V[j][i] = -x * s + z * c;
          }

        z = sqrt(f * f + h * h);
        assert(z > 0);
        w[i1] = z;

        // �������� ����� ���� ������������, ���� z ����� ����

        if (z != 0.0)
        {
          c = f / z;
          s = h / z;
        }

        f = c * g + s * y;
        x = -s * g + c * y;
        
        if (matu)
          
          for (j = 0; j < m; j++)
          {
             y = U[j][i1];
             z = U[j][i];
             U[j][i1] = y * c + z * s;
             U[j][i] = -y * s + z * c;
          }

     }

     rv1[l] = 0.0;
     rv1[k] = f;
     w[k] = x;
     goto splitting_test;
   }

   // ����������

   if (z < 0.0) 
   {
     // w[k] �������� ���������������
     w[k] = -z;
     if (matv)
       for (j = 0; j < n; j++)
         V[j][k] = -V[j][k];
   }
 }

 nl_dvector_free(rv1);

}

double svd_cond(double *w, size_t n)
{
  double sigma_max, sigma_min;
  size_t j;
  
  sigma_max = sigma_min = w[0];
  
  for (j = 1; j < n; j++)
  {
    if (w[j] > sigma_max)
      sigma_max = w[j];
    if (w[j] < sigma_min)
      sigma_min = w[j];
  }

  return (sigma_max / sigma_min);
 
}

void svd_correct(double *w, size_t n, double rel_err)
{
  double sigma_max, mach_eps, tau;
  size_t j;
  
  sigma_max = 0;
  
  for (j = 0; j < n; j++)
    if (w[j] > sigma_max)
      sigma_max = w[j];

  if (!rel_err)
  {
    mach_eps = 1;
    do
      mach_eps /= 2;
    while((mach_eps + 1) > 1);
    mach_eps *= 2;
    rel_err = mach_eps * n;
  }

  tau = rel_err * sigma_max;
  
  for (j = 0; j < n; j++)
    if (w[j] < tau)
      w[j] = 0;
}

void svd_least_squares(double **U, double *w, double **V, size_t m, size_t n, double *b, double *x)
{
  size_t jj, j, i;
  double s, *tmp;

  tmp = nl_dvector_create(n);

  for (j = 0; j < n; j++) 
  {
    s = 0.0;
    if (w[j]) 
    {
      for (i = 0; i < m; i++) 
        s += U[i][j] * b[i];

      s /= w[j];
    }
    tmp[j] = s;
  }

  for (j = 0; j < n; j++) 
  {
    s = 0.0;
    for (jj = 0; jj < n; jj++) 
      s += V[j][jj] * tmp[jj];
 
    x[j] = s;
  }

  nl_dvector_free(tmp);
}

#undef SVD_SIGN
