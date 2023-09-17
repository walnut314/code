import numpy as np
import pandas as pd

print("Series:\n")
list1 = [1,2,3,4,5]
print(list1, end='\n\n')

series1 = pd.Series(list1)
print(series1, end='\n\n')

series2 = pd.Series(list1, index=['pa','qb','rn','sa','tx'])
print(series2, end='\n\n')

print("DataFrames:\n")
myList = [[1,2,3], [4,5,6]]
df1 = pd.DataFrame(myList)
print(df1, end='\n\n')

myDict = {'A':[1,2,3], 'B':[4,5,6]}
df2 = pd.DataFrame(myDict)
print(df2, end='\n\n')

myList2 = [[1,2,3,4,5], [10,20,30,40,50]]
df3 = pd.DataFrame(myList2, index = ['A', 'B'], columns = ['1st','2nd','3rd','4th','5th'])
print(df3, end='\n\n')

