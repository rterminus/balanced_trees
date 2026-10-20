#ifndef RB_HPP
#define RB_HPP

#include <cstdint>
#include <initializer_list>
#include <iostream>
#include <string>

template <typename T> class RB {
    using value_type = T; //!< The node value type.

  private:
    enum class Color { RED, BLACK };

    struct Node {
        value_type value{};
        Color color{Color::RED};
        Node *parent{nullptr};
        Node *n_left{nullptr};
        Node *n_right{nullptr};
    };
    using node_pointer = Node *; //!< Pointer to a tree node.

    node_pointer root{};           //!< Red-Black tree root.
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
    auto get_color(node_pointer node) -> Color {
        if (node == nullptr)
            return Color::BLACK; // null nodes are always black

        return node->color;
    }

    // utils
    auto get_min_node(node_pointer node) -> node_pointer {
        while (node->n_left != nullptr) {
            node = node->n_left;
        }
        return node;
    }

    // replaces subtree u with subtree v
    auto transplant(node_pointer u, node_pointer v) -> void {
        if (u->parent == nullptr) {
            root = v;
        } else if (u == u->parent->n_left) {
            u->parent->n_left = v;
        } else {
            u->parent->n_right = v;
        }
        if (v != nullptr) {
            v->parent = u->parent;
        }
    }

    auto fix_insert(node_pointer node) -> void {
        while (node->parent != nullptr && node->parent->color == Color::RED) {
            node_pointer grandfather = node->parent->parent;

            // parent is left child of grandfather
            if (node->parent == grandfather->n_left) {
                node_pointer uncle = grandfather->n_right;

                // case 1: uncle is red (recolor only)
                if (uncle != nullptr && uncle->color == Color::RED) {
                    node->parent->color = Color::BLACK;
                    uncle->color = Color::BLACK;
                    grandfather->color = Color::RED;
                    node = grandfather;
                } else {
                    // case 2: node is right child (rotate left)
                    if (node == node->parent->n_right) {
                        node = node->parent;
                        simple_left_rotation(node);
                    }
                    // case 3: node is left child (recolor and rotate right)
                    node->parent->color = Color::BLACK;
                    grandfather->color = Color::RED;
                    simple_right_rotation(grandfather);
                }
            }
            // parent is right child of grandfather
            else {
                node_pointer uncle = grandfather->n_left;

                // case 1: uncle is red (recolor only)
                if (uncle != nullptr && uncle->color == Color::RED) {
                    node->parent->color = Color::BLACK;
                    uncle->color = Color::BLACK;
                    grandfather->color = Color::RED;
                    node = grandfather;
                } else {
                    // case 2: node is left child (rotate right)
                    if (node == node->parent->n_left) {
                        node = node->parent;
                        simple_right_rotation(node);
                    }
                    // case 3: node is right child (recolor and rotate left)
                    node->parent->color = Color::BLACK;
                    grandfather->color = Color::RED;
                    simple_left_rotation(grandfather);
                }
            }
        }
        root->color = Color::BLACK;
    }

    auto fix_remove(node_pointer x, node_pointer x_parent) -> void {
        while (x != root && get_color(x) == Color::BLACK) {
            // x is left child
            if (x == x_parent->n_left) {
                node_pointer sibling = x_parent->n_right;

                // case 1: sibling is red
                if (get_color(sibling) == Color::RED) {
                    sibling->color = Color::BLACK;
                    x_parent->color = Color::RED;
                    simple_left_rotation(x_parent);
                    sibling = x_parent->n_right;
                }

                // case 2: sibling is black, both children of sibling are black
                if (get_color(sibling->n_left) == Color::BLACK &&
                    get_color(sibling->n_right) == Color::BLACK) {
                    sibling->color = Color::RED;
                    x = x_parent;
                    x_parent = x->parent;
                } else {
                    // case 3: sibling is black, left child is red, right is
                    // black
                    if (get_color(sibling->n_right) == Color::BLACK) {
                        if (sibling->n_left != nullptr)
                            sibling->n_left->color = Color::BLACK;
                        sibling->color = Color::RED;
                        simple_right_rotation(sibling);
                        sibling = x_parent->n_right;
                    }
                    // case 4: sibling is black, right child is red
                    sibling->color = x_parent->color;
                    x_parent->color = Color::BLACK;
                    if (sibling->n_right != nullptr)
                        sibling->n_right->color = Color::BLACK;
                    simple_left_rotation(x_parent);
                    x = root;
                }
            }
            // x is right child
            else {
                node_pointer sibling = x_parent->n_left;

                // case 1: sibling is red
                if (get_color(sibling) == Color::RED) {
                    sibling->color = Color::BLACK;
                    x_parent->color = Color::RED;
                    simple_right_rotation(x_parent);
                    sibling = x_parent->n_left;
                }

                // case 2: sibling is black, both children of sibling are black
                if (get_color(sibling->n_right) == Color::BLACK &&
                    get_color(sibling->n_left) == Color::BLACK) {
                    sibling->color = Color::RED;
                    x = x_parent;
                    x_parent = x->parent;
                } else {
                    // case 3: sibling is black, right child is red, left is
                    // black
                    if (get_color(sibling->n_left) == Color::BLACK) {
                        if (sibling->n_right != nullptr)
                            sibling->n_right->color = Color::BLACK;
                        sibling->color = Color::RED;
                        simple_left_rotation(sibling);
                        sibling = x_parent->n_left;
                    }
                    // case 4: sibling is black, left child is red
                    sibling->color = x_parent->color;
                    x_parent->color = Color::BLACK;
                    if (sibling->n_left != nullptr)
                        sibling->n_left->color = Color::BLACK;
                    simple_right_rotation(x_parent);
                    x = root;
                }
            }
        }
        if (x != nullptr)
            x->color = Color::BLACK;
    }

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
        node_pointer original_parent = node->parent;
        node_pointer right_child = pivot->n_right;

        pivot->n_right = node;
        node->n_left = right_child;

        // link parent pointers
        if (original_parent == nullptr) {
            root = pivot;
            pivot->parent = nullptr;
        } else {
            pivot->parent = original_parent;
            if (original_parent->n_left == node)
                original_parent->n_left = pivot;
            else
                original_parent->n_right = pivot;
        }

        node->parent = pivot;
        if (right_child != nullptr)
            right_child->parent = node;

        return pivot;
    }

    auto simple_left_rotation(node_pointer node) -> node_pointer {
        if (node == nullptr || node->n_right == nullptr)
            return node;

        rotation_count++;

        node_pointer pivot = node->n_right;
        node_pointer original_parent = node->parent;
        node_pointer left_child = pivot->n_left;

        pivot->n_left = node;
        node->n_right = left_child;

        // link parent pointers
        if (original_parent == nullptr) {
            root = pivot;
            pivot->parent = nullptr;
        } else {
            pivot->parent = original_parent;
            if (original_parent->n_left == node)
                original_parent->n_left = pivot;
            else
                original_parent->n_right = pivot;
        }

        node->parent = pivot;
        if (left_child != nullptr)
            left_child->parent = node;

        return pivot;
    }

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
    }

    auto inorder_traversal(node_pointer node) -> void {
        if (node == nullptr)
            return;
        inorder_traversal(node->n_left);
        std::cout << node->value << " ";
        inorder_traversal(node->n_right);
    }

    auto print_tree_visual(node_pointer node, std::string prefix, bool is_left)
        -> void {
        if (node == nullptr)
            return;
        std::cout << prefix << (is_left ? "├── " : "└── ") << node->value
                  << (node->color == Color::RED ? " [V]" : " [P]") << "\n";
        auto next_prefix = prefix;
        if (is_left)
            next_prefix += "│   ";
        else
            next_prefix += "    ";

        print_tree_visual(node->n_left, next_prefix, true);
        print_tree_visual(node->n_right, next_prefix, false);
    }

  public:
    RB() = default;
    ~RB() { destroy_tree(root); }

    // interface operations
    auto insert(value_type value) -> void {
        node_pointer new_node = new Node{value};
        node_pointer current = root;
        node_pointer parent = nullptr;

        // bst traversal to find insertion point
        while (current != nullptr) {
            parent = current;
            if (new_node->value < current->value) {
                current = current->n_left;
            } else if (new_node->value > current->value) {
                current = current->n_right;
            } else {
                delete new_node;
                return; // ignore duplicates
            }
        }

        new_node->parent = parent;

        if (parent == nullptr) {
            root = new_node;
        } else {
            if (new_node->value < parent->value)
                parent->n_left = new_node;
            else
                parent->n_right = new_node;
        }

        fix_insert(new_node);
    }

    auto insert(std::initializer_list<value_type> values) -> void {
        for (const auto &value : values)
            insert(value);
    }

    auto remove(value_type value) -> void {
        node_pointer z = root;

        // search for the node
        while (z != nullptr) {
            if (value == z->value)
                break;
            if (value < z->value)
                z = z->n_left;
            else
                z = z->n_right;
        }

        if (z == nullptr)
            return; // node not found

        node_pointer y = z;
        node_pointer x;
        node_pointer x_parent = nullptr;

        // keep track of the original color to fix tree later if needed
        Color y_original_color = y->color;

        // node has no left child
        if (z->n_left == nullptr) {
            x = z->n_right;
            x_parent = z->parent;
            transplant(z, z->n_right);
        }
        // node has no right child
        else if (z->n_right == nullptr) {
            x = z->n_left;
            x_parent = z->parent;
            transplant(z, z->n_left);
        }
        // node has two children
        else {
            y = get_min_node(z->n_right);
            y_original_color = y->color;
            x = y->n_right;

            if (y->parent == z) {
                x_parent = y;
            } else {
                x_parent = y->parent;
                transplant(y, y->n_right);
                y->n_right = z->n_right;
                y->n_right->parent = y;
            }

            transplant(z, y);
            y->n_left = z->n_left;
            y->n_left->parent = y;
            y->color = z->color;
        }
        delete z;

        // if a black node was removed, rb properties might be violated
        if (y_original_color == Color::BLACK) {
            fix_remove(x, x_parent);
        }
    }

    auto remove(std::initializer_list<value_type> values) -> void {
        for (const auto &value : values)
            remove(value);
    }

    auto search(value_type value) -> bool { return search_node(root, value); }
    auto print_inorder() -> void {
        inorder_traversal(root);
        std::cout << "\n";
    }
    auto print_tree() -> void { print_tree_visual(root, "", false); }

    // testing functions
    auto reset_rotations() -> void { rotation_count = 0; }
    auto get_rotations() const -> std::size_t { return rotation_count; }
    auto get_tree_height() -> std::int16_t { return calculate_height(root); }
};

#endif // !RB_HPP
