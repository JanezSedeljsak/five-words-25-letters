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

const ORDER: [usize; 26] = {
    let order_chars = b"qxjzvfwbkgpmhdcytlnuroisea";
    let mut result = [0; 26];
    let mut i = 0;
    while i < 26 {
        result[i] = (order_chars[i] - b'a') as usize;
        i += 1;
    }
    result
};

fn solve() -> (usize, usize) {
    let c: String = fs::read_to_string("words_alpha.txt").expect("Could not read words_alpha.txt");
    let s: &'static str = Box::leak(c.into_boxed_str());

    let mut masks: Vec<u32> = Vec::with_capacity(15000);
    let mut mask_to_bytes: HashMap<u32, &'static [u8]> = HashMap::with_capacity(15000);
    let mut seen: Vec<u8> = vec![0u8; 1 << 23];

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

        let bi: usize = (m >> 3) as usize;
        let bt: u8 = (m & 7) as u8;

        if (seen[bi] >> bt) & 1 == 0 {
            seen[bi] |= 1 << bt;
            masks.push(m);
            mask_to_bytes.insert(m, b);
        }
    }

    let mut ltm: Vec<Vec<u32>> = (0..26).map(|_| Vec::with_capacity(5000)).collect();
    for &mask in &masks {
        let mut t: u32 = mask;
        while t != 0 {
            let i: usize = t.trailing_zeros() as usize;
            ltm[i].push(mask);
            t &= t - 1;
        }
    }

    let out_file: fs::File = fs::File::create("rust_out.txt").unwrap();
    let mut writer: BufWriter<fs::File> = BufWriter::with_capacity(65536, out_file);
    let mut count: usize = 0;

    for m_l in 0..26 {
        let target: u32 = ((1u32 << 26) - 1) ^ (1 << m_l);
        let mut stack: [(u32, usize); 6] = [(0u32, 0usize); 6];
        stack[0] = (0, 0);
        let mut d: usize = 0;

        'outer: loop {
            if d == 5 {
                break;
            }

            let (cur, b_idx): (u32, usize) = stack[d];
            let missing = target & !cur;

            let mut r_idx: usize = 0;
            while r_idx < 26 {
                let l = ORDER[r_idx];
                if (missing >> l) & 1 == 1 {
                    break;
                }
                r_idx += 1;
            }

            if r_idx == 26 {
                if d == 0 {
                    break 'outer;
                }
                d -= 1;
                stack[d].1 += 1;
                continue;
            }

            let bucket: &[u32] = &ltm[ORDER[r_idx]];
            let bucket_len = bucket.len();
            let mut found: bool = false;

            let mut i = b_idx;
            while i < bucket_len {
                let m: u32 = unsafe { *bucket.get_unchecked(i) };
                if (m & !target) == 0 && (cur & m) == 0 {
                    stack[d].1 = i;
                    d += 1;
                    stack[d] = (cur | m, 0);

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
            if !found {
                if d == 0 {
                    break 'outer;
                }
                d -= 1;
                stack[d].1 += 1;
            }
        }
    }
    writer.flush().unwrap();
    drop(writer);
    (count, masks.len())
}

fn main() {
    let ((count, words), dur): ((usize, usize), std::time::Duration) = benchmark!(5, solve());
    println!("Total time:   {}", format!("{:?}", dur));
    println!("Unique words: {}", words);
    println!("Unique sets:  {}", count);
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn verify() {
        let (count, _): (usize, usize) = solve();
        assert_eq!(count, 538);
        let out_content: String =
            fs::read_to_string("rust_out.txt").expect("Could not read rust_out.txt");
        let result_content: String =
            fs::read_to_string("result.txt").expect("Could not read result.txt");

        let mut actual: Vec<Vec<String>> = out_content
            .lines()
            .map(|l| {
                let mut v: Vec<String> = l.split_whitespace().map(|s| s.to_string()).collect();
                v.sort();
                v
            })
            .collect();

        actual.sort();

        let mut expected: Vec<Vec<String>> = result_content
            .lines()
            .filter(|l| !l.trim().is_empty())
            .map(|l| {
                let mut v: Vec<String> = l.split_whitespace().map(|s| s.to_string()).collect();
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
