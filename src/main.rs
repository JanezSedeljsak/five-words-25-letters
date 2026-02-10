use std::fs;
use std::time::Instant;

macro_rules! benchmark {
    ($n:expr, $code:expr) => {{
        let mut total = std::time::Duration::ZERO;
        let mut res = None;
        for _ in 0..$n {
            let start = Instant::now();
            let r = $code;
            total += start.elapsed();
            if res.is_none() { res = Some(r); }
        }
        (res.unwrap(), total / $n)
    }};
}

fn solve() -> (Vec<[u32; 5]>, usize) {
    let content = fs::read_to_string("words_alpha.txt").expect("Could not read words_alpha.txt");
    let s: &'static str = Box::leak(content.into_boxed_str());

    let mut masks: Vec<u32> = Vec::with_capacity(6000);
    let mut seen = vec![0u8; 1 << 23];
    for l in s.lines() {
        if l.len() == 5 {
            let b = l.as_bytes();
            let mut m = 1u32 << (b[0].wrapping_sub(b'a'));
            m |= 1 << (b[1].wrapping_sub(b'a'));
            m |= 1 << (b[2].wrapping_sub(b'a'));
            m |= 1 << (b[3].wrapping_sub(b'a'));
            m |= 1 << (b[4].wrapping_sub(b'a'));
            
            if m.count_ones() == 5 {
                let bi = (m >> 3) as usize;
                let bt = (m & 7) as u8;
                if (seen[bi] >> bt) & 1 == 0 {
                    seen[bi] |= 1 << bt;
                    masks.push(m);
                }
            }
        }
    }

    let order_chars = "qxjzvfwbkgpmhdcytlnuroisea";
    let order: Vec<usize> = order_chars.bytes().map(|b| (b - b'a') as usize).collect();
    
    let mut letter_to_masks: Vec<Vec<u32>> = (0..26).map(|_| Vec::with_capacity(1024)).collect();
    for &mask in &masks {
        let mut m = mask;
        while m != 0 {
            let i = m.trailing_zeros() as usize;
            letter_to_masks[i].push(mask);
            m &= m - 1;
        }
    }

    let mut mask_results = Vec::with_capacity(10000);
    for missing_letter in 0..26 {
        let target_mask = ((1 << 26) - 1) ^ (1 << missing_letter);
        let mut stack = [(0u32, 0usize); 6];
        stack[0] = (0, 0);
        let mut depth = 0;

        while depth < 5 {
            let (current_mask, b_idx) = stack[depth];
            let mut rarest_missing_idx = 0;
            while rarest_missing_idx < 26 {
                let l = order[rarest_missing_idx];
                if ((target_mask >> l) & 1 == 1) && ((current_mask >> l) & 1 == 0) {
                    break;
                }
                rarest_missing_idx += 1;
            }

            if rarest_missing_idx == 26 {
                if depth == 0 { break; }
                depth -= 1;
                stack[depth].1 += 1;
                continue;
            }

            let letter = order[rarest_missing_idx];
            let bucket = &letter_to_masks[letter];
            let mut found = false;
            for i in b_idx..bucket.len() {
                let m = bucket[i];
                if (m & !target_mask) == 0 && (current_mask & m) == 0 {
                    stack[depth].1 = i;
                    depth += 1;
                    stack[depth] = (current_mask | m, 0);
                    if depth == 5 {
                        let mut combo = [0u32; 5];
                        for d in 1..6 { combo[d-1] = stack[d].0 ^ stack[d-1].0; }
                        mask_results.push(combo);
                        depth -= 1;
                        stack[depth].1 += 1;
                    }
                    found = true;
                    break;
                }
            }
            if !found {
                if depth == 0 { break; }
                depth -= 1;
                stack[depth].1 += 1;
            }
        }
    }
    (mask_results, masks.len())
}

fn main() {
    let ((mask_results, unique_words), duration) = benchmark!(5, solve());
    println!("Total time:   {:?}", duration);
    println!("Unique words: {}", unique_words);
    println!("Unique sets:  {}", mask_results.len());
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn verify_against_result_txt() {
        let (mask_results, _) = solve();
        
        let result_content = std::fs::read_to_string("result.txt").expect("Could not read result.txt");
        let mut expected_masks = Vec::new();
        for line in result_content.lines() {
            if line.trim().is_empty() { continue; }
            let mut line_masks = [0u32; 5];
            for (i, word) in line.split_whitespace().enumerate().take(5) {
                let mut m = 0u32;
                for &b in word.as_bytes() {
                    m |= 1 << (b.wrapping_sub(b'a'));
                }
                line_masks[i] = m;
            }
            line_masks.sort();
            expected_masks.push(line_masks);
        }
        expected_masks.sort();

        let mut actual_masks = Vec::new();
        for mut combo in mask_results {
            combo.sort();
            actual_masks.push(combo);
        }
        actual_masks.sort();

        assert_eq!(actual_masks.len(), expected_masks.len());
        for (actual, expected) in actual_masks.iter().zip(&expected_masks) {
            assert_eq!(actual, expected);
        }
    }
}