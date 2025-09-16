.PHONY: run clean perf

BENCH=-isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread
CMAKE_OPT=-DBENCHMARK_DOWNLOAD_DEPENDENCIES=on -DCMAKE_BUILD_TYPE=Release

TEST = -lpthread -DGTEST_ON -isystem googletest/googletest/include -pthread -L googletest/build/lib

bench: bench.cpp searchers.hpp benchmark/build/lib/libgtest.a
	g++ -std=c++2a -Wall -Wextra -DNDEBUG -pedantic -Wshadow -Ofast -march=native bench.cpp $(BENCH) -o bench

bf_test: bf_test.cpp searchers.hpp
	g++ -std=c++2a -Wall -Wextra -DNDEBUG -pedantic -Wshadow -Ofast -march=native bf_test.cpp -o bf_test

benchmark/include:
	git submodule update --init

benchmark/build/lib/libgtest.a: | benchmark/include
	mkdir -p benchmark/build
	(cd benchmark; cmake $(CMAKE_OPT) -S . -B "build")
	(cd benchmark; cmake --build "build" --config Release)

couters/counters.hpp:
	git submodule update --init

googletest/googletest:
	git submodule update --init

googletest/build/lib/libgtest_main.a: | googletest/googletest
	(mkdir -p googletest/build && cd googletest/build && cmake .. && make)

test/test: googletest/build/lib/libgtest_main.a test/test.cpp searchers.hpp
	g++ $(CFLAGS) $(GFLAGS) -g test/test.cpp -o test/test -lgtest_main -lgtest

test: test/test
	test/test $(ARG)

clean:
	rm -f data.txt
	rm -f bench
	rm -f results.tsv
	rm -f p_bench