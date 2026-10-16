#include "../include/avl.hpp"

int main(int argc, char *argv[]) {
    AVL<int> tree1;
    tree1.insert({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
    tree1.print_tree();

    AVL<int> tree2;
    tree2.insert({10, 20, 30, 40, 50, 25});
    tree2.print_tree();

    return 0;
}
