import os
from ctypes import *
from ctypes.util import find_library  

# add current directory
path = os.path.abspath(os.getcwd())
os.add_dll_directory(path)

# is this a windows issue?
dll = find_library(os.path.abspath('pazusoba.so'))
print(dll)
pazusoba = CDLL(dll)
print(pazusoba)
pazusoba.pazusoba(0, [])
