#include <cstdint>
#include <iostream>
#include <bitset>
#include <chrono>

uint8_t binary(int* arr, int q) {
    uint8_t a = 0;
    uint8_t b = 64;
    uint8_t m;
    while (b > a) {
        m = (a + b) / 2;
        if (arr[m] < q) a = m + 1;
        else b = m;
    }
    return a;
}

uint8_t branchless_cmov(int* arr, int q) {
    uint8_t idx = (uint8_t(1) << 5) - 1;
    idx ^= ((arr[idx] < q) << 5) | (uint8_t(1) << 4);
    idx ^= ((arr[idx] < q) << 4) | (uint8_t(1) << 3);
    idx ^= ((arr[idx] < q) << 3) | (uint8_t(1) << 2);
    idx ^= ((arr[idx] < q) << 2) | (uint8_t(1) << 1);
    idx ^= ((arr[idx] < q) << 1) | uint8_t(1);
    return idx ^ (arr[idx] < q); 
}

uint8_t branchless_sub(int* arr, int q) {
    constexpr uint32_t MASK = uint32_t(1) << 31;
    uint8_t idx = (uint8_t(1) << 5) - 1;
    idx ^= (((arr[idx] - q) & MASK) >> 26) | (uint8_t(1) << 4);
    idx ^= (((arr[idx] - q) & MASK) >> 27) | (uint8_t(1) << 3);
    idx ^= (((arr[idx] - q) & MASK) >> 28) | (uint8_t(1) << 2);
    idx ^= (((arr[idx] - q) & MASK) >> 29) | (uint8_t(1) << 1);
    idx ^= (((arr[idx] - q) & MASK) >> 30) | uint8_t(1);
    return idx ^ (((arr[idx] - q) & MASK) >> 31); 
}

uint8_t branchless_sub_fix(uint32_t* arr, uint32_t q) {
    constexpr uint32_t MASK = uint32_t(1) << 31;
    uint8_t idx = (uint8_t(1) << 5) - 1;
    uint32_t v1 = arr[idx];
    uint32_t v2 = q;
    uint32_t res = MASK & v1 & v2;
    v1 ^= res;
    v2 ^= res;
    v1 |= (MASK - 1) * (v1 >> 31);
    v1 &= (MASK - 1);
    idx ^= (((v1 - v2) & MASK) >> 26) | (uint8_t(1) << 4);

    v1 = arr[idx];
    v2 = q;
    res = MASK & v1 & v2;
    v1 ^= res;
    v2 ^= res;
    v1 |= (MASK - 1) * (v1 >> 31);
    v1 &= (MASK - 1);
    idx ^= (((v1 - v2) & MASK) >> 27) | (uint8_t(1) << 3);

    v1 = arr[idx];
    v2 = q;
    res = MASK & v1 & v2;
    v1 ^= res;
    v2 ^= res;
    v1 |= (MASK - 1) * (v1 >> 31);
    v1 &= (MASK - 1);
    idx ^= (((v1 - v2) & MASK) >> 28) | (uint8_t(1) << 2);

    v1 = arr[idx];
    v2 = q;
    res = MASK & v1 & v2;
    v1 ^= res;
    v2 ^= res;
    v1 |= (MASK - 1) * (v1 >> 31);
    v1 &= (MASK - 1);
    idx ^= (((v1 - v2) & MASK) >> 29) | (uint8_t(1) << 1);

    v1 = arr[idx];
    v2 = q;
    res = MASK & v1 & v2;
    v1 ^= res;
    v2 ^= res;
    v1 |= (MASK - 1) * (v1 >> 31);
    v1 &= (MASK - 1);
    idx ^= (((v1 - v2) & MASK) >> 30) | uint8_t(1);
    
    v1 = arr[idx];
    v2 = q;
    res = MASK & v1 & v2;
    v1 ^= res;
    v2 ^= res;
    v1 |= (MASK - 1) * (v1 >> 31);
    v1 &= (MASK - 1);
    return idx ^ (((v1 - v2) & MASK) >> 31); 
}

uint8_t linear_scan(int* arr, int q) {
    for (uint8_t i = 0; i < 64; i++) {
        if (arr[i] >= q) return i;
    }
    return 63;
}

uint8_t linear_scan_cmov(int* arr, int q) {
    uint8_t res = 0;
    for (uint8_t idx = 0; idx < 64; idx++) {
        res += arr[idx] < q;
    }
    return res;
}

uint8_t linear_scan_sub(int* arr, int q) {
    uint8_t res = 0;
    for (uint8_t idx = 0; idx < 64; idx++) {
        res += uint32_t(arr[idx] - q) >> 31;
    }
    return res;
}

int main() {
    int arr[64 + 100];
    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;
    using std::chrono::nanoseconds;

    std::cout << "binary\tbranchless_cmov\tbranchless_sub\tbranchless_sub_fix\tlinear_scan\tlinear_scan_cmov\tlinear_scan_sub" << std::endl;

    for (size_t i = 0; i < 1000; i++) {
        for (int i = 0; i < 64 + 100; i++) {
            std::cin >> arr[i];
        }

        uint32_t a = 0;
        auto t1 = high_resolution_clock::now();
        for (int qi = 64; qi < 64 + 100; qi++) {
            a += binary(arr, arr[qi]);
        }
        auto t2 = high_resolution_clock::now();
        std::cout << (double)duration_cast<nanoseconds>(t2 - t1).count() / 100 << "\t";

        uint32_t b = 0;
        t1 = high_resolution_clock::now();
        for (int qi = 64; qi < 64 + 100; qi++) {
            b += branchless_cmov(arr, arr[qi]);
        }
        t2 = high_resolution_clock::now();
        std::cout << (double)duration_cast<nanoseconds>(t2 - t1).count() / 100 << "\t";

        uint32_t c = 0;
        t1 = high_resolution_clock::now();
        for (int qi = 64; qi < 64 + 100; qi++) {
            c += branchless_sub(arr, arr[qi]);
        }
        t2 = high_resolution_clock::now();
        std::cout << (double)duration_cast<nanoseconds>(t2 - t1).count() / 100 << "\t";

        uint32_t d = 0;
        t1 = high_resolution_clock::now();
        for (int qi = 64; qi < 64 + 100; qi++) {
            d += branchless_sub_fix(reinterpret_cast<uint32_t*>(arr), arr[qi]);
        }
        t2 = high_resolution_clock::now();
        std::cout << (double)duration_cast<nanoseconds>(t2 - t1).count() / 100 << "\t";

        uint32_t e = 0;
        t1 = high_resolution_clock::now();
        for (int qi = 64; qi < 64 + 100; qi++) {
            e += linear_scan(arr, arr[qi]);
        }
        t2 = high_resolution_clock::now();
        std::cout << (double)duration_cast<nanoseconds>(t2 - t1).count() / 100 << "\t";

        uint32_t f = 0;
        t1 = high_resolution_clock::now();
        for (int qi = 64; qi < 64 + 100; qi++) {
            f += linear_scan_cmov(arr, arr[qi]);
        }
        t2 = high_resolution_clock::now();
        std::cout << (double)duration_cast<nanoseconds>(t2 - t1).count() / 100 << "\t";

        uint32_t g = 0;
        t1 = high_resolution_clock::now();
        for (int qi = 64; qi < 64 + 100; qi++) {
            g += linear_scan_sub(arr, arr[qi]);
        }
        t2 = high_resolution_clock::now();
        std::cout << (double)duration_cast<nanoseconds>(t2 - t1).count() / 100 << std::endl;

        if (a != b || b != c || c != d || d != e || e != f || f != g) {
            std::cerr << "Error on line " << i << "\n";
            std::cerr << a << ", " << b << ", " << c << ", " << d << ", " << e << std::endl;
            exit(1);
        }
    }//*/

    /*for (int i = 0; i < 64 + 100; i++) {
        std::cin >> arr[i];
    }

    

    for (int i = 64; i < 64 + 100; i++) {
        uint8_t a = binary(arr, arr[i]);
        uint8_t b = branchless(arr, arr[i]);
        uint8_t c = u_branchless(reinterpret_cast<uint32_t *>(arr), arr[i]);
        uint8_t d = scan(arr, arr[i]);
        uint8_t e = branchless_scan(arr, arr[i]);
        if (a != b || b != c || c != d || d != e) {
            for (int k = 0; k < 8; k++) {
                for (int j = 0; j < 8; j++) {
                    std::cout << arr[k * 8 + j] << "\t";
                }
                std::cout << std::endl;
            }
            std::cout << "Query " << (i - 63) << " with value " << arr[i] << " failed:\n";
            std::cout << "binary\tbranchless\tu_branchless\tscan\tbranchless_scan\n";
            std::cout << int(a) << "\t" << int(b) << "\t" << int(c) << "\t" << int(d) << "\t" << int(e) << std::endl,
            exit(1);
        }
    }//*/
    
    return 0;
}
