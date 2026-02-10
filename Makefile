rust:
	@rustc -C opt-level=3 --test main.rs -o wordz-test && (./wordz-test > .test_log 2>&1 || (cat .test_log && exit 1)) && rm -f .test_log wordz-test
	@rustc -C opt-level=3 main.rs -o wordz && ./wordz && rm wordz

cpp:
	@g++ -O3 -std=c++2b -o wordz-cpp main.cpp && ./wordz-cpp && rm wordz-cpp

c:
	@gcc -O3 -march=native -o wordz-c main.c && ./wordz-c && rm wordz-c

clean:
	@rm -f rust_out.txt cpp_out.txt c_out.txt wordz wordz-test wordz-c .test_log wordz-cpp