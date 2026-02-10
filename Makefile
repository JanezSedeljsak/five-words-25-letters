rust:
	@rustc -C opt-level=3 --test main.rs -o wordz-test && (./wordz-test > .test_log 2>&1 || (cat .test_log && exit 1)) && rm -f .test_log wordz-test
	@rustc -C opt-level=3 -C target-cpu=native main.rs -o wordz && ./wordz && rm wordz

cpp:
	@g++ -O3 -std=c++2b -march=native -pthread -DRUN_TESTS -o wordz-cpp-test main.cpp && (./wordz-cpp-test > .test_log 2>&1 || (cat .test_log && exit 1)) && rm -f .test_log wordz-cpp-test
	@g++ -O3 -std=c++2b -march=native -pthread -o wordz-cpp main.cpp && ./wordz-cpp && rm wordz-cpp

clean:
	@rm -f rust_out.txt solutions.txt wordz wordz-test wordz-c .test_log wordz-cpp wordz-cpp-test