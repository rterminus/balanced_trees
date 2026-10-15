#ifndef AVL_HPP
#define AVL_HPP

#include <cstdint>
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

    node_pointer root{}; //!< AVL tree root.

    // recursive destroyer
    auto destroy_tree(node_pointer node) -> void;

    // getters
    auto get_height(node_pointer node) -> height_type;
    auto get_balance_factor(node_pointer node) -> height_type;

    // utils
    auto get_min_node(node_pointer node) -> node_pointer;

    // rotations
    auto simple_right_rotation(node_pointer node) -> node_pointer;
    auto simple_left_rotation(node_pointer node) -> node_pointer;

    // private operations
    auto insert_node(node_pointer node, value_type value) -> node_pointer;
    auto remove_node(node_pointer node, value_type value) -> node_pointer;
    auto search_node(node_pointer node, value_type value) -> bool;
    auto inorder_traversal(node_pointer node) -> void;
    auto print_tree_visual(node_pointer node, std::string prefix, bool is_left)
        -> void;

  public:
    AVL() = default;
    ~AVL();

    // interface operations
    auto insert(value_type value) -> void;
    auto remove(value_type value) -> void;
    auto search(value_type value) -> bool;
    auto print_inorder() -> void;
    auto print_tree() -> void;
};

#endif // !AVL_HPP
