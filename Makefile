wordz:
	@if cargo test; then \
		cargo run --release; \
	else \
		echo "Tests failed, skipping run"; \
	fi