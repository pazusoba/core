from distutils.core import setup, Extension
import platform

compilor_args = ['-std=c++11']
if platform.system() == 'Windows':
    compilor_args = []

module_pazusoba = Extension('pazusoba',
                            sources=['pazusoba.cpp', 'api.cpp'],
                            extra_compile_args=compilor_args)

setup(name='pazusoba',
      version='1.0.0',
      license='MIT',
      url='https://github.com/pazusoba',
      description='Solve Puzzle & Dragons with C++ in Python',
      ext_modules=[module_pazusoba])
