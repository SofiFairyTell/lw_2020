#ifndef __FFT_H__
#define __FFT_H__

/**
  \file
  ������� ������������� �����
  
  (<i>������</i>) <i>���������� ��������������� �����</i> �������� 
  ����������� \f$\varphi: {\bf C}^n \to {\bf C}^n\f$,
  �������� � ������������ ������� \f$x=(x_0, x_1, \dots, x_{n-1})\f$
  ������ \f$\widehat{x}=(\widehat{x}_0,\widehat{x}_1,\dots,\widehat{x}_{n-1})\f$, ���
  \f$\widehat{x}_k = \displaystyle\sum_{j=0}^{n-1} x_j e^{\displaystyle-\frac{jk2\pi i}{n}}\f$
  \f$(k=0,1,\dots,n-1)\f$.

  �������� ����������� \f$\varphi^{-1}\f$ ���������� 
  <i>�������� ���������� ��������������� �����</i>.
  ��� ����� ��������� �� ��������
  \f$x_j = \frac{\displaystyle 1}{\displaystyle n} \displaystyle\sum_{k=0}^{n-1} \widehat{x}_k e^{\displaystyle\frac{jk2\pi i}{n}}\f$
  \f$(j=0,1,\dots,n-1)\f$.

  <i>������� �������������� �����</i> - ��� ����������� ������ ����������
  ������� � ��������� �������������� �����. � ���������� ���������� ��������
  �������� �������������� ����� ��� ������, ����� \f$n\f$ - ������� ������.
  ����� ������������ ������� ��� �������� ������ �����, ���������� �������� �
  �������� ������� ����������.
*/

/**
  \example xfft.c
*/


/**
  ������ ������� �������������� ����� ��� ������� \f$x\f$ ����� \f$n\f$.
  �������������� ����� ������� \f$x\f$ �������� � ������� \f$re\f$,
  ������ ����� - � ������� \f$im\f$,
  \f$n\f$ ������ ���� �������� ������.
  ��������� ������������ � �������� \f$re\f$ � \f$im\f$.
*/
extern void fft_transform(double* re, double* im, size_t n);


/**
  �������� ������� �������������� ����� ��� ������� \f$x\f$ ����� \f$n\f$.
  �������������� ����� ������� \f$x\f$ �������� � ������� \f$re\f$,
  ������ ����� - � ������� \f$im\f$,
  \f$n\f$ ������ ���� �������� ������.
  ��������� ������������ � �������� \f$re\f$ � \f$im\f$.
*/
extern void fft_inverse(double* re, double* im, size_t n);

#endif