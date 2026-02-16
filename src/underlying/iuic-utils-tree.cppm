

export module iuic.underlying:utils.tree.decl;
import :erasure;

export namespace iuic::utils::tree {

struct bfs_tag {};
struct dfs_tag {};
// --
template <erasure::as_pure_type T> struct sentinel {
  sentinel(const T &) {}
};

// --
template <erasure::as_pure_type T> struct base_iterator;
template <erasure::as_pure_type T> struct access_iterator;
template <erasure::as_pure_type T> struct const_access_iterator;
template <erasure::as_pure_type T> struct root_iterator;
template <erasure::as_pure_type T> struct const_root_iterator;
template <erasure::as_pure_type T> struct sibling_iterator;
template <erasure::as_pure_type T> struct const_sibling_iterator;
template <erasure::as_pure_type T> struct insert_iterator;

// --
// using approach = bfs_tag;
template <erasure::as_pure_type T> struct copy_iterator;
template <erasure::as_pure_type T> struct move_iterator;

// --
template <erasure::as_pure_type T> struct bfs_iterator;
template <erasure::as_pure_type T> struct dfs_iterator;

// --
template <erasure::as_pure_type T> struct range_for;
template <erasure::as_pure_type T> struct dfs_range_for;
template <erasure::as_pure_type T> struct bfs_range_for;

struct invalide_iterator_type {};

template <typename T> struct traits {};

template <typename T>
concept is_base_iterator =
    std::same_as<T, typename traits<std::remove_cvref_t<T>>::base_iterator_t> &&
    requires {
      typename T::container_t;
      typename T::value_t;
      typename T::lvalue_t;
      typename T::rvalue_t;
      typename T::pointer_t;
    };

template <typename T>
concept is_access_iterator = requires(T iterator) {
  {
    typename traits<std::remove_cvref_t<T>>::base_iterator_t{iterator}
  } -> is_base_iterator;
};

template <typename T>
concept is_sibling_iterator = requires(T iterator) {
  {
    typename traits<std::remove_cvref_t<T>>::base_iterator_t{iterator}
  } -> is_base_iterator;
  {
    typename traits<std::remove_cvref_t<T>>::access_iterator_t{iterator}
  } -> is_access_iterator;
};

template <typename T>
concept is_root_iterator = requires(T iterator) {
  {
    typename traits<std::remove_cvref_t<T>>::base_iterator_t{iterator}
  } -> is_base_iterator;
  {
    typename traits<std::remove_cvref_t<T>>::access_iterator_t{iterator}
  } -> is_access_iterator;
};

template <typename T>
concept is_insert_iterator =
    requires(T iterator, typename T::lvalue_t l, typename T::rvalue_t r) {
      {
        typename traits<std::remove_cvref_t<T>>::base_iterator_t{iterator}
      } -> is_base_iterator;
      {
        typename traits<std::remove_cvref_t<T>>::access_iterator_t{iterator}
      } -> is_access_iterator;
      iterator = l;
      iterator = std::move(r);
    };

template <typename T>
consteval access_iterator<T> access_iterator_from(sibling_iterator<T>);

template <typename T> struct traits<base_iterator<T>> {
  using base_iterator_t = base_iterator<T>;
  using container_t = T;
};

template <typename T> struct traits<access_iterator<T>> {
  using base_iterator_t = base_iterator<T>;
};

template <typename T> struct traits<sibling_iterator<T>> {
  using base_iterator_t = base_iterator<T>;
  using access_iterator_t = access_iterator<T>;
};

// end traits

template <typename T>
auto childs_of(base_iterator<T> it)
  requires is_base_iterator<decltype(it)>;

template <typename T>
auto siblings_of(base_iterator<T> it)
  requires is_base_iterator<decltype(it)>;

template <typename T>
auto parent_of(base_iterator<T> it)
  requires is_base_iterator<decltype(it)>;

template <typename T>
concept has_tree_walk = requires(T iterator) {
  { root_of(iterator) } -> is_root_iterator;
  { childs_of(iterator) } -> is_sibling_iterator;
  { siblings_of(iterator) } -> is_sibling_iterator;
};

template <typename T, typename U>
insert_iterator<T> copy(copy_iterator<T>, sentinel<T>, insert_iterator<U>);

template <typename T, typename U>
insert_iterator<T> move(move_iterator<T>, sentinel<T>, insert_iterator<U>);

template <typename T> base_iterator<T> begin(T &&);

template <typename T> sentinel<base_iterator<T>> end(T &&) { return {}; };

template <erasure::as_pure_type T>
  requires has_tree_walk<T>
struct copy_iterator<T> {
  using approach = bfs_tag;
  using base_iterator_t = typename traits<T>::base_iterator;
  using lvalue_t = typename traits<T>::base_iterator::lvalue_t;

  // walk or select
  void next_op();

  lvalue_t get() { return *access_iterator{it}; };

  bool operator==(sentinel<copy_iterator>);

private:
  base_iterator_t it;
};
}; // namespace iuic::utils::tree
