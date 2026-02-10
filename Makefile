wordz:
	@if cargo test; then \
		cargo run --release; \
	else \
		echo "Tests failed, skipping run"; \
	fi; \
	rm /target

wordz-cpp:
	g++ -O3 -o wordz-cpp main.cpp; \
	./wordz-cpp; \
	rm wordz-cpp