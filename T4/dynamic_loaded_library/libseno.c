#include <stdio.h>
#define NUMBER_OF_TERMS 10

double factorial(int number){
  double result = number;

  while (number > 1) {
    result *= (number-1);
    number--;
  }

  return result;
}

double double_factorial(int number){
  double result = number;

  if (number <= 1){
    result = 1;
  } else {
    while (number > 2) {
      result *= (number-2);
      number = number - 2;
    }
  }

  return result;
}

double power(double base, double exponent){
  double result = 1.0;

  if (exponent != 0) {
    for (int count = 0; count < exponent; count++){
      result = result*base;
    }
  }

  return result;
}

double sine(double angle){
  double result = 0.0;

  for (int term = 0; term < NUMBER_OF_TERMS; term++){
    result += power(-1, term)*power(angle, 2*term+1)/factorial(2*term+1);
  }

  return result;
}

double arcsine(double angle){
  double result = 0.0;

  for (int term = 0; term < NUMBER_OF_TERMS; term++){
    result += (double_factorial(2*term-1)/double_factorial(2*term))*(power(angle, 2*term+1)/(2*term+1));
  }

  return result;
}
