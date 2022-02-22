#!/usr/bin/env python3

# https://plot.ly/python/gantt/

import plotly.figure_factory as ff
import pandas as pd

df = [
        dict(Task="Hall_POST",           Start='2019-09-01', Finish='2019-09-03', Complete=10, Resource='David'),
        dict(Task="Misc_POST",           Start='2019-09-01', Finish='2019-09-03', Complete=10, Resource='Bart'),
        dict(Task="POST_MCP",            Start='2019-09-01', Finish='2019-09-03', Complete=10, Resource='James'),
        dict(Task="Power_POST",          Start='2019-09-03', Finish='2019-09-06', Complete=10, Resource='David'),
        dict(Task="Filters (1/3 axis)",  Start='2019-09-03', Finish='2019-09-06', Complete=10, Resource='Bart'),
        dict(Task="Tasks",               Start='2019-09-03', Finish='2019-09-06', Complete=10, Resource='James'),
        dict(Task="FRAM",                Start='2019-09-06', Finish='2019-09-09', Complete=10, Resource='David'),
        dict(Task="Accelerometer",       Start='2019-09-06', Finish='2019-09-09', Complete=10, Resource='Bart'),
        dict(Task="Magnetometer & POST", Start='2019-09-06', Finish='2019-09-09', Complete=10, Resource='James'),
        dict(Task="Inner Loop",          Start='2019-09-09', Finish='2019-09-12', Complete=10, Resource='David'),
        dict(Task="AttitudeController",  Start='2019-09-09', Finish='2019-09-12', Complete=10, Resource='Bart'),
        dict(Task="MCPControlProducer",  Start='2019-09-09', Finish='2019-09-12', Complete=10, Resource='James'),
        dict(Task="CyclicFeedback",      Start='2019-09-12', Finish='2019-09-15', Complete=10, Resource='David'),
        dict(Task="UARModeLogic",        Start='2019-09-12', Finish='2019-09-15', Complete=10, Resource='Bart'),
        dict(Task="Gyro",                Start='2019-09-12', Finish='2019-09-15', Complete=10, Resource='James'),
        dict(Task="Signals MCP",         Start='2019-09-15', Finish='2019-09-18', Complete=10, Resource='David'),
        dict(Task="Target_SW_Defs_Cmn",  Start='2019-09-15', Finish='2019-09-18', Complete=10, Resource='Bart'),
        dict(Task="Clutch Module",       Start='2019-09-15', Finish='2019-09-18', Complete=10, Resource='James'),
        dict(Task="Override Detect",     Start='2019-09-18', Finish='2019-09-21', Complete=10, Resource='David'),
        dict(Task="User Input",          Start='2019-09-18', Finish='2019-09-21', Complete=10, Resource='Bart'),
        dict(Task="NVRAM Segments",      Start='2019-09-18', Finish='2019-09-21', Complete=10, Resource='James'),
        dict(Task="Config Mgmt",         Start='2019-09-21', Finish='2019-09-24', Complete=10, Resource='David'),
        dict(Task="MCP Main",            Start='2019-09-21', Finish='2019-09-24', Complete=10, Resource='Bart'),
        dict(Task="ServoLoop rewrite",   Start='2019-09-21', Finish='2019-09-24', Complete=10, Resource='James'),
        dict(Task="MCP_to_ADP_COMM",     Start='2019-09-24', Finish='2019-09-28', Complete=10, Resource='David')
     ]

colors = ['#7a0504', (0.2, 0.7, 0.3), 'rgb(210, 60, 180)']
#fig = ff.create_gantt(df)
#fig = ff.create_gantt(df, colors='Viridis', index_col='Complete', show_colorbar=True)
fig = ff.create_gantt(df, colors=colors, index_col='Resource', reverse_colors=True, show_colorbar=True)
fig.show()


