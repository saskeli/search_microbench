"""
Test data generation

Run with "python bs_data.py <file name> [skew]"
to generate testing data for branchless.cpp.

Skew sets the maximum search target for the generated queries.
If skew = 0, all generated queries will match index 0 of the generated array.
If skew = 63, queries will be (pseudo uniformly) distribute in the range [0..63].

Requires python 3.6 or newer.
"""

import sys
import random

def main(f_path, skew):
    with open(f_path, 'w') as out_file:
        for _ in range(1000):
            sum = 0
            limit = 0
            for i in range(64):
                sum += random.randint(0, 2**14)
                out_file.write("{} ".format(sum))
                if i == skew:
                    limit = sum
            out_file.write(" ".join(
                [str(random.randint(0, limit - 1)) for _ in range(100)]
            ) + "\n")

if __name__ == "__main__":
    skew = 63
    if len(sys.argv) < 2:
        print("File name required")
        exit()
    if len(sys.argv) > 2:
        skew = int(sys.argv[2])
        if skew < 0 or skew >= 63:
            print("Skew must be in range [0..63].")
            exit()
    main(sys.argv[1], skew)