"""
Test if the binding is working or not
"""
# import pazusoba
from ctypes import cdll

# NOTE: make sure you run `make dll` first and drag the .so file into this folder
pazusoba = cdll.LoadLibrary("./pazusoba.so")
# it seems that string is not linked... what should I do?
pazusoba.hello()
