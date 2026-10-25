#include "../include/avl.hpp"
#include "../include/rb.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

constexpr int BENCH_REPEATS = 5;    //!< runs per scenario, size, tree
constexpr unsigned BENCH_SEED = 42; //!< fixed seed: reproducible results
const std::vector<std::size_t> BENCH_SIZES{10000, 50000, 100000};
const std::vector<std::string> BENCH_SCENARIOS{"asc_insert", "desc_insert",
                                               "rand_insert", "rand_remove"};

auto read_int(const std::string &prompt, int &out) -> bool {
    std::string line;
    while (true) {
        std::cout << prompt;
        if (!std::getline(std::cin, line))
            return false;

        try {
            std::size_t pos{0};
            const int parsed = std::stoi(line, &pos);
            if (line.find_first_not_of(" \t\r\n", pos) == std::string::npos) {
                out = parsed;
                return true;
            }
        } catch (const std::exception &) {
        }
        std::cout << "invalid input, please type an integer.\n";
    }
}

template <typename TreeType>
auto interactive_menu(TreeType &tree, const std::string &tree_name) -> void {
    int option{-1};
    int value{0};

    do {
        std::cout << "\n--- interacting with " << tree_name << " (size "
                  << tree.size() << ") ---\n";
        std::cout << "1. insert element\n";
        std::cout << "2. remove element\n";
        std::cout << "3. search element\n";
        std::cout << "4. print in order\n";
        std::cout << "5. view tree\n";
        std::cout << "6. check tree invariants\n";
        std::cout << "0. back to main menu\n";
        if (!read_int("choose option: ", option))
            return; // EOF

        switch (option) {
        case 1:
            if (!read_int("type value to insert: ", value))
                return;
            if (tree.insert(value))
                std::cout << "value inserted.\n";
            else
                std::cout << "value already in tree, ignored.\n";
            break;
        case 2:
            if (!read_int("type value to remove: ", value))
                return;
            if (tree.remove(value))
                std::cout << "value removed.\n";
            else
                std::cout << "value not found, nothing removed.\n";
            break;
        case 3:
            if (!read_int("type value to search: ", value))
                return;
            if (tree.search(value)) {
                std::cout << "element " << value << " found in tree.\n";
            } else {
                std::cout << "element " << value << " NOT found in tree.\n";
            }
            break;
        case 4:
            std::cout << "in-order traversal: ";
            tree.print_inorder();
            break;
        case 5:
            std::cout << "tree visual structure:\n";
            tree.print_tree();
            break;
        case 6:
            std::cout << (tree.is_valid() ? "all invariants hold.\n"
                                          : "INVARIANT VIOLATION!\n");
            break;
        case 0:
            std::cout << "returning to main menu...\n";
            break;
        default:
            std::cout << "invalid option\n";
        }
    } while (option != 0);
}

template <typename Tree>
auto matches_oracle(const Tree &tree, const std::set<int> &oracle) -> bool {
    if (!tree.is_valid())
        return false;
    if (tree.size() != oracle.size())
        return false;
    const std::vector<int> expected(oracle.begin(), oracle.end());
    return tree.to_vector() == expected;
}

template <typename Tree>
auto fuzz_test(const std::string &name, unsigned seed, int ops, int range)
    -> bool {
    Tree tree;
    std::set<int> oracle;
    std::mt19937 gen(seed);
    std::uniform_int_distribution<int> value_dist(0, range - 1);
    std::uniform_int_distribution<int> roll_dist(0, 99);

    for (int i = 0; i < ops; ++i) {
        const int value = value_dist(gen);
        const int roll = roll_dist(gen);
        // first half grows the tree, second half shrinks it
        const bool growing = i < ops / 2;
        const int insert_limit = growing ? 60 : 25;
        const int remove_limit = growing ? 85 : 85;

        bool expected{false};
        bool actual{false};
        const char *op{""};

        if (roll < insert_limit) {
            op = "insert";
            expected = oracle.insert(value).second;
            actual = tree.insert(value);
        } else if (roll < remove_limit) {
            op = "remove";
            expected = oracle.erase(value) > 0;
            actual = tree.remove(value);
        } else {
            op = "search";
            expected = oracle.count(value) > 0;
            actual = tree.search(value);
        }

        if (expected != actual || !matches_oracle(tree, oracle)) {
            std::cout << "[FAIL] " << name << " fuzz (seed " << seed << ", op #"
                      << i << "): " << op << "(" << value
                      << ") expected=" << expected << " actual=" << actual
                      << (tree.is_valid() ? "" : " [invariant broken]") << "\n";
            return false;
        }
    }
    return true;
}

template <typename Tree>
auto sequential_test(const std::string &name, int n) -> bool {
    Tree tree;
    std::set<int> oracle;

    auto step_ok = [&](const char *phase, int value) {
        if (matches_oracle(tree, oracle))
            return true;
        std::cout << "[FAIL] " << name << " sequential (" << phase << ", value "
                  << value << ")\n";
        return false;
    };

    for (int i = 1; i <= n; ++i) { // ascending insertion
        tree.insert(i);
        oracle.insert(i);
        if (!step_ok("asc insert", i))
            return false;
    }
    for (int i = 1; i <= n; ++i) { // ascending removal
        tree.remove(i);
        oracle.erase(i);
        if (!step_ok("asc remove", i))
            return false;
    }
    for (int i = n; i >= 1; --i) { // descending insertion
        tree.insert(i);
        oracle.insert(i);
        if (!step_ok("desc insert", i))
            return false;
    }
    for (int i = n; i >= 1; --i) { // descending removal
        tree.remove(i);
        oracle.erase(i);
        if (!step_ok("desc remove", i))
            return false;
    }
    return true;
}

template <typename Tree> auto edge_case_test(const std::string &name) -> bool {
    bool ok = true;
    auto expect = [&](bool condition, const char *what) {
        if (!condition) {
            std::cout << "[FAIL] " << name << " edge case: " << what << "\n";
            ok = false;
        }
    };

    Tree tree;
    expect(!tree.remove(42), "remove on empty tree must return false");
    expect(!tree.search(42), "search on empty tree must return false");
    expect(tree.is_valid() && tree.size() == 0, "empty tree must be valid");

    expect(tree.insert(100), "first insert must succeed");
    expect(!tree.insert(100), "duplicate insert must return false");
    expect(tree.size() == 1, "duplicate must not change the size");
    expect(!tree.remove(42), "remove of non-existent element must be false");
    expect(tree.size() == 1 && tree.search(100), "tree must be unchanged");
    expect(tree.remove(100), "remove of the only element must succeed");
    expect(tree.size() == 0 && tree.is_valid(), "tree must be empty again");
    expect(!tree.search(100), "removed element must not be found");
    return ok;
}

template <typename Tree> auto test_tree(const std::string &name) -> bool {
    bool ok = true;
    ok = edge_case_test<Tree>(name) && ok;
    ok = sequential_test<Tree>(name, 1000) && ok;
    for (unsigned seed = 1; seed <= 10; ++seed) {
        // small value range => many collisions, duplicates and removals of
        // missing values; larger range => deeper trees
        ok = fuzz_test<Tree>(name, seed, 4000, 300) && ok;
        ok = fuzz_test<Tree>(name, seed + 100, 4000, 3000) && ok;
    }
    std::cout << (ok ? "[ OK ] " : "[FAIL] ") << name << "\n";
    return ok;
}

auto run_correctness_tests() -> bool {
    std::cout << "\n--- running correctness tests (invariants + std::set "
                 "oracle after every operation) ---\n";
    bool ok = true;
    ok = test_tree<AVL<int>>("AVL") && ok;
    ok = test_tree<RB<int>>("RB") && ok;
    std::cout << (ok ? "all correctness tests passed.\n"
                     : "SOME TESTS FAILED.\n");
    return ok;
}

struct Sample {
    double ms{0.0};
    int height{0};
    std::size_t rotations{0};
    std::size_t size{0};
    bool valid{false};
};

struct Row {
    std::string scenario;
    std::size_t n;   //!< size of the data set
    std::size_t ops; //!< operations executed in the scenario
    std::string tree;
    int repeat;
    Sample s;
};

struct Summary {
    std::string scenario;
    std::size_t n{0};
    std::size_t ops{0};
    std::string tree;
    int repeats{0};
    double ms_mean{0}, ms_sd{0}, us_per_op{0};
    double height_mean{0}, rotations_mean{0}, rotations_per_op{0};
    bool all_valid{true};
};

template <typename Tree, typename Fn>
auto measure(Tree &tree, Fn &&fn) -> Sample {
    tree.reset_rotations();

    const auto start = std::chrono::steady_clock::now();
    fn(tree);
    const auto end = std::chrono::steady_clock::now();

    Sample s;
    s.ms = std::chrono::duration<double, std::milli>(end - start).count();
    s.rotations = tree.get_rotations();
    s.height = tree.get_tree_height();
    s.size = tree.size();
    s.valid = tree.is_valid();
    return s;
}

template <typename Tree>
auto bench_tree(const std::string &name, std::size_t n, int repeat,
                const std::vector<int> &asc, const std::vector<int> &desc,
                const std::vector<int> &rnd, std::vector<Row> &rows) -> void {
    const std::size_t removals = n / 2;

    {
        Tree tree;
        Sample s = measure(tree, [&](Tree &t) {
            for (int v : asc)
                t.insert(v);
        });
        s.valid = s.valid && s.size == n;
        rows.push_back({"asc_insert", n, n, name, repeat, s});
    }
    {
        Tree tree;
        Sample s = measure(tree, [&](Tree &t) {
            for (int v : desc)
                t.insert(v);
        });
        s.valid = s.valid && s.size == n;
        rows.push_back({"desc_insert", n, n, name, repeat, s});
    }
    {
        Tree tree;
        Sample ins = measure(tree, [&](Tree &t) {
            for (int v : rnd)
                t.insert(v);
        });
        ins.valid = ins.valid && ins.size == n;
        rows.push_back({"rand_insert", n, n, name, repeat, ins});

        // removes the first n/2 elements of the random order, on the tree that
        // was just built
        Sample rem = measure(tree, [&](Tree &t) {
            for (std::size_t i = 0; i < removals; ++i)
                t.remove(rnd[i]);
        });
        rem.valid = rem.valid && rem.size == n - removals;
        rows.push_back({"rand_remove", n, removals, name, repeat, rem});
    }
}

auto run_dataset(std::size_t n, int repeat, unsigned seed,
                 std::vector<Row> &rows) -> void {
    std::vector<int> asc(n);
    std::iota(asc.begin(), asc.end(), 1);
    std::vector<int> desc(asc.rbegin(), asc.rend());
    std::vector<int> rnd = asc;
    std::mt19937 gen(seed + static_cast<unsigned>(repeat) * 7919u +
                     static_cast<unsigned>(n));
    std::shuffle(rnd.begin(), rnd.end(), gen);

    // both trees receive exactly the same data
    bench_tree<AVL<int>>("AVL", n, repeat, asc, desc, rnd, rows);
    bench_tree<RB<int>>("RB", n, repeat, asc, desc, rnd, rows);
}

auto summarize(const std::vector<Row> &rows) -> std::vector<Summary> {
    std::vector<Summary> out;

    for (const auto &scenario : BENCH_SCENARIOS) {
        for (std::size_t n : BENCH_SIZES) {
            for (const char *tree : {"AVL", "RB"}) {
                std::vector<const Row *> sel;
                for (const auto &r : rows)
                    if (r.scenario == scenario && r.n == n && r.tree == tree)
                        sel.push_back(&r);
                if (sel.empty())
                    continue;

                Summary s;
                s.scenario = scenario;
                s.n = n;
                s.ops = sel.front()->ops;
                s.tree = tree;
                s.repeats = static_cast<int>(sel.size());

                const double cnt = static_cast<double>(sel.size());
                double sum_ms{0}, sum_h{0}, sum_rot{0};
                for (const Row *r : sel) {
                    sum_ms += r->s.ms;
                    sum_h += r->s.height;
                    sum_rot += static_cast<double>(r->s.rotations);
                    s.all_valid = s.all_valid && r->s.valid;
                }
                s.ms_mean = sum_ms / cnt;
                s.height_mean = sum_h / cnt;
                s.rotations_mean = sum_rot / cnt;

                double var{0};
                for (const Row *r : sel)
                    var += (r->s.ms - s.ms_mean) * (r->s.ms - s.ms_mean);
                s.ms_sd = cnt > 1 ? std::sqrt(var / (cnt - 1)) : 0.0;

                const double ops = static_cast<double>(s.ops);
                s.us_per_op = s.ms_mean * 1000.0 / ops;
                s.rotations_per_op = s.rotations_mean / ops;
                out.push_back(s);
            }
        }
    }
    return out;
}

auto print_summary(const std::vector<Summary> &summaries, int repeats) -> void {
    std::cout << "\n=== BENCHMARK SUMMARY (mean of " << repeats
              << " runs, seed " << BENCH_SEED << ") ===\n";
    const std::string line(96, '-');

    std::string current;
    for (const auto &s : summaries) {
        if (s.scenario != current) {
            current = s.scenario;
            std::cout << "\n" << current << "\n" << line << "\n";
            std::cout << std::left << std::setw(9) << "n" << std::setw(9)
                      << "ops" << std::setw(6) << "tree" << std::right
                      << std::setw(12) << "time (ms)" << std::setw(10)
                      << "+/- sd" << std::setw(10) << "us/op" << std::setw(9)
                      << "height" << std::setw(13) << "rotations"
                      << std::setw(9) << "rot/op" << std::setw(7) << "ok"
                      << "\n"
                      << line << "\n";
        }
        std::cout << std::left << std::setw(9) << s.n << std::setw(9) << s.ops
                  << std::setw(6) << s.tree << std::right << std::fixed
                  << std::setprecision(3) << std::setw(12) << s.ms_mean
                  << std::setw(10) << s.ms_sd << std::setw(10) << s.us_per_op
                  << std::setprecision(1) << std::setw(9) << s.height_mean
                  << std::setprecision(0) << std::setw(13) << s.rotations_mean
                  << std::setprecision(3) << std::setw(9) << s.rotations_per_op
                  << std::setw(7) << (s.all_valid ? "yes" : "NO") << "\n";
    }
    std::cout << std::defaultfloat << "\n";
}

auto has_csv_extension(const std::string &path) -> bool {
    const std::string ext = ".csv";
    return path.size() > ext.size() &&
           path.compare(path.size() - ext.size(), ext.size(), ext) == 0;
}

auto raw_path(const std::string &path) -> std::string {
    return has_csv_extension(path) ? path : path + ".csv";
}

auto summary_path(const std::string &path) -> std::string {
    const std::string raw = raw_path(path);
    return raw.substr(0, raw.size() - 4) + "_summary.csv";
}

auto write_csv(const std::string &requested_path, const std::vector<Row> &rows,
               const std::vector<Summary> &summaries) -> void {
    const std::string path = raw_path(requested_path);
    const std::string spath = summary_path(requested_path);

    std::ofstream raw(path);
    std::ofstream agg(spath);
    if (!raw || !agg) {
        std::cout << "could not open '" << path << "' / '" << spath
                  << "' for writing; CSV export skipped.\n";
        return;
    }

    raw << "scenario,n,ops,tree,repeat,time_ms,height,rotations,size,valid\n";
    for (const auto &r : rows) {
        raw << r.scenario << ',' << r.n << ',' << r.ops << ',' << r.tree << ','
            << r.repeat << ',' << std::fixed << std::setprecision(4) << r.s.ms
            << ',' << r.s.height << ',' << r.s.rotations << ',' << r.s.size
            << ',' << (r.s.valid ? 1 : 0) << '\n';
    }

    agg << "scenario,n,ops,tree,repeats,time_ms_mean,time_ms_sd,us_per_op,"
           "height_mean,rotations_mean,rotations_per_op,valid\n";
    for (const auto &s : summaries) {
        agg << s.scenario << ',' << s.n << ',' << s.ops << ',' << s.tree << ','
            << s.repeats << ',' << std::fixed << std::setprecision(4)
            << s.ms_mean << ',' << s.ms_sd << ',' << s.us_per_op << ','
            << s.height_mean << ',' << s.rotations_mean << ','
            << s.rotations_per_op << ',' << (s.all_valid ? 1 : 0) << '\n';
    }

    std::cout << "raw runs written to      " << path << "\n"
              << "aggregated data written to " << spath << "\n";
}

auto run_benchmark(const std::string &csv_path) -> void {
    std::cout << "\n--- running benchmark (sizes:";
    for (std::size_t n : BENCH_SIZES)
        std::cout << ' ' << n;
    std::cout << "; removal = n/2; " << BENCH_REPEATS << " runs each) ---\n";

    // warm-up run: avoids skewing the first measurement with cold caches and
    // first-touch page faults
    {
        std::vector<Row> discard;
        run_dataset(20000, -1, BENCH_SEED, discard);
    }

    std::vector<Row> rows;
    for (std::size_t n : BENCH_SIZES) {
        for (int rep = 0; rep < BENCH_REPEATS; ++rep) {
            std::cout << "  n = " << n << ", run " << (rep + 1) << "/"
                      << BENCH_REPEATS << "\r" << std::flush;
            run_dataset(n, rep, BENCH_SEED, rows);
        }
    }
    std::cout << std::string(40, ' ') << "\r";

    const auto summaries = summarize(rows);
    print_summary(summaries, BENCH_REPEATS);
    write_csv(csv_path, rows, summaries);
}

auto print_usage(const char *program) -> void {
    std::cout << "usage:\n"
              << "  " << program << "                  interactive menu\n"
              << "  " << program << " --test           correctness tests\n"
              << "  " << program
              << " --bench [file]   tests + benchmark, CSV in [file] "
                 "(default: results.csv)\n";
}

int main(int argc, char *argv[]) {
    const std::vector<std::string> args(argv + 1, argv + argc);

    if (!args.empty()) {
        if (args[0] == "--test")
            return run_correctness_tests() ? 0 : 1;

        if (args[0] == "--bench") {
            if (!run_correctness_tests())
                return 1;
            run_benchmark(args.size() > 1 ? args[1] : "results.csv");
            return 0;
        }

        print_usage(argv[0]);
        return 2;
    }

    int option{-1};
    AVL<int> avl_tree;
    RB<int> rb_tree;

    do {
        std::cout << "\n============================================\n";
        std::cout << " AMAZON LOGISTICS: BALANCED TREES CHALLENGE\n";
        std::cout << "============================================\n";
        std::cout << "1. interact with AVL tree\n";
        std::cout << "2. interact with red-black tree\n";
        std::cout << "3. run correctness tests\n";
        std::cout << "4. run benchmark (10k / 50k / 100k) and export CSV\n";
        std::cout << "0. exit\n";
        if (!read_int("choose option: ", option))
            break; // EOF

        switch (option) {
        case 1:
            interactive_menu(avl_tree, "AVL tree");
            break;
        case 2:
            interactive_menu(rb_tree, "red-black tree");
            break;
        case 3:
            run_correctness_tests();
            break;
        case 4: {
            std::string path;
            std::cout << "csv output path [results.csv]: ";
            std::getline(std::cin, path);
            run_benchmark(path.empty() ? "results.csv" : path);
            break;
        }
        case 0:
            std::cout << "exiting program...\n";
            break;
        default:
            std::cout << "invalid option\n";
        }
    } while (option != 0);

    return 0;
}
