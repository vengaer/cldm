import os
from subprocess import Popen, PIPE

def exec_bash(command):
    proc = Popen(['/usr/bin/env', 'bash', '-c', command], stdin=PIPE, stdout=PIPE, stderr=PIPE, bufsize=-1)
    proc.wait()
    output, error = proc.communicate()
    return proc.returncode, output, error
