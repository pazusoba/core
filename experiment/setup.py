from distutils.core import setup, Extension

module_pazusoba = Extension('pazusoba',
                    sources = ['pazusoba.cpp', 'api.cpp'],
                    extra_compile_args=['-std=c++11'])

setup(name = 'pazusoba',
       version = '1.0.0',
       license='MIT',
       url='https://github.com/pazusoba',
       description = 'Solve Puzzle & Dragons with C++ in Python',
       ext_modules = [module_pazusoba])