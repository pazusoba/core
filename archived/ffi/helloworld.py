from ctypes import *
hello_world = CDLL("./helloworld.so")
hello_world.printHelloWorld()
