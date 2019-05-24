#ifndef ALGO_BST_HPP
#define ALGO_BST_HPP
/**
 *  Binary Search Tree
 *  Reference: https://github.com/xorz57/forest  (22/05/2019)
 */

#include <algorithm>
#include <cstddef>
#include <functional>
#include <queue>
#include <utility>


namespace utils::algo {
    template <typename T> class BSTree;

    template <typename T>
    class BSTreeNodeBase {
            template <typename U> friend class BSTree;

        private:
            T *mLeft{nullptr};
            T *mRight{nullptr};
            std::size_t mHeight{1};

        public:
            BSTreeNodeBase() = default;
            ~BSTreeNodeBase() = default;
            BSTreeNodeBase(const BSTreeNodeBase& other) {
                mHeight = other.mHeight;
            }
            BSTreeNodeBase(BSTreeNodeBase&& other) {
                mHeight = other.mHeight;
                other.mHeight = 1;
            }

            BSTreeNodeBase& operator=(const BSTreeNodeBase& other) {
                if (&other == this)
                    return *this;

                mHeight = other.mHeight;
                return *this;
            }

            BSTreeNodeBase& operator=(BSTreeNodeBase&& other) {
                if (&other == this)
                    return *this;

                mHeight = other.mHeight;
                other.mHeight = 1;
                return *this;
            }
    };

    template <typename T>
    class BSNode : public utils::algo::BSTreeNodeBase<BSNode<T>> {
        private:
            int mWeight = 0;
            T   mValue;

        public:
            BSNode() = default;
            BSNode(const int& weight, const T& value)
                : mWeight{weight}, mValue{std::move(value)}
            {
                // Empty
            }
            ~BSNode() = default;

            inline bool operator<(const BSNode& other) const {
                return this->mWeight < other.mWeight;
            }

            friend bool operator<(const BSNode& lhs, const int& rhs) {
                return lhs.mWeight < rhs;
            }

            friend bool operator<(const int& lhs, const BSNode& rhs){
                return lhs < rhs.mWeight;
            }

            int GetWeight() const {
                return this->mWeight;
            }

            inline void SetWeight(const int& key) {
                this->mWeight = key;
            }

            const T& GetValue() const {
                return this->mValue;
            }

            inline void SetValue(const T& value) {
                this->mValue = value;
            }
    };

    template<typename T, typename TChar, typename TCharTraits>
    std::basic_ostream<TChar, TCharTraits>&
    operator<<(
            std::basic_ostream<TChar, TCharTraits>& os,
            const BSNode<T>& node)
    {
        os << "[" << node.GetWeight() << "] "<< node.GetValue();
        return os;
    }


    template <typename T>
    class BSTree {
        public:
            using Callback = std::function<void(T&)>;

        private:
            T *mRoot{nullptr};

            void PreOrderTraversal(T *root, const Callback& callback) {
                if (!root)
                    return;

                callback(*root);
                PreOrderTraversal(root->mLeft, callback);
                PreOrderTraversal(root->mRight, callback);
            }

            void InOrderTraversal(T *root, const Callback& callback) {
                if (!root)
                    return;

                InOrderTraversal(root->mLeft, callback);
                callback(*root);
                InOrderTraversal(root->mRight, callback);
            }

            void PostOrderTraversal(T *root, const Callback& callback) {
                if (!root)
                    return;

                PostOrderTraversal(root->mLeft, callback);
                PostOrderTraversal(root->mRight, callback);
                callback(*root);
            }

            void BreadthFirstTraversal(T *root, const Callback& callback) {
                if (!root)
                    return;

                std::queue<T *> queue;
                queue.push(root);

                while (!queue.empty()) {
                    T *current{queue.front()};
                    callback(*current);
                    queue.pop();

                    if (current->mLeft)
                        queue.push(current->mLeft);

                    if (current->mRight)
                        queue.push(current->mRight);
                }
            }

            T *Minimum(T *root) const {
                if (!root)
                    return nullptr;

                while (root->mLeft)
                    root = root->mLeft;

                return root;
            }

            T *Maximum(T *root) const {
                if (!root)
                    return nullptr;

                while (root->mRight)
                    root = root->mRight;

                return root;
            }

            size_t Height(const T *root) const {
                if (!root)
                    return 0;

                return root->mHeight;
            }

            size_t Size(const T *root) const {
                if (!root)
                    return 0;

                return Size(root->mLeft) + Size(root->mRight) + 1;
            }

            T *Insert(T *root, const T& node) {
                if (!root)
                    return new T(node);

                if (node < *root)
                    root->mLeft = Insert(root->mLeft, node);
                else if (*root < node)
                    root->mRight = Insert(root->mRight, node);

                root->mHeight = std::max(Height(root->mLeft), Height(root->mRight)) + 1;
                return root;
            }

            template <typename Comparable>
            T *Remove(T *root, const Comparable& key) {
                if (!root)
                    return nullptr;

                if (key < *root)
                    root->mLeft = Remove(root->mLeft, key);
                else if (*root < key)
                    root->mRight = Remove(root->mRight, key);
                else {
                    if (!root->mLeft && !root->mRight) {
                        delete root;
                        root = nullptr;
                    } else if (!root->mLeft) {
                        T *tmp{root};
                        root = root->mRight;
                        delete tmp;
                        tmp = nullptr;
                    } else if (!root->mRight) {
                        T *tmp{root};
                        root = root->mLeft;
                        delete tmp;
                        tmp = nullptr;
                    } else {
                        T *min{Minimum(root->mRight)};
                        *root = *min;
                        root->mRight = Remove(root->mRight, *min);
                    }
                }

                if (!root)
                    return nullptr;

                root->mHeight = std::max(Height(root->mLeft), Height(root->mRight)) + 1;
                return root;
            }

            template <typename Comparable>
            T *Search(T *root, const Comparable& key) const {
                while (root) {
                    if (key < *root)
                        root = root->mLeft;
                    else if (*root < key)
                        root = root->mRight;
                    else
                        return root;
                }

                return nullptr;
            }

            void Clear(T *root) {
                if (!root)
                    return;

                if (root->mLeft)
                    Clear(root->mLeft);

                if (root->mRight)
                    Clear(root->mRight);

                delete root;
                root = nullptr;
            }

        public:
            BSTree() = default;
            ~BSTree() { Clear(); }
            BSTree(const BSTree&) = delete;
            BSTree(BSTree&& other) {
                mRoot = other.mRoot;
                other.mRoot = nullptr;
            }

            BSTree& operator=(const BSTree&) = delete;
            BSTree& operator=(BSTree&& other) {
                if (&other == this)
                    return *this;

                mRoot = other.mRoot;
                other.mRoot = nullptr;
                return *this;
            }

            void PreOrderTraversal(const Callback& callback) {
                PreOrderTraversal(mRoot, callback);
            }
            void InOrderTraversal(const Callback& callback) {
                InOrderTraversal(mRoot, callback);
            }
            void PostOrderTraversal(const Callback& callback) {
                PostOrderTraversal(mRoot, callback);
            }
            void BreadthFirstTraversal(const Callback& callback) {
                BreadthFirstTraversal(mRoot, callback);
            }

            T *Minimum() const { return Minimum(mRoot); }
            T *Maximum() const { return Maximum(mRoot); }

            size_t Height() const { return Height(mRoot); }
            size_t Size()   const { return Size(mRoot); }

            void Insert(const T& node) {
                mRoot = Insert(mRoot, node);
            }

            template <typename Key>
            void Remove(const Key& key) {
                mRoot = Remove(mRoot, key);
            }

            template <typename Key>
            T *Search(const Key& key) {
                return Search(mRoot, key);
            }

            void Clear() {
                Clear(mRoot);
                mRoot = nullptr;
            }
    };
}

#endif // ALGO_BST_HPP
