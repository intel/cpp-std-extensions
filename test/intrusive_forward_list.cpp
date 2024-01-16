#include <stdx/intrusive_forward_list.hpp>

#include <catch2/catch_test_macros.hpp>

namespace {
struct int_node {
    int value{};
    int_node *next{};
};
#if __cpp_concepts >= 201907L
static_assert(stdx::single_linkable<int_node>);
#endif

struct single_link_node {
    single_link_node *next{};
};

struct bad_single_link_node {
    int *next{};
};
} // namespace

#if __cpp_concepts >= 201907L
TEST_CASE("single_linkable", "[intrusive_forward_list]") {
    static_assert(not stdx::single_linkable<int>);
    static_assert(not stdx::single_linkable<bad_single_link_node>);
    static_assert(stdx::single_linkable<single_link_node>);
}
#endif

TEST_CASE("push_back, pop_front", "[intrusive_forward_list]") {
    stdx::intrusive_forward_list<int_node> list{};
    int_node n{5};

    list.push_back(&n);
    auto poppedNode = list.pop_front();

    CHECK(poppedNode->value == 5);
}

TEST_CASE("empty", "[intrusive_forward_list]") {
    stdx::intrusive_forward_list<int_node> list{};
    int_node n{5};

    CHECK(list.empty());

    list.push_back(&n);
    REQUIRE_FALSE(list.empty());

    list.pop_front();
    CHECK(list.empty());
}

TEST_CASE("push_back 2,  pop_front 2", "[intrusive_forward_list]") {
    stdx::intrusive_forward_list<int_node> list{};
    int_node n1{1};
    int_node n2{2};

    list.push_back(&n1);
    list.push_back(&n2);

    CHECK(list.pop_front()->value == 1);
    CHECK(list.pop_front()->value == 2);
    CHECK(list.empty());
}

TEST_CASE("push_back 2, pop_front 2 (sequentially)",
          "[intrusive_forward_list]") {
    stdx::intrusive_forward_list<int_node> list{};
    int_node n1{1};
    int_node n2{2};

    list.push_back(&n1);
    CHECK(list.pop_front()->value == 1);
    CHECK(list.empty());

    list.push_back(&n2);
    CHECK(list.pop_front()->value == 2);
    CHECK(list.empty());
}

TEST_CASE("push_back 2, pop_front 2 (multiple lists)",
          "[intrusive_forward_list]") {
    stdx::intrusive_forward_list<int_node> listA{};
    stdx::intrusive_forward_list<int_node> listB{};
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

TEST_CASE("begin", "[intrusive_forward_list]") {
    stdx::intrusive_forward_list<int_node> list{};
    int_node n1{1};
    int_node n2{2};
    int_node n3{3};

    list.push_back(&n1);
    list.push_back(&n2);
    list.push_back(&n3);

    CHECK(std::begin(list)->value == 1);
    CHECK(std::cbegin(list)->value == 1);
}

TEST_CASE("iterator preincrement", "[intrusive_forward_list]") {
    stdx::intrusive_forward_list<int_node> list{};
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

TEST_CASE("iterator postincrement", "[intrusive_forward_list]") {
    stdx::intrusive_forward_list<int_node> list{};
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

TEST_CASE("iterator equality", "[intrusive_forward_list]") {
    stdx::intrusive_forward_list<int_node> list{};
    int_node n1{1};

    list.push_back(&n1);

    CHECK(std::begin(list) == std::begin(list));
}

TEST_CASE("iterator inequality", "[intrusive_forward_list]") {
    stdx::intrusive_forward_list<int_node> list{};
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

TEST_CASE("clear", "[intrusive_forward_list]") {
    stdx::intrusive_forward_list<int_node> list{};
    int_node n1{1};
    int_node n2{2};

    list.push_back(&n1);
    list.push_back(&n2);
    list.clear();

    CHECK(list.empty());
}
