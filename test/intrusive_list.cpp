#include <stdx/intrusive_list.hpp>

#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <iterator>
#include <string_view>

namespace {
struct int_node {
    int value{};
    int_node *prev{};
    int_node *next{};
};
#if __cpp_concepts >= 201907L
static_assert(stdx::double_linkable<int_node>);
#endif

struct double_link_node {
    double_link_node *prev{};
    double_link_node *next{};
};

struct bad_double_link_node {
    int *prev{};
    int *next{};
};
} // namespace

#if __cpp_concepts >= 201907L
TEST_CASE("double_linkable", "[intrusive_list]") {
    static_assert(not stdx::double_linkable<int>);
    static_assert(not stdx::double_linkable<bad_double_link_node>);
    static_assert(stdx::double_linkable<double_link_node>);
}
#endif

TEST_CASE("push_back, pop_front", "[intrusive_list]") {
    stdx::intrusive_list<int_node> list{};
    int_node n{5};

    list.push_back(&n);
    auto poppedNode = list.pop_front();

    CHECK(poppedNode->value == 5);
}

TEST_CASE("push_front, pop_back", "[intrusive_list]") {
    stdx::intrusive_list<int_node> list{};
    int_node n{5};

    list.push_front(&n);
    auto poppedNode = list.pop_back();

    CHECK(poppedNode->value == 5);
}

TEST_CASE("empty", "[intrusive_list]") {
    stdx::intrusive_list<int_node> list{};
    int_node n{5};

    CHECK(list.empty());

    list.push_back(&n);
    REQUIRE_FALSE(list.empty());

    list.pop_front();
    CHECK(list.empty());
}

TEST_CASE("push_back 2,  pop_front 2", "[intrusive_list]") {
    stdx::intrusive_list<int_node> list{};
    int_node n1{1};
    int_node n2{2};

    list.push_back(&n1);
    list.push_back(&n2);

    CHECK(list.pop_front()->value == 1);
    CHECK(list.pop_front()->value == 2);
    CHECK(list.empty());
}

TEST_CASE("push_back 2, pop_front 2 (sequentially)", "[intrusive_list]") {
    stdx::intrusive_list<int_node> list{};
    int_node n1{1};
    int_node n2{2};

    list.push_back(&n1);
    CHECK(list.pop_front()->value == 1);
    CHECK(list.empty());

    list.push_back(&n2);
    CHECK(list.pop_front()->value == 2);
    CHECK(list.empty());
}

TEST_CASE("push_back 2, pop_front 2 (multiple lists)", "[intrusive_list]") {
    stdx::intrusive_list<int_node> listA{};
    stdx::intrusive_list<int_node> listB{};
    int_node n1{1};
    int_node n2{2};

    listA.push_back(&n1);
    listA.push_back(&n2);
    listA.pop_front();
    listA.pop_front();
    CHECK(listA.empty());

    listB.push_back(&n1);
    CHECK(listB.pop_front()->value == 1);
    CHECK(listB.empty());
}

TEST_CASE("remove middle node", "[intrusive_list]") {
    stdx::intrusive_list<int_node> list{};
    int_node n1{1};
    int_node n2{2};
    int_node n3{3};

    list.push_back(&n1);
    list.push_back(&n2);
    list.push_back(&n3);

    list.remove(&n2);

    CHECK(list.pop_front()->value == 1);
    CHECK(list.pop_front()->value == 3);
    CHECK(list.empty());
}

TEST_CASE("remove only node", "[intrusive_list]") {
    stdx::intrusive_list<int_node> list{};
    int_node n1{1};

    list.push_back(&n1);
    CHECK(not list.empty());

    list.remove(&n1);
    CHECK(list.empty());
}

TEST_CASE("remove first node", "[intrusive_list]") {
    stdx::intrusive_list<int_node> list{};
    int_node n1{1};
    int_node n2{2};

    list.push_back(&n1);
    list.push_back(&n2);

    list.remove(&n1);

    CHECK(list.pop_front()->value == 2);
    CHECK(list.empty());
}

TEST_CASE("remove last node", "[intrusive_list]") {
    stdx::intrusive_list<int_node> list{};
    int_node n1{1};
    int_node n2{2};

    list.push_back(&n1);
    list.push_back(&n2);

    list.remove(&n2);

    CHECK(list.pop_front()->value == 1);
    CHECK(list.empty());
}

TEST_CASE("begin", "[intrusive_list]") {
    stdx::intrusive_list<int_node> list{};
    int_node n1{1};
    int_node n2{2};
    int_node n3{3};

    list.push_back(&n1);
    list.push_back(&n2);
    list.push_back(&n3);

    CHECK(std::begin(list)->value == 1);
    CHECK(std::cbegin(list)->value == 1);
}

TEST_CASE("front and back", "[intrusive_list]") {
    stdx::intrusive_list<int_node> list{};
    int_node n1{1};
    int_node n2{2};
    int_node n3{3};

    list.push_back(&n1);
    list.push_back(&n2);
    list.push_back(&n3);

    CHECK(list.front().value == 1);
    CHECK(list.back().value == 3);
}

TEST_CASE("iterator preincrement", "[intrusive_list]") {
    stdx::intrusive_list<int_node> list{};
    int_node n1{1};
    int_node n2{2};
    int_node n3{3};

    list.push_back(&n1);
    list.push_back(&n2);
    list.push_back(&n3);

    auto i = std::begin(list);
    CHECK(i->value == 1);

    CHECK((++i)->value == 2);
    CHECK(i->value == 2);

    CHECK((++i)->value == 3);
    CHECK(i->value == 3);
}

TEST_CASE("iterator postincrement", "[intrusive_list]") {
    stdx::intrusive_list<int_node> list{};
    int_node n1{1};
    int_node n2{2};
    int_node n3{3};

    list.push_back(&n1);
    list.push_back(&n2);
    list.push_back(&n3);

    auto i = std::begin(list);
    CHECK(i->value == 1);

    CHECK((i++)->value == 1);
    CHECK(i->value == 2);

    CHECK((i++)->value == 2);
    CHECK(i->value == 3);
}

TEST_CASE("iterator equality", "[intrusive_list]") {
    stdx::intrusive_list<int_node> list{};
    int_node n1{1};

    list.push_back(&n1);
    CHECK(not list.empty());
    CHECK(std::begin(list) == std::begin(list));
}

TEST_CASE("iterator inequality", "[intrusive_list]") {
    stdx::intrusive_list<int_node> list{};
    int_node n1{1};
    int_node n2{2};
    int_node n3{3};

    list.push_back(&n1);
    list.push_back(&n2);
    list.push_back(&n3);

    auto i = std::begin(list);
    CHECK(i == std::begin(list));
    CHECK(i != std::end(list));

    i++;
    CHECK(i != std::begin(list));
    CHECK(i != std::end(list));

    i++;
    CHECK(i != std::begin(list));
    CHECK(i != std::end(list));

    i++;
    CHECK(i != std::begin(list));
    CHECK(i == std::end(list));
}

TEST_CASE("clear", "[intrusive_list]") {
    stdx::intrusive_list<int_node> list{};
    int_node n1{1};
    int_node n2{2};

    list.push_back(&n1);
    CHECK(not list.empty());
    list.push_back(&n2);
    list.clear();

    CHECK(list.empty());
}

TEST_CASE("remove and re-add same node", "[intrusive_list]") {
    stdx::intrusive_list<int_node> list{};
    int_node n1{1};
    int_node n2{2};

    list.push_back(&n2);
    list.push_back(&n1);

    list.remove(&n2);
    list.push_back(&n2);
    CHECK(list.pop_front()->value == 1);
    CHECK(list.pop_front()->value == 2);
    CHECK(list.empty());
}

TEST_CASE("remove from empty list", "[intrusive_list]") {
    stdx::intrusive_list<int_node> list{};
    int_node n1{1};
    list.remove(&n1);
    CHECK(list.empty());
}

TEST_CASE("checked operation clears pointers on pop", "[intrusive_list]") {
    stdx::intrusive_list<int_node> list{};
    int_node n1{1};
    int_node n2{2};
    list.push_back(&n1);
    list.push_back(&n2);

    CHECK(n2.prev != nullptr);
    CHECK(list.pop_back() == &n2);
    CHECK(n2.prev == nullptr);

    list.push_back(&n2);

    CHECK(n1.next != nullptr);
    CHECK(list.pop_front() == &n1);
    CHECK(n1.next == nullptr);
}

TEST_CASE("checked operation clears pointers on remove", "[intrusive_list]") {
    stdx::intrusive_list<int_node> list{};
    int_node n1{1};
    int_node n2{2};
    list.push_back(&n1);
    list.push_back(&n2);

    CHECK(n2.prev != nullptr);
    list.remove(&n2);
    CHECK(n2.prev == nullptr);
}

TEST_CASE("checked operation clears pointers on clear", "[intrusive_list]") {
    stdx::intrusive_list<int_node> list{};
    int_node n1{1};
    int_node n2{2};
    list.push_back(&n1);
    list.push_back(&n2);

    CHECK(n1.next != nullptr);
    CHECK(n2.prev != nullptr);
    list.clear();
    CHECK(n1.next == nullptr);
    CHECK(n2.prev == nullptr);
}

namespace {
#if __cplusplus >= 202002L
int compile_time_calls{};
#else
int runtime_calls{};
#endif

struct injected_handler {
#if __cplusplus >= 202002L
    template <stdx::ct_string Why, typename... Ts>
    static auto panic(Ts &&...) noexcept -> void {
        static_assert(std::string_view{Why} == "bad list node!");
        ++compile_time_calls;
    }
#else
    template <typename Why, typename... Ts>
    static auto panic(Why why, Ts &&...) noexcept -> void {
        CHECK(std::string_view{why} == "bad list node!");
        ++runtime_calls;
    }
#endif
};
} // namespace

template <> inline auto stdx::panic_handler<> = injected_handler{};

#if __cplusplus >= 202002L
TEST_CASE("checked panic when pushing populated node", "[intrusive_list]") {
    stdx::intrusive_list<int_node> list{};
    int_node n{5};

    n.prev = &n;
    compile_time_calls = 0;
    list.push_back(&n);
    CHECK(compile_time_calls == 1);
    list.pop_back();

    n.prev = &n;
    compile_time_calls = 0;
    list.push_front(&n);
    CHECK(compile_time_calls == 1);
}
#else
TEST_CASE("checked panic when pushing populated node", "[intrusive_list]") {
    stdx::intrusive_list<int_node> list{};
    int_node n{5};

    n.prev = &n;
    runtime_calls = 0;
    list.push_back(&n);
    CHECK(runtime_calls == 1);
    list.pop_back();

    n.prev = &n;
    runtime_calls = 0;
    list.push_front(&n);
    CHECK(runtime_calls == 1);
}
#endif

TEST_CASE("unchecked operation doesn't clear pointers", "[intrusive_list]") {
    stdx::intrusive_list<int_node, stdx::node_policy::unchecked> list{};
    int_node n1{1};
    int_node n2{2};
    list.push_back(&n1);
    list.push_back(&n2);

    auto before = n2.prev;
    CHECK(list.pop_back() == &n2);
    CHECK(n2.prev == before);

    list.push_back(&n2);

    before = n1.next;
    CHECK(list.pop_front() == &n1);
    CHECK(n1.next == before);
}

TEST_CASE("insert into empty list", "[intrusive_list]") {
    stdx::intrusive_list<int_node> list{};
    int_node n1{1};
    list.insert(std::begin(list), &n1);

    CHECK(list.front().value == 1);
    CHECK(list.back().value == 1);
    CHECK(list.front().next == nullptr);
    CHECK(list.front().prev == nullptr);
}

TEST_CASE("insert at begin", "[intrusive_list]") {
    stdx::intrusive_list<int_node> list{};
    int_node n1{1};
    int_node n2{2};

    list.push_back(&n1);
    list.insert(std::begin(list), &n2);

    CHECK(list.front().value == 2);
    CHECK(list.back().value == 1);

    CHECK(list.front().next == &n1);
    CHECK(n1.prev == &list.front());
}

TEST_CASE("insert at end", "[intrusive_list]") {
    stdx::intrusive_list<int_node> list{};
    int_node n1{1};
    int_node n2{2};

    list.push_back(&n1);
    list.insert(std::end(list), &n2);

    CHECK(list.front().value == 1);
    CHECK(list.back().value == 2);

    CHECK(list.back().prev == &n1);
    CHECK(n1.next == &list.back());
}

TEST_CASE("insert in middle", "[intrusive_list]") {
    stdx::intrusive_list<int_node> list{};
    int_node n1{1};
    int_node n2{2};
    int_node n3{3};

    list.push_back(&n1);
    list.push_back(&n3);
    list.insert(std::next(std::begin(list)), &n2);

    CHECK(list.front().value == 1);
    CHECK(list.back().value == 3);

    CHECK(list.front().next == &n2);
    CHECK(n2.prev == &list.front());

    CHECK(list.back().prev == &n2);
    CHECK(n2.next == &list.back());
}

TEST_CASE("insert use case", "[intrusive_list]") {
    stdx::intrusive_list<int_node> list{};
    int_node n1{1};
    int_node n2{2};

    list.push_back(&n1);
    list.push_back(&n2);

    int_node n{2};
    {
        auto it =
            std::find_if(std::begin(list), std::end(list),
                         [&](auto &node) { return n.value <= node.value; });
        list.insert(it, &n);
        CHECK(n2.prev == &n);
        CHECK(n.prev == &n1);
    }

    auto it = std::cbegin(list);
    CHECK(it->value == 1);
    ++it;
    CHECK(it->value == 2);
    ++it;
    CHECK(it->value == 2);
    ++it;
    CHECK(it == std::cend(list));
}
