from config import *
from util import *

test_prefix = 'cldm_testproc_'
local_setup = 'cldm_local_setup'
local_teardown = 'cldm_local_teardown'
global_setup = 'cldm_global_setup'
global_teardown = 'cldm_global_teardown'

test_failure_status = 1

def build_cldm():
    return exec_bash('make -j$(nproc) -B -C {} CFLAGS="{}"'.format(project_root, libcflags))
