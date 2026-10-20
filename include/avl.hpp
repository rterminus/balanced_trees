#ifndef AVL_HPP
#define AVL_HPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <iostream>
#include <string>

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

    node_pointer root{};           //!< AVL tree root.
    std::size_t rotation_count{0}; //!< rotation counter for testing purposes

    // recursive destroyer
    auto destroy_tree(node_pointer node) -> void {
        if (node == nullptr)
            return;

        destroy_tree(node->n_left);
        destroy_tree(node->n_right);
        delete node;
    };

    // getters
    auto get_height(node_pointer node) -> height_type {
        if (node == nullptr)
            return 0;

        return node->height;
    };
    auto get_balance_factor(node_pointer node) -> height_type {
        if (node == nullptr)
            return 0;

        return get_height(node->n_left) - get_height(node->n_right);
    };

    // utils
    auto get_min_node(node_pointer node) -> node_pointer {
        while (node->n_left != nullptr) {
            node = node->n_left;
        }

        return node;
    };
    auto update_height(node_pointer node) -> height_type {
        return 1 +
               std::max(get_height(node->n_left), get_height(node->n_right));
    };
    auto calculate_height(node_pointer node) -> std::int16_t {
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

        node->height = update_height(node);
        pivot->height = update_height(pivot);

        return pivot;
    };
    auto simple_left_rotation(node_pointer node) -> node_pointer {
        if (node == nullptr || node->n_right == nullptr)
            return node;

        rotation_count++;

        node_pointer pivot = node->n_right;

        node_pointer left_child = pivot->n_left;
        pivot->n_left = node;
        node->n_right = left_child;

        node->height = update_height(node);
        pivot->height = update_height(pivot);

        return pivot;
    };

    // private operations
    auto insert_node(node_pointer node, value_type value) -> node_pointer {
        // standard bst insertion
        if (node == nullptr) {
            return new Node{value};
        }

        if (value < node->value) {
            node->n_left = insert_node(node->n_left, value);
        } else if (value > node->value) {
            node->n_right = insert_node(node->n_right, value);
        } else {
            return node; // ignore duplicates
        }

        node->height = update_height(node);
        height_type bf = get_balance_factor(node);

        // rebalancing: left-left case
        if (bf > 1 && value < node->n_left->value) {
            return simple_right_rotation(node);
        }
        // rebalancing: right-right case
        else if (bf < -1 && value > node->n_right->value) {
            return simple_left_rotation(node);
        }
        // rebalancing: left-right case, chaining two simple rots
        else if (bf > 1 && value > node->n_left->value) {
            node->n_left = simple_left_rotation(node->n_left);
            return simple_right_rotation(node);
        }
        // rebalancing: right-left case, chaining two simple rots
        else if (bf < -1 && value < node->n_right->value) {
            node->n_right = simple_right_rotation(node->n_right);
            return simple_left_rotation(node);
        }

        return node;
    };
    auto remove_node(node_pointer node, value_type value) -> node_pointer {
        if (node == nullptr)
            return node;

        // standard bst removal
        if (value < node->value) {
            node->n_left = remove_node(node->n_left, value);
        } else if (value > node->value) {
            node->n_right = remove_node(node->n_right, value);
        } else {
            // node with only one child or no child
            if (node->n_left == nullptr || node->n_right == nullptr) {
                if (node->n_left != nullptr) {
                    auto new_node = node->n_left;
                    delete node;
                    return new_node;
                } else {
                    auto new_node = node->n_right;
                    delete node;
                    return new_node;
                }
            } else {
                // node with two children: get inorder successor
                auto successor = get_min_node(node->n_right);
                node->value = successor->value;
                node->n_right = remove_node(node->n_right, successor->value);
            }
        }

        if (node == nullptr)
            return node;

        node->height = update_height(node);
        height_type bf = get_balance_factor(node);

        // rebalancing: left-left case
        if (bf > 1 && get_balance_factor(node->n_left) >= 0) {
            return simple_right_rotation(node);
        }
        // rebalancing: right-right case
        else if (bf < -1 && get_balance_factor(node->n_right) <= 0) {
            return simple_left_rotation(node);
        }
        // rebalancing: left-right case, chaining two simple rots
        else if (bf > 1 && get_balance_factor(node->n_left) < 0) {
            node->n_left = simple_left_rotation(node->n_left);
            return simple_right_rotation(node);
        }
        // rebalancing: right-left case, chaining two simple rots
        else if (bf < -1 && get_balance_factor(node->n_right) > 0) {
            node->n_right = simple_right_rotation(node->n_right);
            return simple_left_rotation(node);
        }

        return node;
    };
    auto search_node(node_pointer node, value_type value) -> bool {
        if (node == nullptr)
            return false;

        if (node->value == value)
            return true;

        if (node->value < value)
            return search_node(node->n_right, value);

        if (node->value > value)
            return search_node(node->n_left, value);

        return false;
    };
    auto inorder_traversal(node_pointer node) -> void {
        if (node == nullptr)
            return;

        inorder_traversal(node->n_left);
        std::cout << node->value << " ";
        inorder_traversal(node->n_right);
    };
    auto print_tree_visual(node_pointer node, std::string prefix, bool is_left)
        -> void {
        if (node == nullptr)
            return;

        std::cout << prefix << (is_left ? "├── " : "└── ") << node->value
                  << "\n";

        auto next_prefix = prefix;
        if (is_left) {
            next_prefix += "│   ";
        } else {
            next_prefix += "    ";
        }

        print_tree_visual(node->n_left, next_prefix, true);
        print_tree_visual(node->n_right, next_prefix, false);
    };

  public:
    AVL() = default;
    ~AVL() { destroy_tree(root); };

    // interface operations
    auto insert(value_type value) -> void { root = insert_node(root, value); };
    auto insert(std::initializer_list<value_type> values) -> void {
        for (const auto &value : values) {
            insert(value);
        }
    }
    auto remove(value_type value) -> void { root = remove_node(root, value); };
    auto remove(std::initializer_list<value_type> values) -> void {
        for (const auto &value : values) {
            remove(value);
        }
    }
    auto search(value_type value) -> bool { return search_node(root, value); };
    auto print_inorder() -> void { inorder_traversal(root); };
    auto print_tree() -> void { print_tree_visual(root, "", false); };

    // testing functions
    auto reset_rotations() -> void { rotation_count = 0; }
    auto get_rotations() const -> std::size_t { return rotation_count; }
    auto get_tree_height() -> std::int16_t { return calculate_height(root); }
};

#endif // !AVL_HPP
