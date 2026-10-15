#include "../include/avl.hpp"

int main (int argc, char *argv[]) {
    AVL<int> tree;

    tree.insert({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});

    tree.print_tree();

    return 0;
}
