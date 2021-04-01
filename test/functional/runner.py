import re

from config import *
from util import *

class Matcher:
    def run(self, opt):
        raise NotImplementedError()

class DummyMatcher(Matcher):
    def run(self, arg):
        pass

class TextMatcher(Matcher):
    def __init__(self, ref):
        self.ref = ref

    def _preproc(output):
        output = output.decode('utf-8').split('\n')
        return output[:len(output) - 1]

class RvMatcher(Matcher):
    def __init__(self, ref):
        self.ref = ref

class EqMatcher(TextMatcher):
    def run(self, output):
        assert self.ref == TextMatcher._preproc(output)

class ContainsMatcher(TextMatcher):
    def run(self, output):
        assert re.search(self.ref, ' '.join(TextMatcher._preproc(output)))

class ContainsOnceMatcher(ContainsMatcher):
    def __init__(self, pattern, exclusive):
        super().__init__(pattern)
        self.exclusive = exclusive

    def run(self, output):
        super().run(output)
        output = TextMatcher._preproc(output)
        for e in self.exclusive:
            assert output.count(e) == 1

class RvEqMatcher(RvMatcher):
    def run(self, rv):
        assert rv == self.ref

class RvDiffMatcher(RvMatcher):
    def run(self, rv):
        assert rv != self.ref

def run(stdout_matcher, stderr_matcher=DummyMatcher(), rvmatcher=RvEqMatcher(0)):
    assert exec_bash('make -C {}'.format(working_dir))[0] == 0
    retval, output, error = exec_bash('make -sC {} run'.format(working_dir))
    rvmatcher.run(retval)
    stdout_matcher.run(output)
    stderr_matcher.run(error)

    return retval, output, error

