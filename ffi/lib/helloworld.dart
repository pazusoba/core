import 'dart:ffi' as ffi;

typedef cFunc = ffi.Void Function();
typedef printHelloWorld = void Function();

void main() {
  final libary = ffi.DynamicLibrary.open('helloworld.so');
  final printHelloWorld hello =
      libary.lookup<ffi.NativeFunction<cFunc>>('printHelloWorld').asFunction();
  hello();
  // print('Hello World');
}
