#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <chrono>

using namespace std;

vector<string> LoadWords(std::string filename) {
	vector<string> words;
	ifstream file(filename);
	vector<bool> seen(26*26*26*26*26);
	string word;
	while (file >> word) {
		if (word.length() != 5) continue;
		string tmp = word;
		sort(tmp.begin(), tmp.end());
		bool bad_word = false;
		for (int i = 0; i < 4; ++i) {
			if (tmp[i] == tmp[i+1]) {
				bad_word = true;
				break;
			}
		}
		if (bad_word) continue;
		int hash = 0;
		for (int i = 0; i < 5; ++i) {
			hash = hash*26 + tmp[i] - 'a';
		}
		if (seen[hash]) continue;
		seen[hash] = true;
		words.push_back(word);
	}
	return words;
}

void CollectSets(const vector<vector<bool>> &can_construct,
					const vector<int> &masks,
					vector<int> &result,
					int mask,
					int start_from,
					vector<vector<int>> &all_results) {
	if (result.size() == 5) {
		all_results.push_back(result);
		return;
	}
	for (int cur_word = start_from; cur_word < (int)masks.size(); ++cur_word) {
		if (((mask & masks[cur_word]) == masks[cur_word]) && (result.size() == 4 || can_construct[3-result.size()][mask ^ masks[cur_word]])) {
			result.push_back(cur_word);
			CollectSets(can_construct, masks, result, mask ^ masks[cur_word], cur_word + 1, all_results);
			result.pop_back();
		}
	}
}

void Solve(const vector<string> &words) {
	auto start = chrono::high_resolution_clock::now();
	vector<vector<bool>> can_construct(5, vector<bool>(1 << 26));
	vector<int> masks(words.size());
	for (int i = 0 ; i < (int)words.size(); ++i) {
		int mask = 0;
		for (auto c: words[i]) {
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
					can_construct[cnt+1][masks[i] | mask] = true;
				}
			}
		}
	}

	vector<vector<int>> all_results;
	vector<int> result;
	for (int mask = 0; mask < (1 << 26); ++mask) {
		if (can_construct[4][mask]) {
			CollectSets(can_construct, masks, result, mask, 0, all_results);
		}
	}
	auto end = chrono::high_resolution_clock::now();
	chrono::duration<double, milli> dur = end - start;
	cout << "Total time:   " << dur.count() << "ms" << endl;
	cout << "Unique words: " << words.size() << endl;
	cout << "Unique sets:  " << all_results.size() << endl;
}

int main() {
	vector<string> words = LoadWords("words_alpha.txt");
	Solve(words);
	return 0;
}