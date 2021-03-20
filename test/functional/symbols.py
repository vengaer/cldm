import json
import re
import os

from cldm import *
from config import *

def read_db():
    with open(symbol_db, 'r') as fd:
        return json.load(fd)

def list_gensymbs():
    return read_db()['symbols'].keys()

def gen_symbols():
    db = read_db()
    with open(symfile, 'w') as fd:
        fd.write('#include "cldm.h"\n')
        for inc in db['includes']:
            fd.write('#include <{}>\n'.format(inc))
        for struct in db['structs']:
            field_names = [n for n in db['structs'][struct]['members'].keys()]
            fd.write('struct {} {{\n'.format(struct))
            for name in field_names:
                fd.write('\t{} {};\n'.format(db['structs'][struct]['members'][name]['type'], name))
            fd.write('};\n')
        for symname in db['symbols']:
            sym = db['symbols'][symname]
            suffix = '0' if len(sym.get('params', [])) == 1 and sym['params'][0] == 'void' else ''
            param_delim = ', ' if len(sym.get('params', [])) else ''
            if sym['rettype'] == 'void':
                fd.write('MOCK_FUNCTION_VOID{}({}{} {});\n'.format(suffix, symname, param_delim, ', '.join(sym.get('params', []))))
            else:
                fd.write('MOCK_FUNCTION{}({}, {}{} {});\n'.format(suffix, sym['rettype'], symname, param_delim, ', '.join(sym.get('params', []))))

    return build_cldm()

def list_symbols():
    return os.popen('nm {}'.format(solib)).read()

def symbol_defined(symbol, nm_output):
    for line in nm_output.split('\n'):
        if re.search(r'^[0-9a-fA-F]+\s+T\s+{}'.format(symbol), line):
            print('Found symbol {}'.format(symbol))
            return True
    return False
