#!/usr/bin/env python3

from pandas_datareader import data as pdr
import pandas as pd
import yfinance as yf
import datetime

today = datetime.date.today()
weekday = today.weekday()
yesterday = today - datetime.timedelta(days=1)
start_delta = datetime.timedelta(days=weekday, weeks=1)
lastMonday = today - start_delta
week_delta = datetime.timedelta(days=4)
lastFriday = lastMonday+week_delta

print('today:       {0}'.format(today))
#print('weekday {0}'.format(weekday))
print('yesterday:   {0}'.format(yesterday))

print('last monday: {0}'.format(lastMonday))
print('last friday: {0}'.format(lastFriday))


