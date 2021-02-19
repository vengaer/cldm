from config import *

class CGen():
    def __init__(self, filename):
        self.filename = str(working_dir / filename)
        self.contents = []
        self.indent = 0

    def __append_content(self, string):
        self.contents.append('{}{}\n'.format('\t' * self.indent, string))

    def append_include(self, name, system_header=True):
        if system_header:
            osym = '<'
            csym = '>'
        else:
            osym = '"'
            csym = '"'

        self.__append_content('#inclue {}{}{}'.format(osym, name, csym))

    def close_scope(self):
        if self.indent == 0:
            raise ValueError('No open scope')
        self.indent = self.indent - 1
        self.__append_content('}')

    def open_function(self, rettype, name, params=['void']):
        self.__append_content('{} {}({}) {{'.format(rettype, name, ', '.join([p for p in params])))
        self.indent = self.indent + 1

    def close_function(self):
        self.close_scope()

    def append_return(self, retval):
        self.__append_content('return {};'.format(retval))

    def write(self):
        with open(self.filename, 'w') as fd:
            for line in self.contents:
                fd.write('{}'.format(line))
