import 'dart:ffi';
import 'package:ffi/ffi.dart';

// Pointer<Utf8> is const char* and treat Pointer like an array
typedef pazusoba_func = Void Function(Int32, Pointer<Pointer<Utf8>>);
typedef Pazusoba = void Function(int, Pointer<Pointer<Utf8>>);

void main() {
  final libary = DynamicLibrary.open('pazusoba.so');
  final Pazusoba pazusoba = libary
      .lookup<NativeFunction<pazusoba_func>>('pazusoba')
      .asFunction<Pazusoba>();

  // Convert Dart string list to char **
  final list = [
    'pazusoba.exe',
    'GLHLGGLBDHDDDHGHHRDRLDDLGLDDRG',
    '3',
    '25',
    '1000',
  ];

  int argc = list.length;
  // Create a native char pointer
  final Pointer<Pointer<Utf8>> argv = malloc.allocate(argc);
  for (int i = 0; i < argc; i++) {
    argv[i] = list[i].toNativeUtf8();
  }

  // Call the native function
  pazusoba(argc, argv);
  // Free up memory
  for (int i = 0; i < argc; i++) {
    malloc.free(argv[i]);
  }
  malloc.free(argv);
}
