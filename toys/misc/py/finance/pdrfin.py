#!/usr/bin/env python3

import csv
import sys

from pandas_datareader import data as pdr
import pandas as pd
import yfinance as yf
from datetime import datetime
import numpy as np
import matplotlib.pyplot as plt

# Tickers list
# We can add and delete any ticker from the list to get desired ticker live data
#ticker_list = ['AMZN', 'MSFT', 'WDC', 'FB', 'AAPL']
ticker_list = ['XLK', 'SPY', 'AAPL', 'OXY', 'IVV']

today = datetime.today()
# We can get data by our choice by giving days bracket
start_date = datetime(2021, 2, 12) #'2017–01–01'
end_date   = datetime(2021, 2, 12) #'2019–11–30'
files=[]
def getData(ticker):
    print (ticker)
    data = None
    try:
        #data = pdr.get_data_yahoo(ticker, start=start_date, end=today)
        data = pdr.get_data_yahoo(ticker, start=start_date, end=end_date)
    finally:
        return data
#    data = pdr.get_data_yahoo(ticker, start="2017-08-13",end="2017-08-14")    
#   dataname = ticker + '_' + str(today)
#   files.append(dataname)
#   SaveData(data, dataname)

# Create a data folder in your current dir.
#def SaveData(df, filename):
#    df.to_csv('./data/'+filename+'.csv')

#This loop will iterate over ticker list, will pass one ticker to get data, and save that data as file.
# for tik in ticker_list:

def analyze(tik):
    data = getData(tik)
    if data is None:
        return
#    for dp in data:
#        print(dp)
    length = len(data['Close'].values)
#   print(len(data['Close'].values))
#   print(data)
#   data.to_csv(r'%s.csv' % tik) 

    t = np.linspace(0, 1-1/length, length)
    f = np.arange(length)/(length)
    y = np.zeros(length)
    yt = np.zeros(length, dtype=complex)

    """
    for v in data['Close'].values:
        fv = float(v)
        print(fv)
    """
    
    for i in range(length):
        y[i] = data['Close'].values[i]

    yt = np.fft.fft(y)
    yt[0] = 0
#   yt[1] = 0

#   print(yt)
    # find max amplitude in frequency
    max_freq = -1
    max_bin = -1
    for i in range(length):
        if yt[i].imag > max_freq:
            max_freq = yt[i].imag
            max_bin = i

    freqs = np.fft.fftfreq(len(yt))
    print("min, max")
    print(freqs.min(), freqs.max())
    # Find the peak in the coefficients
    idx = np.argmax(np.abs(yt))
    freq = freqs[idx]
    frate = 1
    freq_in_hertz = abs(freq * frate)
    print("freq at max")
    print(freq_in_hertz)

    print(max_bin, max_freq)

#   """
    plt.subplot(1, 2, 1) # Left plot
    #ax = plt.gca()
    #ax.set_xticks(np.arange(0, N, 4))
    #ax.set_yticks(np.arange(0, N, 4))
    plt.grid()
    plt.plot(t, y)
    plt.title('Original time series')
    plt.xlabel('Time')

    plt.subplot(1, 2, 2) # Right plot
    ax = plt.gca()
    ax.set_xticks(np.arange(0, 1.0, 0.15))
    #ax.set_yticks(np.arange(0, N, 4))
    plt.plot(f, np.real(yt), '-', f, np.imag(yt), '--')
    plt.legend(['Real', 'Imaginery '])
    plt.grid()
    plt.title('Fourier transform')
    plt.xlabel('Frequency')

    plt.show()
#   """

def process(stock_csv):
    f = open(stock_csv, 'rt')
    try:
        reader = csv.reader(f)
        #headers = reader.next() # python 2.7
        headers = next(reader) # python 3
        #print("headers: %s" % headers)
        for row in reader:
#           print(row[0])
            analyze(row[0])

    finally:
        f.close()

def getData2(ticker, start_datum, end_datum):
    print (ticker)
    data = None
    try:
        data = pdr.get_data_yahoo(ticker, start_datum, end_datum)
    finally:
        return data

year = '2021'
month = '2'
day = '12'
start_datum = year + '-' + month + '-' + day
end_datum   = year + '-' + month + '-' + day

def all_data(tik):
    print(start_datum)
    print(end_datum)
    data = getData2(tik, start_datum, end_datum)
    if data is None:
        return
    print(data)

def main():
    for tik in ticker_list:
        all_data(tik)
#       analyze(tik)

#     process('nyse.csv');
#     process('nasdaq.csv');
#     process('amex.csv');

if __name__ == "__main__":
    # execute only if run as a script
    main()    

