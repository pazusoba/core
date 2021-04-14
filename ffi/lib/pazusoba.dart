import 'dart:ffi';
import 'package:ffi/ffi.dart';

typedef pazusoba_func = Void Function(Int32, Pointer<Pointer<Int8>>);
typedef Pazusoba = void Function(int, Pointer<Pointer<Int8>>);

void main() {
  final libary = DynamicLibrary.open('helloworld.so');
  final Pazusoba pazusoba = libary
      .lookup<NativeFunction<pazusoba_func>>('pazusoba')
      .asFunction<Pazusoba>();
  final Pointer<Pointer<Int8>> list = malloc();
  pazusoba(0, list);
}
