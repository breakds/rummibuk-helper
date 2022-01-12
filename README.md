[![built with nix](https://builtwithnix.org/badge.svg)](https://builtwithnix.org)

# Rummikub Helper

## Description

This provides a solver for the game Rummikub called `brok`. It is a homophonic name after "broke", but also stands for the 4 colors in the regular rummibuk game (b = blue, r = red, o = orange, and k = black).

Given a **subset** a full set of Rummikub tiles, the solver `brok` will try to find a solution that

1. forms valid sets (i.e. runs and groups, see [wikipedia](https://en.wikipedia.org/wiki/Rummikub#Sets) for details)
2. by using all the tiles in the subset

If solution exists, the solver will print one of the solutions. Otherwise it will tell you that there is no solution.

## How to use it?

### Approach 1: Build From Source

```bash
$ cd rummikub-helper
$ nix develop
$ mkdir build
$ cd build
$ cmake ..
$ make
```

After building it, you should be able to run it by feeding a file containing a list of tiles ([example](./problems/example3.txt)) to it.

As an example:

![brok1](https://user-images.githubusercontent.com/1111035/147836136-c7478eee-5108-4f2c-883e-24022fe1801f.png)

Where `B` stands for blue, `R` stands for red, `O` stands for orange and `K` stands for black.

### Approach 2: Run Directly Via Nix

Alternatively, if you have `nix` there is no need to clone this repo at all in case you just want to run it.

This should be as simple as

```
$ nix run "github:breakds/rummikub-helper" /the/path/to/to/the/problem.txt
```

## Algorithm

The problem is essentially an graph-theoretical optimization problem of "finding exact coverage". Such problems are commonly solved by

1. Posing it as an integer programming problem and use integer programming solver to solve it. Example: [paper](https://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.95.9619&rep=rep1&type=pdf)
2. Brute-force search with heuristics

To my best knowledge, the most efficient algorithm to this has polynomial complexity, which is based on dynamic programming as proposed in [The Complexity of Rummikub Problems](https://arxiv.org/pdf/1604.07553.pdf) and [this answer](https://cs.stackexchange.com/questions/88180/how-to-determine-the-maximum-valued-play-in-rummikub).

The solver `brok` as presented in this repo follows approach 2, i.e. it is a brute-force tree search with heuristics. I do it this way because it is easy to implement and alreayd extremely efficient.
