# search_microbench

Benchmark for searching sorted $2^k$ (with $k \in [1..13]$) element arrays of both signed and unsigned 8-, 16-, 32-, and 64-bit integers.

Benchmark related to branch selection in B-trees in e.g. [https://github.com/saskeli/bit_vector](https://github.com/saskeli/bit_vector)

Contains 9 implementations:

| name               | description |
|--------------------|-------------|
| `binary`           | Naive binary search. |
| `templated_binary` | Recursive binary search with templated array size. |
| `templated_cmov`   | Recursive templated binary search with conditional moves instead of branchin. |
| `templated_sub`    | Recursive templated binary search that uses the sign bit and subtraction instead of conditional moves. (Limits application to positive signed integers.) |
| `branchless_cmov`  | Iterative binary search based on conditional moves as proposed by Sanders and Winkel<sup>1</sup>. |
| `branchless_sub`   | Iterative binary search using sign bit arithmetic. (Limits application to positive signed integers.) |
| `linear_scan`      | Naive linear scan. |
| `linear_scan_cmov` | Linear scan based on conditional moves. |
| `linear_scan_sub`  | Linear scan using sign bit arithmetic. (Limits application to positive signed integers.) |

In addition, the simple `search` function is included, that automatically dispaches the "best" implementation based on architecture, data type and array size.

## Citing

If you find this michrobenchmark usefull, please cite:


Diego Díaz-Domínguez, Saska Dönges, Simon J. Puglisi, and Leena Salmela. \
Simple Runs-Bounded FM-Index Designs Are Fast. \
In 21st International Symposium on Experimental Algorithms (SEA 2023). Leibniz International Proceedings in Informatics (LIPIcs), Volume 265, pp. 7:1-7:16, Schloss Dagstuhl – Leibniz-Zentrum für Informatik (2023) \
[https://doi.org/10.4230/LIPIcs.SEA.2023.7]

## Usage

Just do `make -j run` after cloning. This will download dependences, compile all the binaries, and run benchmark with outputs to `<name of cpu>(_avx)?.(prof|res)`.

Or more manually:

* Clone the repo (`git clone https://github.com/saskeli/search_microbench.git`)
* Run unit tests with `make test` \
  Should download Google test dependence
* Compile google benchmarks with `make bench` or `make bench_avx` \
  Dependences should be downloaded and compiled automatically
* Compile profiling binary with `make profile` \
  **note**: profiling will only work on systems with "modern" linux kernels with performance counters accessible from user space. \
  **note 2**: Profiling on arm will produce more noisy results due to necessary overhead in reading performance counters.
* Run benchmarks with e.g. `./bench | tee benchmark.res`
* Run profiling with e.g. `./profile | tee profiling.prof`

## "Requirements"

* Defaults to c++ 20 standard but may compile fine with older versions as well.
* Assumes a newish version of GCC. (may work with clang as well)
* [Google test](https://github.com/google/googletest) for running tests
* [Google benchmark](https://github.com/google/benchmark) for timing
* [counters](https://github.com/saskeli/counters) for accessing performance counters for profiling

Profiling will only work on machines running "modern" linux.

-----------------------------------------------------

<sup>1</sup> Sanders, S. (2004). Super Scalar Sample Sort. In Algorithms – ESA 2004 (pp. 784–796). Springer Berlin Heidelberg.
