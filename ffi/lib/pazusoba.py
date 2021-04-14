from ctypes import *
cpp = CDLL("./helloworld.so")
cpp.printHelloWorld()
