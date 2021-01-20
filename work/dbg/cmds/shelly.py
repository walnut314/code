#!/usr/bin/env python3

from subprocess import check_output
from datetime import datetime
import time
import logging

svc   = "SurfaceService"
start = "net start %s" % (svc)
stop  = "net stop  %s" % (svc)
delay = 10
iteration = 0

logging.basicConfig(filename='app.log', filemode='a', level=logging.DEBUG, format='%(asctime)s : %(name)s - %(levelname)s - %(message)s')

now = datetime.now()
current_time = now.strftime("%H:%M:%S")
print("Start Time =", current_time)
logging.info("start test")

try:
    while True:
        out = check_output(stop, shell=True).decode()
        #out = stop
        print(out)
        logging.info(out)
        time.sleep(delay)
        out = check_output(start, shell=True).decode()
        #out = start
        print(out)
        logging.info(out)
        iteration += 1
        str_iter = "iter %d" % (iteration)
        print(str_iter)
        logging.info(str_iter)
        time.sleep(delay)
except KeyboardInterrupt:
    print("you sick?")

now = datetime.now()
current_time = now.strftime("%H:%M:%S")
print("End Time =", current_time)
logging.info("end test")

