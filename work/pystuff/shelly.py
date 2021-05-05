#!/usr/bin/env python3

from subprocess import check_output
from datetime import datetime
import time
import logging
import random

svc   = "SurfaceService"
start = "sc start %s" % (svc)
stop  = "sc stop  %s" % (svc)
quantum = 50.0/1000.0 # milliseconds
iteration = 0

logging.basicConfig(filename='app.log', filemode='a', level=logging.DEBUG, format='%(asctime)s : %(name)s - %(levelname)s - %(message)s')

now = datetime.now()
current_time = now.strftime("%H:%M:%S")
print("Start Time =", current_time)
logging.info("start test")

try:
    while True:
        ## generate a random byte
        n = 1
        random_bytes = bytes([random.randrange(33, 126) for _ in range(0, n)])
        randi = ord(random_bytes.decode())
        delay_time = (randi/1000.0) * quantum
        #print(delay_time)

        out = check_output(stop, shell=True).decode()
        #out = stop
        print(out)
        logging.info(out)
        time.sleep(delay_time)
        out = check_output(start, shell=True).decode()
        #out = start
        print(out)
        logging.info(out)
        iteration += 1
        str_iter = "iter %d" % (iteration)
        print(str_iter)
        logging.info(str_iter)
        time.sleep(delay_time)
except KeyboardInterrupt:
    print("you sick?")

now = datetime.now()
current_time = now.strftime("%H:%M:%S")
print("End Time =", current_time)
logging.info("end test")

