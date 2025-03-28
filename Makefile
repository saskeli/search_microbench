.PHONY: run clean perf

run: bench
	python3 bs_data.py data.txt
	./bench < data.txt | tee results.tsv

perf: p_bench
	python3 bs_data.py data.txt
	./p_bench < data.txt | tee perf.tsv

bench: branchless.cpp
	g++ -std=c++2a -Wall -Wextra -DNDEBUG -pedantic -Wshadow -Ofast -march=native -o bench branchless.cpp

p_bench: branchless.cpp counters.hpp
	g++ -std=c++2a -DCOUNT -DNDEBUG -Wall -Wextra -pedantic -Wshadow -Ofast -march=native -o p_bench branchless.cpp

clean:
	rm -f data.txt
	rm -f bench
	rm -f results.tsv
	rm -f p_bench