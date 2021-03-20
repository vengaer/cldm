from config import *
from util import *

TEST_PREFIX = 'cldm_test_proc_'
LOCAL_SETUP = 'cldm_local_setup'
LOCAL_TEARDOWN = 'cldm_local_teardown'
GLOBAL_SETUP = 'cldm_global_setup'
GLOBAL_TEARDOWN = 'cldm_global_teardown'

def build_cldm():
    return exec_bash('make -j$(nproc) -B -C {} CFLAGS="{}"'.format(project_root, libcflags))
