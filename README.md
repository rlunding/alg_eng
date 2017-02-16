# Algorithm Engineering

This project is all about optimizing binary searches 
in arrays. We consider `BFS`, `DFS`, `INLINE`(sorted) 
and `vEB` (van Emde Boas layout).

We build the project with CMake. The executables are 
located in the folder called `/output`.

## Benchmarks
Out benchmark executable is used for performing 
benchmarks on the different layouts according to the 
four strategies meantioned above.

To run the benchmark use:
```commandline
> ./benchmark [input_file]
```

For more options use

```commandline
> ./benchmark help
```

## Test program
We have written a bunch of tests for the different 
implementations. They can be executed with the command

```commandline
> ./testing
```

