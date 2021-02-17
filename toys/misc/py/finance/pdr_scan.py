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
[start_week, end_week] = [lastMonday, lastFriday]
[start_day, end_day] = [yesterday, yesterday]

# Tickers list
ticker_list = ['SPY', 'XLE', 'OXY']
# globals
[now_high, now_open, now_close]                   = [0, 0, 0]
[prev_day_high, prev_day_open, prev_day_close]    = [0, 0, 0]
[prev_week_high, prev_week_open, prev_week_close] = [0, 0, 0]

def getData(ticker, start_date, end_date):
    data = None
    try:
        data = pdr.get_data_yahoo(ticker, start_date, end_date)
    finally:
        return data

def opi_npi(ticker, now, day):
    opi_npi_signal = False
    bid = yf.Ticker(ticker).info['bid']
    [opi, npi] = [bid - now_open, bid - prev_day_close]
    #if opi > 0.25 and npi > 0.25:
    #    print('long {0}'.format(ticker))
    if opi > 0.50 and npi > 0.50:
        opi_npi_signal = True
    return opi_npi_signal

def show_day(ticker, now, day):
    day_signal = False
    if (now_high > prev_day_high) and (now_open > prev_day_open) and (now_close > prev_day_close):
        day_signal = True
    return day_signal

def show_week(ticker, now, week):
    week_signal = False
    if (now_high > prev_week_high) and (now_open > prev_week_open) and (now_close > prev_week_close):
        week_signal = True
    return week_signal

def main():
    for ticker in ticker_list:
        [now, week, day]                                  = [None, None, None]
        [week_signal, day_signal, opi_npi_signal]         = [False, False, False]
        [now_high, now_open, now_close]                   = [0, 0, 0]
        [prev_day_high, prev_day_open, prev_day_close]    = [0, 0, 0]
        [prev_week_high, prev_week_open, prev_week_close] = [0, 0, 0]
        try:
            [now, week, day]   = [pdr.get_data_yahoo(ticker, today, today), 
                                  pdr.get_data_yahoo(ticker, start_week, end_week),
                                  pdr.get_data_yahoo(ticker, start_day, end_day)]
        finally:
            if week is not None and day is not None and now is not None:
                [now_high, now_open, now_close] = [now['High'].values[0], now['Open'].values[0], now['Close'].values[0]]
                [prev_day_high, prev_day_open, prev_day_close] = [day['High'].values[0], day['Open'].values[0], day['Close'].values[0]]

                length = len(week['Close'].values)
                [high, open, close]  = [np.zeros(length), np.zeros(length), np.zeros(length)]
                for i in range(length):
                    [high[i], open[i], close[i]] = [week['High'].values[i], week['Open'].values[i], week['Close'].values[i]]
                [prev_week_high, prev_week_open, prev_week_close]  = [np.amax(high), np.amax(open), np.amax(close)]
                [week_signal, day_signal, opi_npi_signal] = [show_week(ticker, now, week), show_day(ticker, now, day), opi_npi(ticker, now, day)]

            print('{0} week: {1}, day: {2}, opi/npi: {3}'.format(ticker, week_signal, day_signal, opi_npi_signal))
            if week_signal == True and day_signal == True and opi_npi_signal == True:
                print('DUDE!!! buy {0}'.format(ticker))

if __name__ == "__main__":
    main()    

