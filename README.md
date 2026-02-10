# Five words 25 letters

Rust solver for the five words 25-letter challange.

## The Problem
Find all unique sets of 5 words (len == 5) from a txt file (370k+ words) that together contain exactly 25 unique letters of the English alphabet.

## The Algorithm (core idea)

The rust based bitwise magic implementation can be devided into a few steps:

- first we read in the whole file into a str and then filter out lines with 5 characters with no repeated characters using bitmasks.

- we create the masks and calculate the frequency of each letter for a optimal search order + we also remove angorams (keep only the first one).

- move the words into buckets based on their rarest letter (we search for the rarest letters first).

- we use a stack to keep track of the current path and search for solutions using depth-first search (recursion is expensive). We also do a skip-one pruning (if we can't find a solution with the current letter we skip it and try the next one).

- target is reached once we find 5 words with 25 uniq letters, then backtrack and continue - we find all the possible solutions and collect them. at the end we print to output buffer.

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
Total time: 35.38ms
Unique words: 5977

janezsedeljsak@Janezs-MacBook-Pro wordscnt % make cpp
538 solutions written to cpp_out.txt.
Total time: 31.09ms
Unique words: 5977
```

## License

This project is under the MIT license.
