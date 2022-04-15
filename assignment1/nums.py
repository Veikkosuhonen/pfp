import numpy as np
import sys

def output_help():
    print("""
Generates benchmark data for problem set 1 of programming for perfomance.

The a, b, c flags are mutually exclusive, and only one of them will ever be active at one time.

nums.py requires numpy to work.

Usage:
    python3 nums.py [options]

Options:
    -h           Print this message and terminate.
    -n <int>     Number of insertions and queries to generate. Defaults to 10^6.
    -m <int>     Highest permitted number. Defaults to 10^6.
    -a           Generate an additional n membership / sum queries. Default.
    -b           Generate an additional n location queries.
    -c           Generate an additional n index queries.
    -r <number>  Set the seed for the random number generator.

Example:
    python3 nums.py -n 10 -m 10 > data
        Generates a test case similar to A0.""")
    exit()

def main(n, limit, extra):
    a = np.random.randint(0, limit, size=n + 2, dtype=np.uint64)
    a[0] = n
    if extra == 3:
        a[1] = len(bin(limit - 1)) - 2
    else:
        a[1] = limit
    sys.stdout.buffer.write(a)
    if extra == 1:
        b = np.random.randint(0, limit, size=n, dtype=np.uint64)
        sys.stdout.buffer.write(b)
    elif extra == 2:
        s = {e for e in a}
        b = np.random.randint(1, len(s) + 1, size=n, dtype=np.uint64)
        sys.stdout.buffer.write(b)
    elif extra == 3:
        b = np.random.randint(0, n, size=n, dtype=np.uint64)
        sys.stdout.buffer.write(b)

if __name__ == "__main__":
    n = 1000000
    if "-h" in sys.argv or "help" in sys.argv:
        output_help()
    if "-n" in sys.argv:
        n = int(sys.argv[sys.argv.index("-n") + 1])
    limit = 1000000
    if "-m" in sys.argv:
        limit = int(sys.argv[sys.argv.index("-m") + 1])
    if "-r" in sys.argv:
        random.seed(int(sys.argv[sys.argv.index("-r") + 1]))
    extra = 1
    if "-a" in sys.argv:
        extra = 1
    if "-b" in sys.argv:
        extra = 2
    if "-c" in sys.argv:
        extra = 3
    main(n, limit, extra)
