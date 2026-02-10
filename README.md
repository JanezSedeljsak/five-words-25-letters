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
[CPP solution](https://github.com/oisyn/parkerwords)

My adjusted implementation has minor tweaks - we measure execution time and do a 5 time benchmark run and average these times and also add a verification test + I removed paralelism I wanted to see single core performance :)

<b>Run the CPP solution</b>
```bash
make cpp
```

#### Outputs on my MAC:

```bash
janezsedeljsak@Janezs-MacBook-Pro wordscnt % make rust
538 solutions written to rust_out.txt.
Total time: 60.35ms
Unique words: 5977

janezsedeljsak@Janezs-MacBook-Pro wordscnt % make cpp
538 solutions written to cpp_out.txt.
Total time: 31.26ms
Unique words: 5977
janezsedeljsak@Janezs-MacBook-Pro wordscnt % 
```

## License

This project is under the MIT license.
