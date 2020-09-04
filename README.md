## About
Small C compiler written in C.
I aim to implement the self-hosting. (not yet)

## How to use

First, build a binary of compiler as follows:

```
$ make
```

Next, input a C source code into the binary as follows:

```
$ ./main <C-source file>
```

## Test

You can do all unittests as follows:

```
$ make test
```

## Requirement
Linux
(my environment: Ubuntu 18.04.5 LTS (Bionic Beaver))

## References

- Rui Ueyama, “低レイヤを知りたい人のためのCコンパイラ作成入門”. <https://www.sigbus.info/compilerbook>
- Rui Ueyama, [chibicc](https://github.com/rui314/chibicc)

