

export module iuic.underlying:utils.tree.decl;
import :erasure;

export namespace iuic::utils::tree {

enum class type { dfs, bfs };

namespace tags {
struct bfs_t {
} constexpr inline bfs{};
struct dfs_t {
} constexpr inline dfs{};
}; // namespace tags

enum class iterator_state { valid, invalid, root };

enum class walk_op {
  to_root,   //
  to_parent, //
  to_childs, //
  r_shift,   // single right shift
  l_shift,   // single left shift
  re_shift,  // shift to right last
  le_shift,  // shift to left last
  end,
};

struct insert_traits {
  enum class bfs_insert_op { ins, sep, deep, end };

  enum class dfs_insert_op { at, to, ret, end };

  template <type t> static consteval auto get_enum_type() {
    if constexpr (t == type::bfs) {
      return bfs_insert_op{};
    } else {
      return dfs_insert_op{};
    };
  };
};

template <type t> using insert_op = decltype(insert_traits::get_enum_type<t>());

// --
template <erasure::as_pure_type T> struct base_iterator;
template <erasure::as_pure_type T> struct access_iterator;
template <erasure::as_pure_type T> struct const_access_iterator;
template <erasure::as_pure_type T> struct root_iterator;
template <erasure::as_pure_type T> struct const_root_iterator;
template <erasure::as_pure_type T> struct sibling_iterator;
template <erasure::as_pure_type T> struct const_sibling_iterator;

// --
template <erasure::as_pure_type T> struct sentinel {
  sentinel(const T &) {}
  sentinel(const base_iterator<T>) {}
  sentinel() {};
};

// --
template <type t, erasure::as_pure_type T> struct insert_iterator;
template <type t, erasure::as_pure_type T> struct copy_iterator;
template <type t, erasure::as_pure_type T> struct move_iterator;

// --
template <erasure::as_pure_type T> struct range_for;
template <erasure::as_pure_type T> struct dfs_range_for;
template <erasure::as_pure_type T> struct bfs_range_for;
template <erasure::as_pure_type T> struct reverse_bfs_range_for;

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

template <typename T, typename U, type t>
insert_iterator<t, T> copy(copy_iterator<t, T>, sentinel<T>,
                           insert_iterator<t, U>);

template <typename T, typename U, type t>
insert_iterator<t, T> move(move_iterator<t, T>, sentinel<T>,
                           insert_iterator<t, U>);

template <typename T> base_iterator<T> begin(T &&);

template <typename T> sentinel<base_iterator<T>> end(T &&) { return {}; };

template <typename T, typename U>
base_iterator<T> shift(base_iterator<T>, base_iterator<U>);

template <typename Src, typename Dest>
void move(move_iterator<type::dfs, Src> it,
          insert_iterator<type::dfs, Dest> ins) {
  using op = insert_op<type::dfs>;

  op op_;

  for (;; it.advance(op_)) {
    switch (op_) {
    case op::at: {
      ins.at(it.get());
    }
    case op::to: {
      ins.to(it.get());
    }
    case op::ret: {
      ins = insert_iterator{++root_iterator{ins}};
    }
    case insert_op<type::dfs>::end: {
      return;
    }
    }
  }
}

template <typename Src, typename Dest>
void move(move_iterator<type::bfs, Src> it,
          insert_iterator<type::bfs, Dest> ins) {
  using op = insert_op<type::bfs>;

  op op_;

  base_iterator pa_begin{ins};
  base_iterator pa_end{ins};
  base_iterator ch_begin{ins};
  base_iterator ch_end{ins};

  for (;; it.advance(op_)) {
    switch (op_) {
    case op::ins: {
      ins.insert(it.get());
    }
    case op::sep: {
      ins.separate(it.get());
    }
    case op::deep: {
      ins = insert_iterator{++root_iterator{ins}};
    }
    case op::end: {
      return;
    }
    }
  }
}

}; // namespace iuic::utils::tree
