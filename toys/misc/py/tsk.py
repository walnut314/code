#!/usr/bin/env python3

import pandas as pd
from sklearn.preprocessing import StandardScaler

X = pd.DataFrame({'A':[1,2,3,2], 'B':[11,1,8,3]})
scaler = StandardScaler()
scaler.fit(X)

print(scaler.mean_)
print(scaler.var_)

print("hello, world")

