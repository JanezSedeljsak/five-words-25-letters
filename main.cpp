#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <string>

using namespace std;

constexpr auto benchmark(int n, auto code) {
    auto total = chrono::nanoseconds::zero();
    for (int i = 0; i < n; ++i) {
        auto start = chrono::high_resolution_clock::now();
        code();
        auto end = chrono::high_resolution_clock::now();
        total += chrono::duration_cast<chrono::nanoseconds>(end - start);
    }
    return total / n;
}

vector<string> LoadWords(string filename) {
    vector<string> words;
    ifstream file(filename);
    vector<bool> seen(26 * 26 * 26 * 26 * 26);
    string word;
    while (file >> word) {
        if (word.length() != 5) continue;
        string tmp = word;
        sort(tmp.begin(), tmp.end());
        bool bad_word = false;
        for (int i = 0; i < 4; ++i) {
            if (tmp[i] == tmp[i + 1]) {
                bad_word = true;
                break;
            }
        }
        if (bad_word) continue;
        int hash = 0;
        for (int i = 0; i < 5; ++i) {
            hash = hash * 26 + tmp[i] - 'a';
        }
        if (seen[hash]) continue;
        seen[hash] = true;
        words.push_back(word);
    }
    return words;
}

int Solve(const vector<string>& words) {
    vector<vector<bool>> can_construct(5, vector<bool>(1 << 26));
    vector<int> masks(words.size());
    for (int i = 0; i < (int)words.size(); ++i) {
        int mask = 0;
        for (auto c : words[i]) {
            mask |= 1 << (c - 'a');
        }
        masks[i] = mask;
        can_construct[0][mask] = true;
    }
    for (int cnt = 0; cnt < 4; ++cnt) {
        for (int mask = 0; mask < (1 << 26); ++mask) {
            if (!can_construct[cnt][mask]) continue;
            for (int i = 0; i < (int)words.size(); ++i) {
                if ((masks[i] & mask) == 0) {
                    can_construct[cnt + 1][masks[i] | mask] = true;
                }
            }
        }
    }

    ofstream out("cpp_out.txt");
    int count = 0;
    vector<int> current;
    
    auto collect = [&](auto self, int mask, int start_from, int depth) -> void {
        if (depth == 5) {
            for (int i = 0; i < 5; ++i) {
                out << words[current[i]] << (i == 4 ? "" : " ");
            }
            out << "\n";
            count++;
            return;
        }
        for (int cur_word = start_from; cur_word < (int)masks.size(); ++cur_word) {
            if (((mask & masks[cur_word]) == masks[cur_word]) && (depth == 4 || can_construct[3 - depth][mask ^ masks[cur_word]])) {
                current.push_back(cur_word);
                self(self, mask ^ masks[cur_word], cur_word + 1, depth + 1);
                current.pop_back();
            }
        }
    };

    for (int mask = 0; mask < (1 << 26); ++mask) {
        if (can_construct[4][mask]) {
            collect(collect, mask, 0, 0);
        }
    }
    out.close();
    return count;
}

int main() {
    vector<string> words = LoadWords("words_alpha.txt");
    int count = 0;
    auto avg_ns = benchmark(5, [&]() {
        count = Solve(words);
    });
    
    double ms = static_cast<double>(avg_ns.count()) / 1'000'000.0;
    cout << "Total time:   " << ms << "ms" << endl;
    cout << "Unique words: " << words.size() << endl;
    cout << "Unique sets:  " << count << endl;
    return 0;
}