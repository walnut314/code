#!/usr/bin/env python3


# http://book.pythontips.com/en/latest/map_filter.html

items = [1, 2, 3, 4, 5]
squared = []
for i in items:
    squared.append(i**2)
print("{0} -> {1}".format(items, squared))

items = [1, 2, 3, 4, 5]
squared = list(map(lambda x: x**2, items))
print("{0} -> {1}".format(items, squared))

def multiply(x):
    return (x*x)
def add(x):
    return (x+x)

funcs = [multiply, add]
for i in range(5):
    value = list(map(lambda x: x(i), funcs))
    print(value)


number_list = range(-5, 5)
less_than_zero = list(filter(lambda x: x < 0, number_list))
print(less_than_zero)


product = 1
list = [1, 2, 3, 4]
for num in list:
    product = product * num
print(product)

from functools import reduce
product = reduce((lambda x, y: x * y), [1, 2, 3, 4])
print(product)


