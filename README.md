# Five words 25 letters

Rust solver for the five words 25-letter challange.

## The Problem
Find all unique sets of 5 words (len == 5) from a txt file (370k+ words) that together contain exactly 25 unique letters of the English alphabet.

## Performance
- **Average Search Time**: ~60ms
- **Unique Solutions**: 538

## Usage
Run the search and verify results against `result.txt`:
```bash
make wordz
```

## Credits and Idea

This project was inspired by the following video:
[Five Words, 25 Letters](https://www.youtube.com/watch?v=_-AfhLQfb6w)

#### C++ solution credits:

This was the fastest solution presented by the author of the YT video
[CPP solution](https://github.com/ilyanikolaevsky/five_words)

My adjusted implementation has minor adjustments - it doesn't print to the stdout it just collects into a vector since printing is not something i want to measure :)

<b>Run the CPP solution</b>
```bash
make wordz-cpp
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
