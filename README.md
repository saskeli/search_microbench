# search_microbench

Benchmark for searching sorted 64 element arrays of signed non-negative 32-bit integers.

Very specific benchmark related to branch selection in [https://github.com/saskeli/bit_vector](https://github.com/saskeli/bit_vector)

Contains 7 implementations:

* `binary` is a naive binary search.
* `branchless_cmov` is based on conditional moves as proposed by Sanders and Winkel<sup>1</sup>.
* `branchless_sub` uses the sign bit and subtraction instead of conditional moves. Limits search space to [0..2^31 - 1]
* `branchless_sub_fix` is hacked to work for all uint32_t values
* `linear_scan` is a naive linear scan.
* `linear_scan_cmov` is a linear scan based on conditional moves.
* `linear_scan_sub` uses uses sign bit and subtraction instead of conditional moves.
* `templated_cmov` uses templating to do branching efficiently.

## Citing

If you find this michrobenchmark usefull, please cite:

D. Diaz, S. Dönges, S. Puglisi and L. Salmela. Simple Runs-bounded FM-index Designs are Fast. 2023

Better information hopefully following soon (Feb 2023).

## Running

Run default stuff with `make`. Will output timings to "results.tsv".

The "bs_data" python script can be used to generate skewed test data.

See `Makefile`, `branchless.cpp` and `bs_data.py`...

## "Requirements"

Requires python 3.6 or newer for test data generation.

Defaults to c++ 20 standard but should compile fine with older versions as well.

## Results

Running menchmarks on my Intel Core i5-6200U laptop results in 

![](results.png)


-----------------------------------------------------

<sup>1</sup> Sanders, S. (2004). Super Scalar Sample Sort. In Algorithms – ESA 2004 (pp. 784–796). Springer Berlin Heidelberg.
