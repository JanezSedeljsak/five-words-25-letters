use std::collections::HashMap;
use std::fs;
use std::io::{BufWriter, Write};
use std::time::Instant;

macro_rules! benchmark {
    ($n:expr, $code:expr) => {{
        let mut total = std::time::Duration::ZERO;
        let mut res = None;
        for _ in 0..$n {
            let start = Instant::now();
            let r = $code;
            total += start.elapsed();
            if res.is_none() {
                res = Some(r);
            }
        }
        (res.unwrap(), total / $n)
    }};
}

fn solve() -> (usize, usize) {
    let c: String = fs::read_to_string("words_alpha.txt").expect("Could not read words_alpha.txt");
    let s: &'static str = Box::leak(c.into_boxed_str());

    let mut masks: Vec<u32> = Vec::with_capacity(15000);
    let mut mask_to_bytes: HashMap<u32, &'static [u8]> = HashMap::new();

    for l in s.lines() {
        if l.len() != 5 {
            continue;
        }
        let b: &[u8] = l.as_bytes();
        let m: u32 = (1u32 << (b[0].wrapping_sub(b'a')))
            | (1u32 << (b[1].wrapping_sub(b'a')))
            | (1u32 << (b[2].wrapping_sub(b'a')))
            | (1u32 << (b[3].wrapping_sub(b'a')))
            | (1u32 << (b[4].wrapping_sub(b'a')));

        if m.count_ones() != 5 {
            continue;
        }

        if mask_to_bytes.insert(m, b).is_none() {
            masks.push(m);
        }
    }

    let mut freq: [(u32, usize); 26] = core::array::from_fn(|i| (0, i));
    for &mask in &masks {
        let mut t = mask;
        while t != 0 {
            freq[t.trailing_zeros() as usize].0 += 1;
            t &= t - 1;
        }
    }
    freq.sort();

    let mut order = [0usize; 26];
    let mut reverse_order = [0u32; 26];
    for (i, &(_, l)) in freq.iter().enumerate() {
        order[i] = l;
        reverse_order[l] = i as u32;
    }

    let mut ltm: Vec<Vec<u32>> = (0..26).map(|_| Vec::new()).collect();
    for &mask in &masks {
        let mut t = mask;
        let mut min_rank = u32::MAX;
        while t != 0 {
            let l = t.trailing_zeros();
            min_rank = min_rank.min(reverse_order[l as usize]);
            t &= t - 1;
        }
        ltm[min_rank as usize].push(mask);
    }

    let out_file: fs::File = fs::File::create("rust_out.txt").unwrap();
    let mut writer: BufWriter<fs::File> = BufWriter::with_capacity(65536, out_file);
    let mut count: usize = 0;

    let mut stack: [(u32, usize, usize, bool); 6] = [(0, 0, 0, false); 6];
    stack[0] = (0, 0, 0, false);
    let mut d: usize = 0;

    'outer: loop {
        if d == 5 {
            break;
        }

        let (cur, mut word_start, lo_idx, skipped) = stack[d];
        let mut skipped = skipped;

        let mut r_idx = lo_idx;
        loop {
            if r_idx >= 26 {
                if d == 0 {
                    break 'outer;
                }
                d -= 1;
                stack[d].1 += 1;
                continue 'outer;
            }

            let letter = order[r_idx];
            if (cur >> letter) & 1 == 1 {
                r_idx += 1;
                continue;
            }

            let bucket: &[u32] = &ltm[r_idx];
            let bucket_len = bucket.len();
            let mut found = false;

            let mut i = word_start;
            while i < bucket_len {
                let m: u32 = unsafe { *bucket.get_unchecked(i) };
                if (cur & m) == 0 {
                    stack[d] = (cur, i, r_idx, skipped);
                    d += 1;
                    stack[d] = (cur | m, 0, r_idx + 1, skipped);

                    if d == 5 {
                        for x in 1..6 {
                            let m_sub: u32 = stack[x].0 ^ stack[x - 1].0;
                            let wb = unsafe { mask_to_bytes.get(&m_sub).unwrap_unchecked() };
                            writer.write_all(wb).unwrap();
                            writer.write_all(b" ").unwrap();
                        }
                        writer.write_all(b"\n").unwrap();
                        count += 1;
                        d -= 1;
                        stack[d].1 += 1;
                    }
                    found = true;
                    break;
                }
                i += 1;
            }

            if found {
                continue 'outer;
            }

            if skipped {
                if d == 0 {
                    break 'outer;
                }
                d -= 1;
                stack[d].1 += 1;
                continue 'outer;
            }
            skipped = true;
            r_idx += 1;
            word_start = 0;
            stack[d] = (cur, 0, r_idx, true);
        }
    }
    writer.flush().unwrap();
    (count, masks.len())
}

fn main() {
    let ((count, words), dur): ((usize, usize), std::time::Duration) = benchmark!(5, solve());
    let ms = dur.as_secs_f64() * 1000.0;
    println!("{} solutions written to rust_out.txt.", count);
    println!("Total time: {:.2}ms", ms);
    println!("Unique words: {}", words);
}

#[cfg(test)]
mod tests {
    use super::*;

    fn get_mask(s: &str) -> u32 {
        let b = s.as_bytes();
        (1u32 << (b[0].wrapping_sub(b'a')))
            | (1u32 << (b[1].wrapping_sub(b'a')))
            | (1u32 << (b[2].wrapping_sub(b'a')))
            | (1u32 << (b[3].wrapping_sub(b'a')))
            | (1u32 << (b[4].wrapping_sub(b'a')))
    }

    #[test]
    fn verify() {
        let (count, _): (usize, usize) = solve();
        assert_eq!(count, 538);
        let out_content: String =
            fs::read_to_string("rust_out.txt").expect("Could not read rust_out.txt");
        let result_content: String =
            fs::read_to_string("result.txt").expect("Could not read result.txt");

        let mut actual: Vec<Vec<u32>> = out_content
            .lines()
            .map(|l| {
                let mut v: Vec<u32> = l.split_whitespace().map(get_mask).collect();
                v.sort();
                v
            })
            .collect();

        actual.sort();

        let mut expected: Vec<Vec<u32>> = result_content
            .lines()
            .filter(|l| !l.trim().is_empty())
            .map(|l| {
                let mut v: Vec<u32> = l.split_whitespace().map(get_mask).collect();
                v.sort();
                v
            })
            .collect();

        expected.sort();

        assert_eq!(actual.len(), expected.len());
        for (a, e) in actual.iter().zip(&expected) {
            assert_eq!(a, e);
        }
    }
}
