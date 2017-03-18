# Algorithm Engineering: Project 1

This project is all about optimizing binary searches. Different layouts impact the number of cache misses as well as the amount of instructions to navigate in the array. We consider storing the elements by in-order (sorted), BFS order, DFS order and van Emde Boas layout.

## Building

The project can be built with CMake and GNU Make. The executables are located in `/output`.

To build, navigate to `/output` and invoke `cmake ..` and then `make all`.

## Data generation

`datagen` is a utility that generates data and stores them in files. Invoke `datagen <n> <filename>` to generate an array of -2n, -2n + 2, ..., 2n (2n + 1 elements in total), stores query specification in `<filename>` and layout the array in `<filename>.<layoutname>`.

For example, if we want to generate data of size 2049 we can run `datagen 1024 my_data`. If the invocation is successful, `my_data` will contain `2049 -2053 2053`, indicating that the length of the array is 2049, the recommended lower bound (resp. upper bound) of a random query is -2053 (resp. 2053). And in `my_data.bfs` one finds the BFS order of the data.

## Benchmark

The `bmpcm` program reads layouts from previous invocation of `datagen`, makes random queries to the data structures and measures the approximate number of cycles taken per query. It generates a tabular output to `stdout` (error information goes to `stderr`) and creates a plot with `gnuplot`.

Run `bmpcm` for a comprehensive description of the usage. An example is:

```
bmpcm -i:dataset1 -i:dataset2 -q:500000 -s:0
```

There is also a `bmpapi`, which gives results that are not accurate.

### Searching algorithms

We have 10 searching algorithms in total. Below is an exhaustive list of them.

| Abbreviation | Meaning | Description |
| :----------: | :-----: | :---------- |
| `ino.s`      | In-order, Stable | Binary search in a sorted array. Always returns the pointer to the **last** element `x` such that `x <= y`. |
| `ino.u`      | In-order, Unstable | Binary search in a sorted array. Returns the pointer to **one of** the largest elements `x` such that `x <= y`. |
| `bfs.s`      | BFS order, Stable | Binary search in a **complete** binary search tree, arranged in BFS order in the array. Here complete means every level of the tree is fully filled, except possibly the last level, in which leaves are located to the left. |
| `bfs.u`      | BFS order, Unstable | Unstable version of the above. |
| `dfs.s`      | DFS order, Stable | Binary search in a binary search tree, arranged in DFS order in the array. In this binary search tree, any left subtree is **perfect**, and is as large as possible. |
| `dfs.u`      | DFS order, Unstable | Unstable version of the above. |
| `veb.rs`    | van Emde Boas layout, Recursive, Stable | Binary search in a binary search tree, whose left subtrees are arranged in van Emde Boas layout in the array and whose right subtrees' roots are arranged consecutively in the array. Searching happens among the right subtrees' roots and then in a left subtree. Searching in a large left subtree is completed with two recursive calls into the top and bottom subtree, as in van Emde Boas layout. |
| `veb.ru`    | van Emde Boas layout, Recursive, Unstable | Unstable version of the above. |
| `veb.ns`    | van Emde Boas layout, Inlined + Recursive, Stable | The same algorithm with recursion expanded for all height < 32. |
| `veb.nu`    | van Emde Boas layout, Inlined + Recursive, Unstable | Unstable version of the above. |
| `veb.is`    | van Emde Boas layout, Recursive, Stable | Searching in a large left subtree is completed with an explicit stack. |
| `veb.iu`    | van Emde Boas layout, Recursive, Unstable | Unstable version of the above. |
| `veb.bs`    | van Emde Boas layout, BFS index, Stable | Searching in a large left subtree is completed as if it is searching in BFS layout. The loop tracks the BFS index of the pivot (with fast bit operations) and use an algorithm to convert BFS index to van Emde Boas layout index. The conversion uses a simple loop or tail recursion. |
| `veb.bu`    | van Emde Boas layout, BFS index, Unstable | Unstable version of the above. |

## Test

It is assumed that the unstable inorder implementation (plain sorted array binary search) is correct. Other implementations are executed and the results are compared against corresponding invocations to unstable inorder implementation.

To run the test, place `testcase`, `testcase.ino`, `testcase.bfs`, `testcase.dfs` and `testcase.veb`, generated from a invocation to `datagen`, in the working directory and invoke `testing`.

## Comprehensive example

Having built `all`, run the following commands:

```
datagen 24601 testcase
testing

datagen 36902 dataset00
datagen 55352 dataset01
datagen 83028 dataset02
datagen 124543 dataset03
datagen 186814 dataset04
datagen 280221 dataset05
datagen 420331 dataset06
datagen 630497 dataset07
datagen 945745 dataset08
datagen 1418618 dataset09
datagen 2127926 dataset10
datagen 3191890 dataset11
datagen 4787834 dataset12
datagen 7181752 dataset13
datagen 10772628 dataset14
datagen 16158941 dataset15
datagen 24238412 dataset16
datagen 36357618 dataset17
datagen 54536427 dataset18
datagen 81804641 dataset19
bmpcm -i:dataset00 -i:dataset01 -i:dataset02 -i:dataset03 -i:dataset04 -i:dataset05 -i:dataset06 -i:dataset07 -i:dataset08 -i:dataset09 -i:dataset10 -i:dataset11 -i:dataset12 -i:dataset13 -i:dataset14 -i:dataset15 -i:dataset16 -i:dataset17 -i:dataset18 -i:dataset19 -q:100000 -r:100000

```

## Data processing

The script that converts bunches of output (`*.pcm.*`) to an interactive Excel workbook can be found at `/misc/ConvertTo-InteractiveData.ps1`.

## (Legal) Notices

We use [Catch](https://github.com/philsquared/Catch) as our testing framework.

The ring 0 driver (in the `external` folder) for Windows comes from RealTemp 3.7, available [here](http://www.techpowerup.com/realtemp/).

The pmem driver comes from the Rekall Framework, available [here](https://github.com/google/rekall/tree/df4c820de7b3a21bd241336c99c12b3b9dde4015/tools/pmem/resources/winpmem).

Both drivers are recommended by developers on Intel Developer Zone, as seen in [this post](https://software.intel.com/en-us/forums/software-tuning-performance-optimization-platform-monitoring/topic/542911).

### RealTemp copyright text

Copyright (c) 2007-2008 OpenLibSys.org. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

### The Rekall Memory Forensic Framework copyright

Copyright (C) 2007-2011 Volatile Systems
Copyright 2012-2016 Google Inc. All Rights Reserved.

All Rights Reserved

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
