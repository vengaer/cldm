import os
import pytest
import shutil

from config import *
from symbols import *
from util import *

md5 = None

def pytest_runtest_setup():
    global md5

    try:
        os.mkdir(working_dir)
    except:
        pass

    if md5 is None or md5 != buffered_md5(solib):
        assert gen_symbols()[0] == 0

    if md5 is None:
        md5 = buffered_md5(solib)

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
