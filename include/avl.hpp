#ifndef AVL_HPP
#define AVL_HPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <iostream>
#include <string>
#include <vector>

template <typename T> class AVL {
    using value_type = T;             //!< The node value type.
    using height_type = std::int16_t; //!< Height and balance factor type.

  private:
    struct Node {
        value_type value{};
        height_type height{1};
        Node *n_left{nullptr};
        Node *n_right{nullptr};
    };
    using node_pointer = Node *; //!< Pointer to a tree node.

    node_pointer root{nullptr};    //!< AVL tree root.
    std::size_t node_count{0};     //!< number of stored elements
    std::size_t rotation_count{0}; //!< rotation counter for testing purposes

    // recursive destroyer
    auto destroy_tree(node_pointer node) -> void {
        if (node == nullptr)
            return;

        destroy_tree(node->n_left);
        destroy_tree(node->n_right);
        delete node;
    }

    // getters
    static auto get_height(const Node *node) -> height_type {
        return node == nullptr ? 0 : node->height;
    }
    static auto get_balance_factor(const Node *node) -> height_type {
        if (node == nullptr)
            return 0;

        return get_height(node->n_left) - get_height(node->n_right);
    }

    // utils
    static auto get_min_node(node_pointer node) -> node_pointer {
        while (node->n_left != nullptr) {
            node = node->n_left;
        }

        return node;
    }
    // computes the height of a node from its children's stored heights
    static auto compute_height(const Node *node) -> height_type {
        return 1 +
               std::max(get_height(node->n_left), get_height(node->n_right));
    }
    // full recursive height (independent from the stored heights)
    auto calculate_height(const Node *node) const -> int {
        if (node == nullptr)
            return 0;

        return 1 + std::max(calculate_height(node->n_left),
                            calculate_height(node->n_right));
    }

    // rotations
    auto simple_right_rotation(node_pointer node) -> node_pointer {
        if (node == nullptr || node->n_left == nullptr)
            return node;

        rotation_count++;

        node_pointer pivot = node->n_left;

        node_pointer right_child = pivot->n_right;
        pivot->n_right = node;
        node->n_left = right_child;

        node->height = compute_height(node);
        pivot->height = compute_height(pivot);

        return pivot;
    }
    auto simple_left_rotation(node_pointer node) -> node_pointer {
        if (node == nullptr || node->n_right == nullptr)
            return node;

        rotation_count++;

        node_pointer pivot = node->n_right;

        node_pointer left_child = pivot->n_left;
        pivot->n_left = node;
        node->n_right = left_child;

        node->height = compute_height(node);
        pivot->height = compute_height(pivot);

        return pivot;
    }

    // updates the height of `node` and restores the AVL property, if needed.
    // works for both insertion and removal (decisions use the children's
    // balance factors, never the inserted value).
    auto rebalance(node_pointer node) -> node_pointer {
        node->height = compute_height(node);
        const height_type bf = get_balance_factor(node);

        // left-left case
        if (bf > 1 && get_balance_factor(node->n_left) >= 0) {
            return simple_right_rotation(node);
        }
        // right-right case
        if (bf < -1 && get_balance_factor(node->n_right) <= 0) {
            return simple_left_rotation(node);
        }
        // left-right case, chaining two simple rotations
        if (bf > 1) {
            node->n_left = simple_left_rotation(node->n_left);
            return simple_right_rotation(node);
        }
        // right-left case, chaining two simple rotations
        if (bf < -1) {
            node->n_right = simple_right_rotation(node->n_right);
            return simple_left_rotation(node);
        }

        return node;
    }

    // private operations
    auto insert_node(node_pointer node, const value_type &value, bool &inserted)
        -> node_pointer {
        // standard bst insertion
        if (node == nullptr) {
            inserted = true;
            return new Node{value};
        }

        if (value < node->value) {
            node->n_left = insert_node(node->n_left, value, inserted);
        } else if (node->value < value) {
            node->n_right = insert_node(node->n_right, value, inserted);
        } else {
            return node; // ignore duplicates
        }

        return inserted ? rebalance(node) : node;
    }
    auto remove_node(node_pointer node, const value_type &value, bool &removed)
        -> node_pointer {
        if (node == nullptr)
            return node;

        // standard bst removal
        if (value < node->value) {
            node->n_left = remove_node(node->n_left, value, removed);
        } else if (node->value < value) {
            node->n_right = remove_node(node->n_right, value, removed);
        } else {
            removed = true;

            // node with only one child or no child
            if (node->n_left == nullptr || node->n_right == nullptr) {
                node_pointer child =
                    node->n_left != nullptr ? node->n_left : node->n_right;
                delete node;
                return child;
            }

            // node with two children: copy the inorder successor and remove it
            node_pointer successor = get_min_node(node->n_right);
            node->value = successor->value;
            node->n_right = remove_node(node->n_right, node->value, removed);
        }

        return removed ? rebalance(node) : node;
    }
    auto inorder_traversal(const Node *node) const -> void {
        if (node == nullptr)
            return;

        inorder_traversal(node->n_left);
        std::cout << node->value << " ";
        inorder_traversal(node->n_right);
    }
    auto collect(const Node *node, std::vector<value_type> &out) const -> void {
        if (node == nullptr)
            return;

        collect(node->n_left, out);
        out.push_back(node->value);
        collect(node->n_right, out);
    }
    auto print_tree_visual(const Node *node, const std::string &prefix,
                           bool is_left) const -> void {
        if (node == nullptr)
            return;

        std::cout << prefix << (is_left ? "├── " : "└── ") << node->value
                  << "\n";

        const std::string next_prefix = prefix + (is_left ? "│   " : "    ");

        print_tree_visual(node->n_left, next_prefix, true);
        print_tree_visual(node->n_right, next_prefix, false);
    }

    // returns the real height of the subtree, or -1 if any invariant breaks:
    // bst order, |balance factor| <= 1 and stored height == real height
    auto check_subtree(const Node *node, const value_type *lo,
                       const value_type *hi) const -> int {
        if (node == nullptr)
            return 0;

        if (lo != nullptr && !(*lo < node->value))
            return -1;
        if (hi != nullptr && !(node->value < *hi))
            return -1;

        const int l = check_subtree(node->n_left, lo, &node->value);
        const int r = check_subtree(node->n_right, &node->value, hi);
        if (l < 0 || r < 0)
            return -1;

        if (l - r > 1 || r - l > 1)
            return -1;

        const int real_height = 1 + std::max(l, r);
        if (node->height != real_height)
            return -1;

        return real_height;
    }

  public:
    AVL() = default;
    ~AVL() { destroy_tree(root); }

    // owning raw pointers: copying would double free
    AVL(const AVL &) = delete;
    AVL &operator=(const AVL &) = delete;

    // interface operations (insert/remove return whether the tree changed)
    auto insert(value_type value) -> bool {
        bool inserted{false};
        root = insert_node(root, value, inserted);
        if (inserted)
            ++node_count;
        return inserted;
    }
    auto insert(std::initializer_list<value_type> values) -> void {
        for (const auto &value : values) {
            insert(value);
        }
    }
    auto remove(value_type value) -> bool {
        bool removed{false};
        root = remove_node(root, value, removed);
        if (removed)
            --node_count;
        return removed;
    }
    auto remove(std::initializer_list<value_type> values) -> void {
        for (const auto &value : values) {
            remove(value);
        }
    }
    auto search(const value_type &value) const -> bool {
        const Node *node = root;
        while (node != nullptr) {
            if (value < node->value)
                node = node->n_left;
            else if (node->value < value)
                node = node->n_right;
            else
                return true;
        }
        return false;
    }
    auto size() const -> std::size_t { return node_count; }
    auto print_inorder() const -> void {
        inorder_traversal(root);
        std::cout << "\n";
    }
    auto print_tree() const -> void {
        if (root == nullptr) {
            std::cout << "(empty tree)\n";
            return;
        }
        print_tree_visual(root, "", false);
    }

    // testing functions
    auto reset_rotations() -> void { rotation_count = 0; }
    auto get_rotations() const -> std::size_t { return rotation_count; }
    auto get_tree_height() const -> int { return calculate_height(root); }
    auto to_vector() const -> std::vector<value_type> {
        std::vector<value_type> out;
        out.reserve(node_count);
        collect(root, out);
        return out;
    }
    auto is_valid() const -> bool {
        return check_subtree(root, nullptr, nullptr) >= 0;
    }
};

#endif // !AVL_HPP
