import re

from config import *
from util import *

def test_module_traversal():
    modules = [ \
        'cldm',
        'main',
        'test',
        'unit'
    ]
    trivial_modules = [ \
        'cldm',
        'main',
        'unit'
    ]

    rv, output, _ = exec_bash('make -C {} DEBUG=1'.format(project_root))
    assert rv == 0

    output = output.decode('utf-8').split('\n')
    output = output[:len(output) - 1]
    output = ' '.join(output)

    for m in modules:
        assert re.search('include-module-prologue.*{}'.format(m), output)
        assert re.search('include-module-epilogue.*{}'.format(m), output)
    for tm in trivial_modules:
        assert re.search('declare-trivial-c-module.*{}'.format(tm), output)
