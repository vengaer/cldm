import re

from config import *
from util import *

class Matcher:
    def run(self, output=None, optarg=None):
        raise NotImplementedError()

    def _preproc_output(self, output):
        output = output.decode('utf-8').split('\n')
        return output[:len(output) - 1]

class EqMatcher(Matcher):
    def __init__(self, exp):
        self.exp = exp

    def run(self, output):
        assert self.exp == self._preproc_output(output)

class ContainsMatcher(Matcher):
    def __init__(self, pattern):
        self.pattern = pattern

    def run(self, output):
        assert re.search(self.pattern, ' '.join(self._preproc_output(output)))

class ContainsOnceMatcher(ContainsMatcher):
    def __init__(self, pattern, exclusive):
        super().__init__(pattern)
        self.exclusive = exclusive

    def run(self, output):
        super().run(output)
        output = self._preproc_output(output)
        for e in self.exclusive:
            assert output.count(e) == 1

def run(matcher):
    assert exec_bash('make -C {}'.format(working_dir))[0] == 0
    retval, output, _ = exec_bash('make -sC {} run'.format(working_dir))
    assert retval == 0
    matcher.run(output)
