import sys
import random

def output_help():
    print("""
Generates benchmark data for problem set 0 of programming for perfomance.

Usage:
    python3 nums.py [options]

Options:
    -h           Print this message and terminate.
    -n <int>     Number of insertions and queries to generate. Defaults to 10^6.
    -m <int>     Highest permitted number. Defaults to 2^32 - 1 (max value for signed 32-bit integer).
    -s           Insertions appear in increasing order. Defaults to False.
    -i           Interlieve insertsions and queries. Each operations is approximately equally likely 
                 to be an insertion and a query. Defaults to False.
    -r <number>  Set the seed for the random number generator.

Example:
    python3 nums.py -r 1337 > data.txt
        Generates the default data.txt file.
    
    python3 nums.py -n 1000000 -s -i > data.txt
        Generates a test case that should perform very poorly with the simple binary tree implementation.
    """)
    exit()

def output_interlieved(insertions, queries):
    i_idx = 0
    q_idx = 0
    output_insertion = True
    while i_idx < len(insertions) and q_idx < len(queries):
        if output_insertion:
            print(insertions[i_idx])
            i_idx += 1
        else:
            print(queries[q_idx])
            q_idx += 1
        if random.getrandbits(1):
            print("-1")
            output_insertion = not output_insertion
    if q_idx < len(queries):
        if output_insertion:
            print("-1")
        print("\n".join(str(v) for v in queries[q_idx:]))

def main(n, limit, sorted_insertions, interlieved_queries):
    insertions = [
        random.randint(0, limit)
        for _ in range(n)
    ]
    queries = [
        random.randint(0, limit)
        for _ in range(n)
    ]
    if (sorted_insertions):
        insertions.sort()
    if (interlieved_queries):
        output_interlieved(insertions, queries)
    else:
        print("\n".join(str(v) for v in insertions))
        print("-1")
        print("\n".join(str(v) for v in queries))

if __name__ == "__main__":
    n = 1000000
    if "-h" in sys.argv or "help" in sys.argv:
        output_help()
    if "-n" in sys.argv:
        n = int(sys.argv[sys.argv.index("-n") + 1])
    limit = (2 ** 31) - 1
    if "-m" in sys.argv:
        limit = int(sys.argv[sys.argv.index("-m") + 1])
    sorted_insertions = False
    if "-s" in sys.argv:
        sorted_insertions = True
    interlieved_queries = False
    if "-i" in sys.argv:
        interlieved_queries = True
    if "-r" in sys.argv:
        random.seed(int(sys.argv[sys.argv.index("-r") + 1]))
    main(n, limit, sorted_insertions, interlieved_queries)