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

def test_stack():
    with open(working_dir / 'Makefile', 'w') as fp:
        expected = [    \
            ':foo',
            'foo',
            ':bar foo',
            'bar',
            ':foo',
            'foo',
            ':baz foo',
            'baz',
            ':foo',
            'foo',
            ':',
            ''
        ]

        fp.write('include {}/scripts/stack.mk\n'.format(project_root))
        fp.write('stack :=\n')
        fp.write('$(call stack-push,stack,foo)\n')
        fp.write('$(info $(stack))\n')
        fp.write('$(info $(call stack-top,stack))\n')
        fp.write('$(call stack-push,stack,bar)\n')
        fp.write('$(info $(stack))\n')
        fp.write('$(info $(call stack-top,stack))\n')
        fp.write('$(call stack-pop,stack)\n')
        fp.write('$(info $(stack))\n')
        fp.write('$(info $(call stack-top,stack))')
        fp.write('$(call stack-push,stack,baz)\n')
        fp.write('$(info $(stack))\n')
        fp.write('$(info $(call stack-top,stack))\n')
        fp.write('$(call stack-pop,stack)\n')
        fp.write('$(info $(stack))\n')
        fp.write('$(info $(call stack-top,stack))\n')

        fp.write('$(call stack-pop,stack)\n')
        fp.write('$(info $(stack))\n')
        fp.write('$(info $(call stack-top,stack))\n')
        fp.write('.PHONY: all\n')
        fp.write('all:\n')

    rv, output, _ = exec_bash('make -sC {}'.format(working_dir))
    assert rv == 0

    output = output.decode('utf-8').split('\n')
    output = output[:len(output) - 1]

    assert expected == [o.strip() for o in output]

