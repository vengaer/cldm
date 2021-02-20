from config import *
from symbols import *

def test_symbol_generation():
    symlist = list_symbols()

    for sym in list_gensymbs():
        assert symbol_defined(sym, symlist)
