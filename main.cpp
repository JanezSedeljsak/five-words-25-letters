#include <bit>
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <array>
#include <chrono>
#include <sstream>

using uint = unsigned int;
using WordArray = std::array<uint, 5>;

std::vector<uint> wordbits;
std::vector<std::string> allwords;
std::unordered_map<uint, size_t> bitstoindex;
std::vector<uint> letterindex[26];
uint letterorder[26];

std::string_view getword(const char*& _str, const char* end)
{
    const char* str = _str;
    while(str != end && (*str == '\n' || *str == '\r'))
	{
		if (++str == end)
            return (_str = str), std::string_view{};
	}

    const char* start = str;
    while(str != end && *str != '\n' && *str != '\r')
        ++str;

    _str = str;
    return std::string_view{ start, str };
}

uint getbits(std::string_view word)
{
    uint r = 0;
    for (char c : word)
        r |= 1 << (c - 'a');
    return r;
}

void readwords(const char* file)
{
	struct { int f, l; } freq[26] = { };
	for (int i = 0; i < 26; i++)
		freq[i].l = i;

    std::vector<char> buf;
    std::ifstream in(file, std::ios::binary);
    in.seekg(0, std::ios::end);
    buf.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&buf[0], buf.size());

    const char* str = &buf[0];
	const char* strEnd = str + buf.size();

    std::string_view word;
    while(!(word = getword(str, strEnd)).empty())
    {
        if (word.size() != 5)
            continue;
        uint bits = getbits(word);
        if (std::popcount(bits) != 5)
            continue;

        if (!bitstoindex.contains(bits))
        {
            bitstoindex[bits] = wordbits.size();
            wordbits.push_back(bits);
            allwords.emplace_back(word);

            for(char c: word)
                freq[c - 'a'].f++;
        }
    }

    std::sort(std::begin(freq), std::end(freq), [](auto a, auto b) { return a.f < b.f; });
	uint reverseletterorder[26];
    for (int i = 0; i < 26; i++)
	{
		letterorder[i] = freq[i].l;
        reverseletterorder[freq[i].l] = i;
    }

    for (uint w : wordbits)
    {
        uint m = w;
		uint letter = std::countr_zero(m);
        uint min = reverseletterorder[letter];
		m &= m - 1; 
        while(m)
        {
            letter = std::countr_zero(m);
            min = std::min(min, reverseletterorder[letter]);
			m &= m - 1;
		}

        letterindex[min].push_back(w);
    }
}

void findwords(std::vector<WordArray>& solutions, uint totalbits, int numwords, WordArray words, uint maxLetter, bool skipped)
{
	if (numwords == 5)
	{
		solutions.push_back(words);
		return;
	}

	for (uint i = maxLetter; i < 26; i++)
	{
        uint letter = letterorder[i];
        uint m = 1 << letter;
        if (totalbits & m)
            continue;

        for (uint w : letterindex[i])
		{
			if (totalbits & w)
				continue;

			words[numwords] = w;
			findwords(solutions, totalbits | w, numwords + 1, words, i + 1, skipped);
		}

        if (skipped)
            break;
        skipped = true;
	}
}

int main()
{
    using clock = std::chrono::high_resolution_clock;

    readwords("words_alpha.txt");

    long long totalProcessNs = 0;
    std::vector<WordArray> finalSolutions;

    for (int iter = 0; iter < 5; iter++)
    {
        std::vector<WordArray> solutions;
        solutions.reserve(10000);
        auto t0 = clock::now();
        WordArray words = { };
        findwords(solutions, 0, 0, words, 0, false);
        auto t1 = clock::now();
        totalProcessNs += std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
        if (iter == 0) finalSolutions = std::move(solutions);
    }

	std::ofstream out("cpp_out.txt");
    for (auto& words : finalSolutions)
    {
        for (auto w : words)
            out << allwords[bitstoindex[w]] << "\t";
        out << "\n";
    };

    double avgMs = (totalProcessNs / 5) / 1e6;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << finalSolutions.size() << " solutions written to cpp_out.txt.\n";
    std::cout << "Total time: " << avgMs << "ms\n";
    std::cout << "Unique words: " << wordbits.size() << "\n";

#ifdef RUN_TESTS
    std::ifstream res_file("result.txt");
    if (!res_file) {
        std::cout << "Verification skipped: result.txt not found.\n";
        return 0;
    }

    auto parse_to_masks = [&](std::string_view l) {
        std::vector<uint> masks;
        std::string word;
        std::stringstream ss{std::string(l)};
        while (ss >> word) {
            masks.push_back(getbits(word));
        }
        std::sort(masks.begin(), masks.end());
        return masks;
    };

    std::vector<std::vector<uint>> expected;
    std::string line;
    while (std::getline(res_file, line)) {
        if (line.empty()) continue;
        expected.push_back(parse_to_masks(line));
    }
    std::sort(expected.begin(), expected.end());

    std::vector<std::vector<uint>> actual;
    for (auto& sol : finalSolutions) {
        std::vector<uint> masks(sol.begin(), sol.end());
        std::sort(masks.begin(), masks.end());
        actual.push_back(masks);
    }
    std::sort(actual.begin(), actual.end());

    if (actual == expected) {
        std::cout << "Verification: PASS\n";
    } else {
        std::cout << "Verification: FAIL (Actual size: " << actual.size() << ", Expected size: " << expected.size() << ")\n";
    }
#endif
}