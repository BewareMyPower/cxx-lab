# duplicated-symbols

This project shows a common case that duplicated symbols are introduced.

Run the following command:

```bash
./build.sh
```

You might see the output:

```
# plugin1 run
foo
# plugin2 run
foo
```

However, it's wrong. Let's continue to see why it's wrong.

First, the project organization is:

```
example -> plugin1 -> common
        -> plugin2 -> common-v1
```

The executable `example` links to two C++ **dynamic** libraries `plugin1` and `plugin2`. `plugin1` links to the `common` library and `plugin2` links to the `common-v1` library. 

Let's assume `common` and `common-v1` are the same library of different versions, so we can see they share the same interface ([common.h](./common/common.h)):

```c++
void foo();
```

They have different implementations that `common` prints "foo" and `common-v1` prints `foo v1`. We can see `plugin2` depends on `common-v1` in its [CMakeLists.txt](./plugin2/CMakeLists.txt).

```cmake
target_link_libraries(plugin2 PRIVATE common-v1)
```

The root cause is that `common` and `common-v1` are static libraries and both of them are linked implicitly from `plugin1` and `plugin2`. When multiple static libraries that have the same symbol are linked, the first static library will be linked actually and the second static library will be skipped.

You can go to [simplified](./simplified) directory for a simple test. It build two executables from the same source codes. Unfortunately, the outputs are different.

```
$ cd ./simplified
$ ./build.sh
r - foo1.o
r - foo2.o
+ ./1.out
foo1
+ ./2.out
foo2
```
