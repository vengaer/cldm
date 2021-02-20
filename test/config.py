from pathlib import Path

testdir = Path(__file__).parent.absolute()
project_root = testdir.parent.absolute()
libdir = project_root / 'cmock'
symfile = libdir / 'mockups.h'
solib = project_root / 'libcmock.so'
symbol_db = testdir / 'symdb.json'
working_dir = testdir / 'running'
