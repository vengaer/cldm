from config import *
from util import *

def build_cldm():
    return exec_bash('make -j$(nproc) -B -C {} CFLAGS="{}"'.format(project_root, libcflags))

