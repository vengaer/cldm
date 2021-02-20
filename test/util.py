import os

def exec_sh(command):
    return os.system(command) >> 8
