#!/usr/bin/env python3

from pandas_datareader import data as pdr
import yfinance as yf
import numpy as np
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
ticker_list = ['SPY', 'XLE', 'OXY', 'QCOM', 'AAPL']

#weekly
start_week = lastMonday
end_week = lastFriday

#daily
start_day = yesterday
end_day = yesterday

#today

def getData(ticker, start_date, end_date):
    data = None
    try:
        data = pdr.get_data_yahoo(ticker, start_date, end_date)
    finally:
        return data

def show_week(ticker, now, week):
    print('week: {0}'.format(ticker))
    week_signal = False
    now_high  = now['High'].values[0]
    now_open  = now['Open'].values[0]
    now_close = now['Close'].values[0]
    length = len(week['Close'].values)
    high  = np.zeros(length)
    open  = np.zeros(length)
    close = np.zeros(length)
    for i in range(length):
        high[i] = week['High'].values[i]
        open[i] = week['Open'].values[i]
        close[i] = week['Close'].values[i]
    prev_week_high  = np.amax(high)
    prev_week_open  = np.amax(open)
    prev_week_close = np.amax(close)
    if (now_high > prev_week_high) and (now_open > prev_week_open) and (now_close > prev_week_close):
        week_signal = True
        print('good week signal {0}'.format(ticker))
    return week_signal

def show_day(ticker, now, day):
    print('day: {0}'.format(ticker))
    day_signal = False
    now_high  = now['High'].values[0]
    now_open  = now['Open'].values[0]
    now_close = now['Close'].values[0]
    prev_day_high  = day['High'].values[0]
    prev_day_open  = day['Open'].values[0]
    prev_day_close = day['Close'].values[0]
    bid = yf.Ticker(ticker).info['bid']
    opi = bid-now_open
    npi = bid-prev_day_close
    #print('OPI: {0}'.format(opi))
    #print('NPI: {0}'.format(npi))
    if (now_high > prev_day_high) and (now_open > prev_day_open) and (now_close > prev_day_close) and (opi > 0.50) and (npi > 0.50):
        day_signal = True
    if opi > 0.50 and npi > 0.50 and day_signal == True:
        print('buy signal {0}'.format(ticker))
    return day_signal

def main():
    print('week: ', start_week, ' to ', end_week)
    print('day:  ', start_day, ' to ', end_day)
    for ticker in ticker_list:
        print()
        try:
            now = pdr.get_data_yahoo(ticker, today, today)
            week  = pdr.get_data_yahoo(ticker, start_week, end_week)
            day   = pdr.get_data_yahoo(ticker, start_day, end_day)
        finally:
            #print()
            #if now is not None:
            #    print(now)
            if week is not None:
                week_signal = show_week(ticker, now, week)
            if day is not None:
                day_signal = show_day(ticker, now, day)
            if week_signal == True and day_signal == True:
                print('DUDE!!! buy {0}'.format(ticker))

if __name__ == "__main__":
    main()    

