#include "../include/avl.hpp"
#include "../include/rb.hpp"
#include <algorithm>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>

auto report(AVL<int> &avl, RB<int> &rb, const std::string &scenario) -> void {
    size_t bar_width{19 + scenario.length()};

    std::cout << "\n=== SCENARIO: " << scenario << " ===\n";
    std::cout << "[AVL] height: " << avl.get_tree_height() << "\n";
    std::cout << " | rotations: " << avl.get_rotations() << "\n";
    std::cout << "[RB] height: " << rb.get_tree_height() << "\n";
    std::cout << " | rotations: " << rb.get_rotations() << "\n";
    std::cout << std::setfill('=') << std::setw(bar_width) << "\n"
              << std::setfill(' ');

    avl.reset_rotations();
    rb.reset_rotations();
}

template <typename TreeType>
auto interactive_menu(TreeType &tree, const std::string &tree_name) -> void {
    int option;
    int value;

    do {
        std::cout << "\n--- interacting with " << tree_name << " ---\n";
        std::cout << "1. insert element\n";
        std::cout << "2. remove element\n";
        std::cout << "3. search element\n";
        std::cout << "4. print in order\n";
        std::cout << "5. view tree\n";
        std::cout << "0. back to main menu\n";
        std::cout << "choose option: ";
        std::cin >> option;

        switch (option) {
        case 1:
            std::cout << "type value to insert: ";
            std::cin >> value;
            tree.insert(value);
            std::cout << "value inserted. duplicates are ignored.\n";
            break;
        case 2:
            std::cout << "type value to remove: ";
            std::cin >> value;
            tree.remove(value);
            std::cout << "value removed.\n";
            break;
        case 3:
            std::cout << "type value to search: ";
            std::cin >> value;
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
        case 0:
            std::cout << "returning to main menu...\n";
            break;
        default:
            std::cout << "invalid option\n";
        }
    } while (option != 0);
}

auto run_stress_tests() -> void {
    const int STRESS_N = 10000;
    const int REMOVE_N = 5000;

    std::cout << "\n--- running automated stress tests (" << STRESS_N
              << " elements) ---\n\n";

    {
        AVL<int> avl_asc;
        RB<int> rb_asc;
        for (int i = 1; i <= STRESS_N; ++i) {
            avl_asc.insert(i);
            rb_asc.insert(i);
        }
        report(avl_asc, rb_asc,
               "ascending insertion (" + std::to_string(STRESS_N) +
                   " elements)");
    }

    {
        AVL<int> avl_desc;
        RB<int> rb_desc;
        for (int i = STRESS_N; i >= 1; --i) {
            avl_desc.insert(i);
            rb_desc.insert(i);
        }
        report(avl_desc, rb_desc,
               "descending insertion (" + std::to_string(STRESS_N) +
                   " elements)");
    }

    std::vector<int> random_data(STRESS_N);
    std::iota(random_data.begin(), random_data.end(), 1);
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(random_data.begin(), random_data.end(), g);

    AVL<int> avl_rand;
    RB<int> rb_rand;
    for (int val : random_data) {
        avl_rand.insert(val);
        rb_rand.insert(val);
    }
    report(avl_rand, rb_rand,
           "random insertion (" + std::to_string(STRESS_N) + " elements)");

    for (int i = 0; i < REMOVE_N; ++i) {
        avl_rand.remove(random_data[i]);
        rb_rand.remove(random_data[i]);
    }
    report(avl_rand, rb_rand,
           "random removal (" + std::to_string(REMOVE_N) + " elements)");

    {
        AVL<int> avl_edge;
        RB<int> rb_edge;
        avl_edge.remove(42);
        rb_edge.remove(42);
        report(avl_edge, rb_edge, "removal on empty tree");

        avl_edge.insert(100);
        rb_edge.insert(100);
        avl_edge.remove(42);
        rb_edge.remove(42);
        report(avl_edge, rb_edge,
               "removal of non-existent element (root only)");
    }
}

int main(int argc, char *argv[]) {
    int option{0};
    AVL<int> avl_tree;
    RB<int> rb_tree;

    do {
        std::cout << "\n============================================\n";
        std::cout << " AMAZON LOGISTICS: BALANCED TREES CHALLENGE\n";
        std::cout << "============================================\n";
        std::cout << "1. interact with AVL tree\n";
        std::cout << "2. interact with red-black tree\n";
        std::cout << "3. run automated stress tests\n";
        std::cout << "0. exit\n";
        std::cout << "choose option: ";
        std::cin >> option;

        switch (option) {
        case 1:
            interactive_menu(avl_tree, "AVL tree");
            break;
        case 2:
            interactive_menu(rb_tree, "red-black tree");
            break;
        case 3:
            run_stress_tests();
            break;
        case 0:
            std::cout << "exiting program...\n";
            break;
        default:
            std::cout << "invalid option\n";
        }
    } while (option != 0);

    return 0;
}
