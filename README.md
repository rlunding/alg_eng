# Algorithm Engineering: Project 1

This project is all about optimizing binary searches. Different layouts impact the number of cache misses as well as the amount of instructions to navigate in the array. We consider storing the elements by in-order (sorted), BFS order, DFS order and van Emde Boas layout.

## Building
The project can be built with CMake and GNU Make. The executables are located in `/output`.

To build, navigate to `/output` and invoke `cmake ..` (for Windows, `cmake -G "MinGW Makefiles" ..` instead) and then `make all` (for Windows, `mingw32-make all` instead).

## Data generation
`datagen` is a utility that generates data and stores them in files. Invoke `datagen <n> <filename>` to generate an array of -2n, -2n + 2, ..., 2n (2n + 1 elements in total), stores query specification in `<filename>` and layout the array in `<filename>.<layoutname>`.

For example, if we want to generate data of size 2049 we can run `datagen 1024 my_data`. If the invocation is successful, `my_data` will contain `2049 -2150 2150`, indicating that the length of the array is 2049, the recommended lower bound (resp. upper bound) of a random query is -2150 (resp. 2150). And in `my_data.bfs` one finds the BFS order of the data.

## Benchmark
The `benchmark` program reads layouts from previous invocation of `datagen`, makes random queries to the data structures and measures the approximate number of cycles taken per query. It generates a tabular output to `stdout` (error information goes to `stderr`) and creates a plot with `gnuplot`.

Run `benchmark` for a comprehensive description of the usage. An example is:

```
benchmark -i:dataset1 -i:dataset2 -r -q:500000 -s:0
```

There is also a pair of arguments, `algorithm` and `dont`, to be used with external profilers.

### Searching algorithms
We have 10 searching algorithms in total. Below is an exhaustive list of them.

| Abbreviation | Meaning | Description |
| :----------: | :-----: | :---------- |
| `ino:s`      | In-order, Stable | Binary search in a sorted array. Always returns the pointer to the **last** element `x` such that `x <= y`. |
| `ino:u`      | In-order, Unstable | Binary search in a sorted array. Returns the pointer to **one of** the largest elements `x` such that `x <= y`. |
| `bfs:s`      | BFS order, Stable | Binary search in a **complete** binary search tree, arranged in BFS order in the array. Here complete means every level of the tree is fully filled, except possibly the last level, in which leaves are located to the left. |
| `bfs:u`      | BFS order, Unstable | Unstable version of the above. |
| `dfs:s`      | DFS order, Stable | Binary search in a binary search tree, arranged in DFS order in the array. In this binary search tree, any left subtree is **perfect**, and is as large as possible. |
| `dfs:u`      | DFS order, Unstable | Unstable version of the above. |
| `veb:r:s`    | van Emde Boas layout, Recursive, Stable | Binary search in a binary search tree, whose left subtrees are arranged in van Emde Boas layout in the array and whose right subtrees' roots are arranged consecutively in the array. Searching happens among the right subtrees' roots and then in a left subtree. Searching in a large left subtree is completed with two recursive calls into the top and bottom subtree, as in van Emde Boas layout. |
| `veb:r:u`    | van Emde Boas layout, Recursive, Unstable | Unstable version of the above. |
| `veb:b:s`    | van Emde Boas layout, BFS index, Stable | Searching in a large left subtree is completed as if it is searching in BFS layout. The loop tracks the BFS index of the pivot (with fast bit operations) and use an algorithm to convert BFS index to van Emde Boas layout index. The conversion uses a simple loop or tail recursion. |
| `veb:b:u`    | van Emde Boas layout, BFS index, Unstable | Unstable version of the above. |

## Test
It is assumed that the unstable inorder implementation (plain sorted array binary search) is correct. Other implementations are executed and the results are compared against corresponding invocations to unstable inorder implementation.

To run the test, place `testcase`, `testcase.ino`, `testcase.bfs`, `testcase.dfs` and `testcase.veb`, generated from a invocation to `datagen`, in the working directory and invoke `testing`.

## Notices
We use [Catch](https://github.com/philsquared/Catch) as our testing framework.
