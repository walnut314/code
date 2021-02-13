#!/usr/bin/env python3

from pandas_datareader import data as pdr
import pandas as pd
import yfinance as yf
from datetime import datetime

# Tickers list
ticker_list = ['XLK', 'SPY', 'AAPL', 'OXY', 'IVV']

today = datetime.today()
# We can get data by our choice by giving days bracket
start_date = datetime(2021, 2, 12) #'2017–01–01'
end_date   = datetime(2021, 2, 12) #'2019–11–30'
year = '2021'
month = '2'
day = '12'
start_datum = year + '-' + month + '-' + day
end_datum   = year + '-' + month + '-' + day

def getData(ticker, start_datum, end_datum):
    data = None
    try:
        data = pdr.get_data_yahoo(ticker, start_datum, end_datum)
    finally:
        return data

def main():
    print(start_datum, ' to ', end_datum)
    for tik in ticker_list:
        print (tik)
        data = getData(tik, start_datum, end_datum)
        if data is None:
            return
        print(data)

if __name__ == "__main__":
    main()    

