from pathlib import Path

testdir = Path(__file__).parent.absolute()
project_root = testdir.parent.absolute()
libdir = project_root / 'libmockc'
symfile = libdir / 'pytest_gensym.c'
solib = project_root / 'libmockc.so'
symbol_db = testdir / 'symdb.json'
working_dir = testdir / 'running'
