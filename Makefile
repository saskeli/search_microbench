.PHONY: run clean

run: bench
	python3 bs_data.py data.txt
	./bench < data.txt | tee results.tsv

bench: branchless.cpp
	g++ -std=c++2a -Wall -Wextra -pedantic -Wshadow -Ofast -march=native -o bench branchless.cpp

clean:
	rm -f data.txt
	rm -f bench
	rm -f results.tsv