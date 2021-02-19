import os
import pytest
import shutil

from config import *

def pytest_runtest_setup():
    try:
        os.mkdir(working_dir)
    except:
        pass

def pytest_runtest_teardown():
    try:
        os.unlink(symfile)
    except:
        pass

    shutil.rmtree(working_dir)

    os.system('make -sC {} clean'.format(project_root))
