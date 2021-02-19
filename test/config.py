from pathlib import Path

project_root = Path(__file__).parent.parent.absolute()
libdir = project_root / 'libmockc'
symfile = libdir / 'pytest_gensym.c'
solib = project_root / 'libmockc.so'
symbol_db = project_root / 'test/symdb.json'

