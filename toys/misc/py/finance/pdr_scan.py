#!/usr/bin/env python3

from pandas_datareader import data as pdr
import pandas as pd
import yfinance as yf
import datetime

# calculate all dates
today = datetime.date.today()
weekday = today.weekday()
yesterday = today - datetime.timedelta(days=1)
start_delta = datetime.timedelta(days=weekday, weeks=1)
lastMonday = today - start_delta
week_delta = datetime.timedelta(days=4)
lastFriday = lastMonday+week_delta

# Tickers list
ticker_list = ['SPY', 'XLK', 'AAPL']

#weekly
start_week = lastMonday
end_week = lastFriday

#daily
start_day = yesterday
end_day = yesterday

def getData(ticker, start_date, end_date):
    data = None
    try:
        data = pdr.get_data_yahoo(ticker, start_date, end_date)
    finally:
        return data

def main():
    print('week: ', start_week, ' to ', end_week)
    print('day:  ', start_day, ' to ', end_day)
    for ticker in ticker_list:
        print()
        print(ticker)
        try:
            week = pdr.get_data_yahoo(ticker, start_week, end_week)
            day  = pdr.get_data_yahoo(ticker, start_day, end_day)
        finally:
            print()
            if week is not None:
                print(week)
            print()
            if day is not None:
                print(day)

if __name__ == "__main__":
    main()    

