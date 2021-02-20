from contextlib import contextmanager

from config import *
from symbols import *

class CGen():
    def __init__(self, srcfile):
        self.srcfile = str(working_dir / srcfile)
        self.headerfile = self.srcfile.replace('.c', '.h')
        self.src = []
        self.header = []
        self.indent = 0

    @contextmanager
    def with_open_struct(self, name):
        self.open_struct(name)
        try:
            yield
        finally:
            self.close_struct()

    @contextmanager
    def with_open_function(self, retval, name, params=['void']):
        self.open_function(retval, name, params)
        try:
            yield
        finally:
            self.close_function()

    def __append_src_content(self, string):
        self.src.append('{}{}\n'.format('\t' * self.indent, string))

    def __append_header_content(self, string):
        self.header.append('{}\n'.format(string))

    def default_call(name, params=['void']):
        return '{}({});'.format(name, '' if len(params) == 1 and params[0] == 'void' else ', '.join(['({}){{ 0 }}'.format(t) for t in params]))

    def append_include(self, name, system_header=True):
        if system_header:
            osym = '<'
            csym = '>'
        else:
            osym = '"'
            csym = '"'

        self.__append_src_content('#include {}{}{}'.format(osym, name, csym))
        return self

    def __close_scope(self, semicolon=False):
        if self.indent == 0:
            raise ValueError('No open scope')
        self.indent = self.indent - 1
        self.__append_src_content('{}'.format('};' if semicolon else '}'))

    def open_struct(self, name):
        self.__append_src_content('struct {} {{'.format(name))
        self.indent = self.indent + 1
        return self

    def close_struct(self):
        self.__close_scope(True)
        return self

    def open_function(self, rettype, name, params=['void']):
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

    def close_function(self):
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
            with self.with_open_struct(struct):
                for name in field_names:
                    self.__append_src_content('{} {};'.format(db['structs'][struct]['members'][name]['type'], name))
        for symname in db['symbols']:
            sym = db['symbols'][symname]
            with self.with_open_function(sym['rettype'], symname, sym.get('params', ['void'])):
                if len(sym.get('params', [])) != 1 or sym['params'][0] != 'void':
                    for i, _ in enumerate(sym.get('params', [])):
                        self.__append_src_content('(void)a{};'.format(i))

                for call in sym.get('exec', []):
                    self.__append_src_content(call)

                if sym['rettype'] != 'void':
                    self.append_return(sym.get('return', '({}){{ 0 }}'.format(sym['rettype'])))

        os.system('make -B -C {}'.format(project_root))
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
