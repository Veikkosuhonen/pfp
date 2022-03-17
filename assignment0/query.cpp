#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <unordered_set>

#include "binary_tree.hpp"
#include "vector_set.hpp"

/**
 * Helper function to ouput usage information when the -h flag is detected
 */
void help() {
    std::cout
        << "Program to test set data structures for positive ints.\n\n"
        << "Usage:\n"
        << "   ./query [options] [input file]\n\n"
        << "Options:\n"
        << "-h             Outputs this message and terminates.\n"
        << "-t <number>    Type. 1 will use std::set, 2 will use "
        << "std::unordered_set.\n"
        << "               Other options will be implementation dependent.\n"
        << "-l <number>    Limit. Highest number that will be inserted. "
        << "Defaults to 2^31 - 1.\n"
        << "-s             If given, it will be assumed that all insertions "
        << "will be done before any queries.\n"
        << "-v             Verify that the datastructure behaves the same way "
        << "as std::unordered_set (slow).\n"
        << "-d             Debug mode. Run the program in interactive / "
        << "verbose mode.\n"
        << "<input file>   Specify file to read insertions and queris from.\n"
        << "               If no input file is specified standard input will "
        << "be used.\n\n"
        << "Accepted input is a sequence of non-negative integers in the "
        << "[0..<limit>] range, with negative\n"
        << "integers switching between insertion and query modes. The program "
        << "will start in insert mode.\n\n"
        << "Examples:\n"
        << "   ./query -t 3 -d\n"
        << "         Interactively test the type 3 data structure (unbalanced "
        << "binary tree by default).\n\n"
        << "   /usr/bin/time ./query -t 2 data.txt >> /dev/null\n"
        << "         Benchmark std::unordered set with operations from the "
        << "data.txt file.\n\n"
        << "   /usr/bin/time ./query -s -l 10000 limited_sorted.txt >> "
        << "/dev/null\n"
        << "         Benchmark data with guaranteed sorted and limited input "
        << "sequence.\n"
        << "         Allows program logic to select data structure type."
        << std::endl;
}

/**
 * Executes operations on compatible datasctuctures. Optionally validating the
 * data structure outputs with std::unordered_set.
 *
 * Class template parametes work similarly to generics in java or object
 * polymorphism in python. Here the query_structure template essentially states
 * that the first input parameter to the function is a pointer to a compatible
 * data structure that supports insert and contains operations for integers.
 *
 * Other types of template parameters allow conditional compilation of the input
 * function based on the template parameter value. For example here the validate
 * parameter along with "if constexpr" allows the compiler to compile separate
 * versions of the functions for both different values of validate if required.
 *
 * There is no (or almost no) performance penalty for template use in c++. A
 * completely separate version of the function is compiled for each possible
 * combination of template parameters. If 5 different query structures are
 * supported, along with the two values for validate, a total of 10 different
 * versions of "run_ops" will be compiled in the final binary. This does have
 * some minor performance implications but significantly less than java generic
 * or object polymorphism.
 *
 * @tparam query_structure Type of query strucure.
 * @tparam validate        Should query_structure operations be validated.
 *
 * @param qs    Pointer to query structure to use.
 * @param in    Input stream to use for retreaving operations.
 */
template <class query_structure, bool debug = false, bool validate = false>
void run_ops(query_structure* qs, std::istream& in) {
    // Creats in instance of undordered_set for use with validation.
    // If validation si not used, an optimizing compiler will remove the
    // initialization.
    std::unordered_set<int> us;
    if constexpr (debug) std::cout << "Enter values to add" << std::endl;
    int val;
    bool insert = true;
    // Will execute in a loop untill reaching the end of the input stream.
    while (in) {
        // Read an integer from the given stream. See std::cin >> val.
        in >> val;
        if (val >= 0) {
            if (insert) {
                qs->insert(val);
                // The constexpr keyword tells the compiler that the value of
                // "validate" is known at compile time. Thus, if validate is
                // false "us.insert(val)" will not be in the compiled output and
                // if validate is true it will. Either way there will be no
                // actual branching here in an optimized binary.
                if constexpr (validate) {
                    us.insert(val);
                }
                if constexpr (debug)
                    std::cout << " " << val << " inserted" << std::endl;
            } else {
                if constexpr (validate) {
                    bool res = qs->count(val);
                    if (res != us.count(val)) {
                        std::cerr << "Validation error: contains(" << val
                                  << ") should be " << !res << std::endl;
                        exit(1);
                    }
                }
                if constexpr (debug) {
                    std::cout << val << " : "
                              << (qs->count(val) ? "found" : "not found")
                              << std::endl;
                } else {
                    std::cout << qs->count(val) << std::endl;
                }
            }
        } else {
            if (insert) {
                if constexpr (debug) std::cout << "Enter queries" << std::endl;
                insert = false;
            } else {
                if constexpr (debug)
                    std::cout << "Enter values to add" << std::endl;
                insert = true;
            }
        }
    }
}
/**
 * Logic for determining data structure type if not given, along with code for
 * instantiating the query structure.
 */
template <bool debug = false, bool verify = false>
void select_qs(int type, uint64_t limit, bool separate_queries,
               std::istream& in) {
    // If type was not specified, try to select the best possible data structure
    // based on other parameters. Note that this makes little sense without
    // doing the exercises as there are only 3 types available initially. After
    // task 2, type 4 should be fastest for separate queries, and after task 3,
    // type 5 should be fastest for all cases but massively memory inefficient
    // unless limits are specified. Type 6 is for the extra task due at the end
    // of the course if you want extra points.
    if (type == 0) {
        if (limit > 0 && limit < 10e6) {
            type = 5;
        } else if (separate_queries) {
            type = 4;
        } else {
            type = 6;
        }
    }

    if (type == 1) {
        if constexpr (debug) std::cerr << "Using std::set" << std::endl;
        std::set<int> s;
        run_ops<std::set<int>, debug, verify>(&s, in);
    } else if (type == 2) {
        if constexpr (debug)
            std::cerr << "Using std::unordered_set" << std::endl;
        std::unordered_set<int> us;
        run_ops<std::unordered_set<int>, debug, verify>(&us, in);
    } else if (type == 3) {
        if constexpr (debug)
            std::cerr << "Using vector_set" << std::endl;
        vector_set us;
        run_ops<vector_set, debug, verify>(&us, in);
    } else {
        if constexpr (debug)
            std::cerr << "Using unbalanced binary tree" << std::endl;
        pfp::binary_tree<int> tree;
        run_ops<pfp::binary_tree<int>, debug, verify>(&tree, in);
    }
}
/**
 * The main function parses command line parameters and calls select_qs
 * appropriately
 *
 * There are cleaner and more abstrac ways (as well as simpler ways) to do
 * command line parsing. The approach used here is common in c++ programs used
 * in research at least.
 */
int main(int argc, char const* argv[]) {
    // Change the line below to override default behaviour at CSES.
    // CSES does not support command line parameters (yet).
    // This mean a workaround is required to test custom data structures.
    // For example to submit with the unbalanced binary tree "-t 3" by default
    // you should change the line below to "int type = 3;".
    int type = 0;
    uint64_t limit = (uint32_t(1) << 31) - 1;
    bool separate_queries = false;
    int input_file = 0;
    int verify = false;
    int i = 1;
    bool debug = false;
    while (i < argc) {
        std::string s(argv[i++]);
        if (s.compare("-l") == 0) {
            limit = std::stoull(argv[i++]);
        } else if (s.compare("-s") == 0) {
            separate_queries = true;
        } else if (s.compare("-t") == 0) {
            type = std::stoi(argv[i++]);
        } else if (s.compare("-v") == 0) {
            verify = true;
        } else if (s.compare("-h") == 0) {
            help();
            exit(0);
        } else if (s.compare("-d") == 0) {
            debug = true;
        } else {
            input_file = i - 1;
        }
    }
    if (debug)
        std::cerr << "type = " << type << ", limit = " << limit
                  << ", separate queries = " << separate_queries << std::endl;

    // Both std::cin (console input) and std::ifstream (input file stream)
    // inherit std::istream (input stream).
    // The rediculously verbose way of calling select_qs is due to not being
    // able to passs variables as template parameters, even if they are boolean.
    if (input_file > 0) {
        std::ifstream in(argv[input_file]);
        if (debug) {
            if (verify) {
                select_qs<true, true>(type, limit, separate_queries, in);
            } else {
                select_qs<true, false>(type, limit, separate_queries, in);
            }
        } else {
            if (verify) {
                select_qs<false, true>(type, limit, separate_queries, in);
            } else {
                select_qs<false, false>(type, limit, separate_queries, in);
            }
        }
    } else {
        if (debug) {
            if (verify) {
                select_qs<true, true>(type, limit, separate_queries, std::cin);
            } else {
                select_qs<true, false>(type, limit, separate_queries, std::cin);
            }
        } else {
            if (verify) {
                select_qs<false, true>(type, limit, separate_queries, std::cin);
            } else {
                select_qs<false, false>(type, limit, separate_queries,
                                        std::cin);
            }
        }
    }
    return 0;
}