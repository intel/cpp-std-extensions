from hypothesis import strategies as st, given, settings, event, assume

def unpack(l):
    return ", ".join([str(i) for i in l])

small_ints = st.integers(min_value=-100, max_value=100)

@st.composite
def tuples(draw, children):
    values = draw(st.lists(children))
    return f"stdx::make_tuple({unpack(values)})"

@st.composite
def list_trees(draw, leaves=st.integers(), max_leaves=100):
    l = draw(st.recursive(leaves, lambda children: st.lists(children), max_leaves=max_leaves))
    if not isinstance(l, list):
        l = [l]
    return l

def as_tuple_tree(value):
    if isinstance(value, list):
        values = [as_tuple_tree(v) for v in value]
        return f"stdx::make_tuple({unpack(values)})"
    else:
        return value

@st.composite
def tuple_trees(draw, leaves=st.integers()):
    return draw(st.recursive(leaves, lambda children: tuples(children)))

@settings(deadline=50000)
@given(tuple_trees(small_ints))
def test_tuple_trees(compile, t):
    assert compile(f"""
        #include <stdx/tuple.hpp>

        [[maybe_unused]] constexpr auto t = {t};

        int main() {{
            return 0;
        }}
    """)

@settings(deadline=50000)
@given(list_trees(small_ints))
def test_tuple_size(compile, l):
    t = as_tuple_tree(l)
    assert compile(f"""
        #include <stdx/tuple.hpp>

        constexpr auto t = {t};
        static_assert(stdx::tuple_size_v<decltype(t)> == {len(l)});
        static_assert(std::size(t) == {len(l)});

        int main() {{
            return 0;
        }}
    """)


@settings(deadline=50000)
@given(list_trees(small_ints), st.integers())
def test_get_by_index(compile, l, i):
    assume(len(l) > 0)
    t = as_tuple_tree(l)
    i = i % len(l)

    expected_v = as_tuple_tree(l[i])

    assert compile(f"""
        #include <stdx/tuple.hpp>

        using namespace stdx::literals;

        constexpr auto t = {t};
        constexpr auto expected = {expected_v};
        
        static_assert(stdx::get<{i}>(t) == expected);
        static_assert(get<{i}>(t) == expected);

        static_assert(t[{i}_idx] == expected);
        static_assert(t[stdx::index<{i}>] == expected);

        int main() {{
            return 0;
        }}
    """)


@settings(deadline=50000)
@given(st.lists(list_trees(small_ints)))
def test_tuple_cat(compile, ls):
    ts = [as_tuple_tree(l) for l in ls]

    flattened_ls = [i for subl in ls for i in subl]
    expected = as_tuple_tree(flattened_ls)

    assert compile(f"""
        #include <stdx/tuple.hpp>
        #include <stdx/tuple_algorithms.hpp>

        static_assert(stdx::tuple_cat({unpack(ts)}) == {expected});

        int main() {{
            return 0;
        }}
    """)


@settings(deadline=50000)
@given(list_trees(small_ints), st.one_of(list_trees(small_ints), small_ints))
def test_push(compile, l, elem):
    expected_back = as_tuple_tree(l + [elem])
    expected_front = as_tuple_tree([elem] + l)

    if isinstance(elem, list):
        elem = as_tuple_tree(elem)
    else:
        elem = str(elem)

    t = as_tuple_tree(l)

    assert compile(f"""
        #include <stdx/tuple.hpp>
        #include <stdx/tuple_algorithms.hpp>

        constexpr auto t = {t};
        constexpr auto elem = {elem};

        constexpr auto expected_back = {expected_back};
        static_assert(stdx::tuple_push_back(t, elem) == expected_back);
        static_assert(stdx::tuple_snoc(t, elem) == expected_back);

        constexpr auto expected_front = {expected_front};
        static_assert(stdx::tuple_push_front(elem, t) == expected_front);
        static_assert(stdx::tuple_cons(elem, t) == expected_front);

        int main() {{
            return 0;
        }}
    """)

from itertools import product

def put_in_list(i):
    if isinstance(i, list):
        return i
    else:
        return [i]

@settings(deadline=50000)
@given(list_trees(small_ints, max_leaves=15))
def test_cartesian_product(compile, ls):
    ls = [put_in_list(i) for i in ls]
    ts = [as_tuple_tree(l) for l in ls]
    expected = as_tuple_tree([list(p) for p in product(*ls)])

    assert compile(f"""
        #include <stdx/tuple.hpp>
        #include <stdx/tuple_algorithms.hpp>

        static_assert(stdx::cartesian_product_copy({unpack(ts)}) == {expected});

        int main() {{
            return 0;
        }}
    """)

from functools import reduce

@settings(deadline=50000)
@given(st.lists(small_ints))
def test_star_of(compile, l):
    expected_any_of = any([i > 50 for i in l])
    expected_all_of = all([i > 50 for i in l])
    expected_none_of = not expected_any_of

    t = as_tuple_tree(l)

    assert compile(f"""
        #include <stdx/tuple.hpp>
        #include <stdx/tuple_algorithms.hpp>

        constexpr auto f = [](int i) {{ return i > 50; }};

        static_assert(stdx::any_of(f, {t}) == {str(expected_any_of).lower()});
        static_assert(stdx::all_of(f, {t}) == {str(expected_all_of).lower()});
        static_assert(stdx::none_of(f, {t}) == {str(expected_none_of).lower()});

        int main() {{
            return 0;
        }}
    """)