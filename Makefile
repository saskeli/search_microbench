.PHONY: run clean perf

CFLAGS=-march=native -std=c++2a -Wall -Wextra -Wshadow -pedantic
BENCH=-isystem benchmark/include -Lbenchmark/build/src -lbenchmark -lpthread
CMAKE_OPT=-DBENCHMARK_DOWNLOAD_DEPENDENCIES=on -DCMAKE_BUILD_TYPE=Release

TEST = -lpthread -DGTEST_ON -isystem googletest/googletest/include -pthread -L googletest/build/lib

bench: bench.cpp searchers.hpp benchmark/build/lib/libgtest.a
	g++ $(CFLAGS) -DNDEBUG -Ofast bench.cpp $(BENCH) -o bench

profile: profile.cpp searchers.hpp counters/counters.hpp
	g++ $(CFLAGS) -DNDEBUG -Ofast profile.cpp -o profile

debug_bench: bench.cpp searchers.hpp benchmark/build/lib/libgtest.a
	g++ $(CFLAGS) -DDEBUG -O1 -g bench.cpp $(BENCH) -o debug_bench

bf_test: bf_test.cpp searchers.hpp
	g++ $(CFLAGS) -DNDEBUG -Ofast bf_test.cpp -o bf_test

debug_bf_test: bf_test.cpp searchers.hpp
	g++ $(CFLAGS) -DDEBUG -O1 -g bf_test.cpp -o debug_bf_test

benchmark/include:
	git submodule update --init

benchmark/build/lib/libgtest.a: | benchmark/include
	mkdir -p benchmark/build
	(cd benchmark; cmake $(CMAKE_OPT) -S . -B "build")
	(cd benchmark; cmake --build "build" --config Release)

counters/counters.hpp:
	git submodule update --init

googletest/googletest:
	git submodule update --init

googletest/build/lib/libgtest_main.a: | googletest/googletest
	(mkdir -p googletest/build && cd googletest/build && cmake .. && make)

test/test: googletest/build/lib/libgtest_main.a test/test.cpp searchers.hpp
	g++ -g $(CFLAGS) test/test.cpp -o test/test -lgtest_main -lgtest

test: test/test
	test/test $(ARG)

clean:
	rm -f data.txt
	rm -f bench
	rm -f results.tsv
	rm -f p_bench