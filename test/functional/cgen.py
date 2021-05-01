from contextlib import contextmanager

from cldm import *
from config import *
from symbols import *
from util import *

class CGen():
    def __init__(self, srcfile):
        self.srcfile = str(working_dir / srcfile)
        self.headerfile = self.srcfile.replace('.c', '.h')
        self.src = []
        self.header = []
        self.indent = 0

    @contextmanager
    def open_struct(self, name):
        self.__open_struct(name)
        try:
            yield
        finally:
            self.__close_struct()

    @contextmanager
    def open_union(self, name):
        self.__open_union(name)
        try:
            yield
        finally:
            self.__close_union()

    @contextmanager
    def open_function(self, retval, name, params=['void']):
        self.__open_function(retval, name, params)
        try:
            yield
        finally:
            self.__close_function()

    @contextmanager
    def open_for(self, itertype, itername, low, high):
        self.__open_for(itertype, itername, low, high)
        try:
            yield
        finally:
            self.__close_scope()

    @contextmanager
    def open_scope(self, prefix, semicolon=False):
        self.__open_scope(prefix)
        try:
            yield
        finally:
            self.__close_scope(semicolon)

    @contextmanager
    def open_macro(self, macro, optargs=[]):
        self.__open_macro(macro, optargs)
        try:
            yield
        finally:
            self.__close_scope()

    def __append_src_content(self, string):
        self.src.append('{}{}\n'.format('\t' * self.indent, string))

    def __append_header_content(self, string):
        self.header.append('{}\n'.format(string))

    def default_call(name, params=['void']):
        return '{}({})'.format(name, '' if len(params) == 1 and params[0] == 'void' else ', '.join(['({}){{ 0 }}'.format(t) for t in params]))

    def append_include(self, name, system_header=True):
        if system_header:
            osym = '<'
            csym = '>'
        else:
            osym = '"'
            csym = '"'

        self.__append_src_content('#include {}{}{}'.format(osym, name, csym))
        return self

    def __open_scope(self, prefix):
        self.__append_src_content('{} {{'.format(prefix))
        self.indent = self.indent + 1
        return self

    def __close_scope(self, semicolon=False):
        if self.indent == 0:
            raise ValueError('No open scope')
        self.indent = self.indent - 1
        self.__append_src_content('{}'.format('};' if semicolon else '}'))

    def __open_struct(self, name):
        self.__append_src_content('struct {} {{'.format(name))
        self.indent = self.indent + 1
        return self

    def __close_struct(self):
        self.__close_scope(True)
        return self

    def __open_union(self, name):
        self.__append_src_content('union {} {{'.format(name))
        self.indent = self.indent + 1
        return self

    def __close_union(self):
        self.__close_scope(True)
        return self

    def __open_for(self, itertype, itername, low, high):
        low = str(low)
        high = str(high)
        self.__append_src_content(f'for({itertype} {itername} = {low}; {itername} < {high}; {itername}++) {{')
        self.indent = self.indent + 1
        return self


    def __open_function(self, rettype, name, params=['void']):
        if len(params) == 1 and params[0] == 'void':
            self.__append_src_content('{} {}(void) {{'.format(rettype, name))
            if name != 'main':
                self.__append_header_content('{} {}(void);'.format(rettype, name))
        else:
            self.__append_src_content('{} {}({}) {{'.format(rettype, name, ', '.join([p + ' a{}'.format(i) for i, p in enumerate(params)])))
            if name != 'main':
                self.__append_header_content('{} {}({});'.format(rettype, name, ', '.join([p + ' a{}'.format(i) for i, p in enumerate(params)])))
        self.indent = self.indent + 1
        return self

    def __close_function(self):
        self.__close_scope()
        return self

    def __open_macro(self, macro, optargs=[]):
        if not type(optargs) == list:
            optargs = [ optargs ]
        self.__append_src_content('{}({}) {{'.format(macro, ', '.join(optargs)))
        self.indent = self.indent + 1
        return self

    def __close_macro(self):
        self.__close_scope()
        return self

    def append_return(self, retval):
        self.__append_src_content('return {};'.format(retval))
        return self

    def generate_matching_symbols(self):
        db = read_db()

        for inc in db['includes']:
            self.append_include(inc)
        for struct in db['structs']:
            field_names = [n for n in db['structs'][struct]['members'].keys()]
            with self.open_struct(struct):
                for name in field_names:
                    self.__append_src_content('{} {};'.format(db['structs'][struct]['members'][name]['type'], name))
        for symname in db['symbols']:
            sym = db['symbols'][symname]
            if not sym.get('generate', True):
                continue
            with self.open_function(sym['rettype'], symname, sym.get('params', ['void'])):
                if len(sym.get('params', [])) != 1 or sym['params'][0] != 'void':
                    for i, _ in enumerate(sym.get('params', [])):
                        self.__append_src_content('(void)a{};'.format(i))

                for call in sym.get('exec', []):
                    self.__append_src_content(call)

                if sym['rettype'] != 'void':
                    self.append_return(sym.get('return', '({}){{ 0 }}'.format(sym['rettype'])))

        assert build_cldm()[0] == 0
        return self

    def append_line(self, line):
        self.__append_src_content(line)
        return self

    def write(self):
        with open(self.srcfile, 'w') as fd:
            for line in self.src:
                fd.write(line)
        if len(self.header):
            with open(self.headerfile, 'w') as fd:
                for line in self.header:
                    fd.write(line)
