import os
import pytest
import shutil

from config import *
from symbols import *
from util import *

crc = None

def pytest_runtest_setup():
    global crc

    if not crc or crc != buffered_crc(solib):
        try:
            os.mkdir(working_dir)
        except:
            pass

        assert gen_symbols()[0] == 0

    if not crc:
        ctc = buffered_crc(solib)

def pytest_runtest_teardown():
    shutil.rmtree(working_dir)

def do_cleanup():
    try:
        os.unlink(symfile)
    except:
        pass

    os.system('make -sC {} clean'.format(project_root))

@pytest.fixture(scope='session', autouse=True)
def cleanup(request):
    request.addfinalizer(do_cleanup)
