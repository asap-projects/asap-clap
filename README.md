# C++ command line arguments parser

![Start Now!!](doc/_static/logo.png "ASAP Logo")

[![Build Matrix](https://github.com/asap-projects/asap-clap/actions/workflows/cmake-build.yml/badge.svg?branch=master)](https://github.com/asap-projects/asap-clap/actions/workflows/cmake-build.yml)

## [Project Documentation](https://asap-projects.github.io/asap-clap/master/html/)

## Overview

This is a C++ command line arguments parser with the popular features of other
command line parsers, while keeping an intuitive interface and avoiding
unnecessary bloat.

## Getting the code

```bash
git clone --recurse-submodules -j4 https://github.com/asap-projects/asap-clap.git
```

NOTES:

- -j4 requests git to parallelize cloning of repos. Needs a relatively recent
  version of git. If that is not available, simply do not use this option.

## Requirements

Make sure you have a C++ compiler with C++-17 capabilities at least. Gnu, Clang
and MSVC all can do that with a recent version.

## Building

```bash
mkdir _build && cd _build && cmake .. && cmake --build .
```

or just use one of the predefined `CMake` presets. Detailed instructions are in
the project documentation, and many useful commands are listed
[here](https://abdes.github.io/asap/asap_master/html/getting-started/useful-commands.html).

## CMake configurable build options

```cmake
# Project options
option(BUILD_SHARED_LIBS        "Build shared instead of static libraries."              ON)
option(ASAP_BUILD_TESTS         "Build tests."                                           OFF)
option(ASAP_BUILD_EXAMPLES      "Build examples."                                        OFF)
option(ASAP_WITH_GOOGLE_ASAN    "Instrument code with address sanitizer"                 OFF)
option(ASAP_WITH_GOOGLE_UBSAN   "Instrument code with undefined behavior sanitizer"      OFF)
option(ASAP_WITH_GOOGLE_TSAN    "Instrument code with thread sanitizer"                  OFF)
option(ASAP_WITH_VALGRIND       "Builds targets with valgrind profilers added"           OFF)
```

## Making changes to this project

Read the developer guides in the upstream
[asap](https://abdes.github.io/asap/master/html/)
project. If you're in a hurry, at least do the following:

Only one time after the project is cloned, do the following:

```bash
npx husky install
npm install -g @commitlint/cli @commitlint/config-conventional
npm install -g standard-version
```

## Getting updates from upstream [`asap`](https://github.com/abdes/asap)

In order to pull and merge updates from the upstream project, make sure to add
it to the repo's remotes and disable pulling/merging tags from the upstream. We
want tags to be limited to those made in this repo, not in the upstream.

```bash
git remote add upstream https://github.com/abdes/asap.git
git config remote.upstream.tagopt --no-tags
```