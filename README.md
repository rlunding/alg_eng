# Algorithm Engineering: Project 1

This project is about studying different memory layouts for binary search. Different layouts impact the number of cache misses as well as the amount of instructions to navigate in the array. We consider storing the elements by in-order (sorted), BFS order, DFS order and van Emde Boas layout.

## Building

To compile the project, clone this repository and clone [PCM](https://github.com/opcm/pcm.git). Put the root directory of both repositories under the same directory. Next, compile PCM with `pcm/PCM_Winpcm.vcxproj` with Release x64 configuration and copy the object files (`pcm/PCM_Win/x64/Release/*.obj`) to the root folder of PCM repository clone `pcm`. Then, under `alg_eng/output`, run `cmake .. -G "Visual Studio 14 2015 Win64"` (replace the version with appropriate Visual Studio version) and build `alg_eng/output/alg_eng.sln` with Release x64 configuration. Finally, copy everything under `alg_eng/external` to `alg_eng/output/Release`, where the binary files are located. The `alg_eng/CMakeLists.txt` directs the code to be compiled with flags `/O2 /Ot /Og /Oi /Oy /Ox /Ob2`, which basically means maximal optimisation for speed.

The automation utilities are PowerShell scripts (`alg_eng/misc/*.ps1`).

## Test

It is assumed that the unstable inorder implementation (plain sorted array binary search) is correct. Other implementations are executed and the results are compared against corresponding invocations to unstable inorder implementation.

To run the test, place `testcase`, `testcase.ino`, `testcase.bfs`, `testcase.dfs` and `testcase.veb`, generated from a invocation to `datagen`, in the working directory and invoke `testing`.


## Data generation

`datagen` is a utility that generates data and stores them in files. Invoke `datagen <n> <filename>` to generate an array of -2n, -2n + 2, ..., 2n (2n + 1 elements in total), stores query specification in `<filename>` and layout the array in `<filename>.<layoutname>`.

For example, if we want to generate data of size 2049 we can run `datagen 1024 my_data`. If the invocation is successful, `my_data` will contain `2049 -2053 2053`, indicating that the length of the array is 2049, the recommended lower bound (resp. upper bound) of a random query is -2053 (resp. 2053). And in `my_data.bfs` one finds the BFS order of the data.

To generate data for benchmarking, run \inlinecode{Get-DataGenerator} and copy the resulting code to the clipboard, then create a folder named \inlinecode{data} under the folder of the binary files, and finally invoke the commands copied to the clipboard in Command Prompt.

## Benchmark

The `bmpcm` program reads layouts from previous invocation of `datagen`, makes random queries to the data structures and measures the approximate number of cycles taken per query. It generates a tabular output to `stdout` (error information goes to `stderr`) and creates a plot with `gnuplot`.

Run `bmpcm` for a comprehensive description of the usage. An example is:

```
bmpcm -i:dataset1 -i:dataset2 -q:500000 -s:0
```

There is also a `bmpapi`, which gives results that are not accurate.

To do the benchmarking, run `Get-BenchmarkCode` and save the resulting code to a batch file under the folder of the binary file, then open Command Prompt with administrator privilege under that folder and finally invoke the batch. It is recommended that the network be cut and other programs be quitted to minimize flucutations.

## Data processing

To analyse the data with Excel, run `ConvertTo-ExcelWorkbook` under each directory of results. In the default setting these will be `output/Release/results/24601`, `output/Release/results/271828` and `output/Release/results/314159`.

### Searching algorithms

We have 10 searching algorithms in total. Below is an exhaustive list of them.

| Abbreviation | Meaning | Description |
| :----------: | :-----: | :---------- |
| `ino.s`      | In-order, Stable | Binary search in a sorted array. Always returns the pointer to the **last** element `x` such that `x <= y`. |
| `ino.u`      | In-order, Unstable | Binary search in a sorted array. Returns the pointer to **one of** the largest elements `x` such that `x <= y`. |
| `bfs.s`      | BFS order, Stable | Binary search in a **complete** binary search tree, arranged in BFS order in the array. Here complete means every level of the tree is fully filled, except possibly the last level, in which leaves are located to the left. |
| `bfs.u`      | BFS order, Unstable | Unstable version of the above. |
| `bfs.cs`      | BFS order, Conditional move, Stable | Uses `?:` operator in the hope that Visual Studio generates `cmov*` instructions. |
| `bfs.ps`      | BFS order, Prefetch, Stable | Uses `_mm_prefetch` to prefetch the data into the cache line. |
| `bfs.cps`      | BFS order, Conditional move, Prefetch, Stable | A mixture of the above two. |
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
