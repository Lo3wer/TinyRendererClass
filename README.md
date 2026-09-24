# Tiny Renderer

A small C++17 software renderer based on the [TinyRenderer tutorial](https://haqr.eu/tinyrenderer/).

## Prerequisites

- A C++17 compiler, such as `g++` or `clang++`
- GNU Make, or CMake 3.10+

## Build and Run with Make

From the repository root:

```sh
make
make run
```

The default command renders `obj/african_head/african_head.obj`. To render one or more other models, pass their paths through `MODEL`:

```sh
make run MODEL="obj/diablo3_pose/diablo3_pose.obj"
make run MODEL="obj/diablo3_pose/diablo3_pose.obj obj/boggie/head.obj"
```

The rendered image is written to `framebufferphong.tga` in the repository root. Remove compiled files with:

```sh
make clean
```

## Build and Run with CMake

From the repository root:

```sh
cmake -S . -B build
cmake --build build
./build/renderer obj/african_head/african_head.obj
```

On Windows, run the generated executable from the repository root so that the bundled texture paths resolve correctly. The output file is still `framebufferphong.tga` in the current working directory.
