#!/usr/bin/env python3

import xlwings as xw
import pandas as pd


wb = xw.Book()  # this will create a new workbook
sht = wb.sheets['Sheet1']

df = pd.DataFrame([[1,2], [3,4]], columns=['a', 'b'])
sht.range('A1').value = df
sht.range('A1').options(pd.DataFrame, expand='table').value



