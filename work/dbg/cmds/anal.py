#!/usr/bin/env python3

import subprocess

cmd_list = ['anal1.cmd','anal2.cmd','anal3.cmd','anal4.cmd','anal5.cmd','anal6.cmd','anal7.cmd','anal8.cmd']

def proc_dump(cmd):
    print(cmd)
    with subprocess.Popen([cmd], shell=True, stdout=subprocess.PIPE, stdin=subprocess.PIPE, stderr=subprocess.PIPE) as p:
        std_out, errors = p.communicate() #(str.encode("utf-8"))
        if std_out:
            out = std_out.splitlines()
            for ln in out:
                if len(ln):
                    print(ln)
        if errors:
            print("Error message: {}".format(error))
    
def main():
    for cmd in cmd_list:
        proc_dump(cmd)

main()

