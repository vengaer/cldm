import hashlib
import os

from subprocess import Popen, PIPE

def exec_bash(command):
    proc = Popen(['/usr/bin/env', 'bash', '-c', command], stdin=PIPE, stdout=PIPE, stderr=PIPE, bufsize=-1)
    proc.wait()
    output, error = proc.communicate()
    return proc.returncode, output, error

def buffered_md5(file):
    h = hashlib.md5()
    with open(file, 'rb') as fp:
        for c in iter(lambda: fp.read(4096), b''):
            h.update(c)
    return h.hexdigest()
