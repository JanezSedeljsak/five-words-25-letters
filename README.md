# Five words 25 letters

Rust solver for the five words 25-letter challange.

## The Problem
Find all unique sets of 5 words (len == 5) from a txt file (370k+ words) that together contain exactly 25 unique letters of the English alphabet.

## The Algorithm (core idea)

The rust based bitwise magic implementation can be devided into a few steps:

- first we read in the whole file into a str and then filter out lines with 5 characters with no repeated characters using bitmasks.

- we create the masks and mark seen masks in a vector for constant lookup + we also remove angorams (keep only the first one).

- build the lookup table for each letter (bucket) and order the letters by rarity - we will search for the least common letters first (Q, X, J).

- we use a stack to keep track of the current path and search for solutions using depth-first search (recursion is expensive).

- target is reached once we find 5 words with 25 uniq letters, then backtrack and continue - we find all the possible solution and write to output buffer.

## Performance
- **Average Search Time**: ~60ms
- **Unique Solutions**: 538

## Usage
Run the search and verify results against `result.txt`:
```bash
make rust
```

## Credits and Idea

This project was inspired by the following video:
[Five Words, 25 Letters](https://www.youtube.com/watch?v=_-AfhLQfb6w)

#### C++ solution credits:

This was the fastest solution presented by the author of the YT video
[CPP solution](https://github.com/ilyanikolaevsky/five_words)

My adjusted implementation has minor tweaks - we measure execution time and do a 5 time benchmark run and average these times :)

<b>Run the CPP solution</b>
```bash
make cpp
```

I also made a C version (vibe coded 100%) inspired by Rust verison the thing I'm predicting is that cpp with DP is more optimized for diff hardware (the DP algorithm to be exact) and my mac doesn't utilize that well...

Rust and C versions are on par in terms of performance.

#### Outputs on my MAC:

```bash
janezsedeljsak@Janezs-MacBook-Pro wordscnt % make cpp
Total time:   2612.62ms
Unique words: 5977
Unique sets:  538

janezsedeljsak@Janezs-MacBook-Pro wordscnt % make c   
Total time:   71.359200ms
Unique words: 5977
Unique sets:  538

janezsedeljsak@Janezs-MacBook-Pro wordscnt % make rust
Total time:   69.424483ms
Unique words: 5977
Unique sets:  538
```

## License

This project is under the GNU GPLv3 license.
