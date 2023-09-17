import pandas as pd
import numpy as np
from sklearn.pipeline import Pipeline
from sklearn.impute import SimpleImputer
from sklearn.preprocessing import MinMaxScaler

data = pd.DataFrame([[1], [4], [np.NaN], [8], [11]], columns=['A'])

pl = Pipeline([
    ('imp', SimpleImputer(strategy = "mean")),
    ('scalar', MinMaxScaler())
    ])

print(pl.fit_transform(data))

