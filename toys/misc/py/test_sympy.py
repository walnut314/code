#!/usr/bin/env python3

import sympy
from sympy import expand, symbols, factor
  
x, y = symbols('x y')
gfg_exp = x + y
exp = sympy.expand(gfg_exp**2)
  
# Use sympy.factor() method
fact = factor(exp)
  
print(fact)
