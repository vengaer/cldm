import os
import pytest

from config import *

def pytest_runtest_teardown():
    try:
        os.unlink(symfile)
    except:
        pass

    os.system('make -C {} clean'.format(project_root))
