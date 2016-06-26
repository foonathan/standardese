# standardese

[![Build Status](https://travis-ci.org/foonathan/standardese.svg?branch=master)](https://travis-ci.org/foonathan/standardese)
[![Build status](https://ci.appveyor.com/api/projects/status/1aw8ml5lawu4mtyv/branch/master?svg=true)](https://ci.appveyor.com/project/foonathan/standardese/branch/master)
[![Join the chat at https://gitter.im/foonathan/standardese](https://badges.gitter.im/foonathan/standardese.svg)](https://gitter.im/foonathan/standardese?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

> Note: This is an early prototype and highly WIP.
> Many features are missing and there are probably bugs everywhere.

Standardese aims to be a nextgen [Doxygen](http://doxygen.org).
It consists of two parts: a library and a tool.

The library aims at becoming *the* documentation frontend that can be easily extended and customized.
It parses C++ code with the help of [libclang](http://clang.llvm.org/doxygen/group__CINDEX.html) and provides access to it.

The tool drives the library to generate documentation for user-specified files.
It currently only supports Markdown as an output format but might be extended in the future.

Read more in the introductory [blog post](http://foonathan.github.io/blog/2016/05/06/standardese-nextgen-doxygen.html).

## Basic example

Consider the following C++ header file:

```cpp
#include <type_traits>

namespace std
{

    /// \effects Exchanges values stored in two locations.
    /// \requires Type `T` shall be `MoveConstructible` and `MoveAssignable`.
    template <class T>
    void swap(T &a, T &b) noexcept(is_nothrow_move_constructible<T>::value &&
                                    is_nothrow_move_assignable<T>::value);
}
```

This will generate the following documentation:

# Header file `swap.cpp`


```cpp
#include <type_traits>

namespace std
{
    template <typename T>
    void swap(T & a, T & b) noexcept(is_nothrow_move_constructible<T>::value &&
    is_nothrow_move_assignable<T>::value);
}
```


## Function template ``swap<T>``


```cpp
template <typename T>
void swap(T & a, T & b) noexcept(is_nothrow_move_constructible<T>::value &&
is_nothrow_move_assignable<T>::value);
```


*Effects:* Exchanges values stored in two locations.

*Requires:* Type `T` shall be `MoveConstructible` and `MoveAssignable`.

---

The example makes it already clear:
Standardese aims to provide a documentation in a similar way to the C++ standard - hence the name.

This means that it provides commands to introduce so called *sections* in the documentation.
The current sections are all the C++ standard lists in `[structure.specifications]/3` like `\effects`, `\requires`, `\returns` and `\throws`.

For a more complete example check out [this gist](https://gist.github.com/foonathan/14e163b76804b6775d780eabcbaa6a51).

## Installation

Standardese uses [CMake](https://cmake.org/) as build system.
Simply clone the project and run `cmake -DSTANDARDESE_BUILD_TEST=OFF --build . --target install` to build the library and the tool and install it on your system.

Both require libclang - only tested with version `3.7.1` and `3.8`.
If it isn't found, set the CMake variable `LIBCLANG_INCLUDE_DIR` to the folder where `clang-c/Index.h` is located,
`LIBCLANG_LIBRARY` to the library binary and `LIBCLANG_SYSTEM_INCLUDE_DIR` where the system include files are located,
under a normal (Linux) installation it is `/usr/lib/clang/<version>/include`.

The library requires Boost.Wave (at least 1.55) and the tool requires Boost.ProgramOptions and Boost.Filesystem, only tested with 1.60.
By default, Boost libraries are linked dynamically (except for Boost.ProgramOptions which is always linked statically),
but if you wish to link them statically, just add `-DBoost_USE_STATIC_LIBS=ON` to the cmake command.

Once built, simply run `standardese --help` for commandline usage.

## Documentation 

> Disclaimer: Due to the lack of proper tooling there is currently no good documentation.
> If you need help or encounter a problem please contact me [on gitter](https://gitter.im/foonathan/standardese?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge), I'll usually answer within a day or faster.

### Basic commandline usage

The tool uses Boost.ProgramOptions for the options parsing, `--help` gives a good overview.

Basic usage is: `standardese [options] inputs`

The inputs can be both files or directories, in case of directory each file is documented, unless an input option is specified (see below).
The tool will currently generate a corresponding Markdown file with the documentation for each file it gets as input.

> Note: You only need and should give header files to standardese.
> The source files are unnecessary.

The options listed under "Generic options:" must be given to the commandline.
They include things like getting the version, enabling verbose output (please provide it for issues) or passing an additional configuration file.

The options listed under "Configuration" can be passed both to the commandline and to the config file.
They are subdivided into various sections:

* The `input.*` options are related to the inputs given to the tool.
They can be used to filter, both the files inside a directory and the entities in the source code.

* The `compilation.*` options are related to the compilation of the source.
You can pass macro definitions and include directories as well as a `commands_dir`.
This is a directory where a `compile_commands.json` file is located.
standardese will pass *all* the flags of all files to libclang.

> This has technical reasons because you give header files whereas the compile commands use only source files.

* The `comment.*` options are related to the syntax of the documentation markup.
You can set both the leading character and the name for each command.

* The `output.*` options are related to the output generation.
Currently you can only set the name printed for a section.

The configuration file you can pass with `--config` uses an INI style syntax, e.g:

```
[compilation]
include_dir=foo/
macro_definition=FOO

[input]
blacklist_namespace=detail
extract_private=true
```

### Basic CMake usage

To ease the compilation options, you can call standardese from CMake like so:

```
find_package(standardese REQUIRED) # find standardese after installation

# generates a custom target that will run standardese to generate the documentation
standardese_generate(my_target CONFIG path/to/config_file
                     INCLUDE_DIRECTORY ${my_target_includedirs}
                     INPUT ${headers})
```

It will use a custom target that runs standardese.
You can specify the compilation options and inputs directly in CMake to allow shared variables.
All other options must be given in a config file.

See `standardese-config.cmake` for a documentation of `standardese_generate()`.

### Documentation syntax overview

standardese currently requires comments with triple slashes `///` for documentation.

Inside the comments you can use the basic inline Markdown syntax (`` ` ``, `*`, etc.) for formatting.

> You can currently use *arbitrary* Markdown because the comments are just copied to the output file
> which is Markdown itself.
> But this *will* change in a future version.

Special commands are introduced by the *command character* (a backslash by default).
It currently only supports *sections*.

A section is the basic way of standardese documentation.
It supports all the sections the C++ standard uses, as explained in the example.
Those sections will create a paragraph in the output prefixed with a human readable name.
A section will end on the next line or when a new section comes.
If you use the same sections two or more times in a row it will be detected and merged.

There are two special sections, `brief` and `details`.
They are not labeled in the output and will be used in future versions.
If you do not specify any sections the first line of the comment will be `brief` and all other lines `details`.

## Acknowledgements

Thanks a lot to:

* Manu @Manu343726 Sánchez, as always

* Jason @jpleau Pleau, for much feedback and requests on Gitter

* Mark @DarkerStar Gibbs, for feature suggestions

* Tristan @tcbrindle Brindle, for (better) clang support

* Marek @mkurdej Kurdej, for (better) MSVC support

* Victor @vitaut Zverovich, for bugfixes

And everyone else who shares and uses this project!
