from config import *
from symbols import *

def test_symbol_generation():
    missing_symbs = []
    gen_symbols()
    symlist = list_symbols()

    for sym in list_gensymbs():
        assert symbol_defined(sym, symlist)

