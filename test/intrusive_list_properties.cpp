#include <stdx/intrusive_forward_list.hpp>
#include <stdx/intrusive_list.hpp>

#include <catch2/catch_test_macros.hpp>
#include <rapidcheck/catch.h>
#include <rapidcheck/state.h>

#include <iterator>
#include <list>

namespace {
struct int_node {
    int value{};
    int_node *prev{};
    int_node *next{};
};
} // namespace

using ListModel = std::list<int>;

template <typename T> struct list_element_deallocator {
    T list{};

    void clear() {
        std::list<int_node *> nodes_to_delete{};

        for (int_node &n : list) {
            nodes_to_delete.push_back(&n);
        }

        list.clear();

        for (int_node *n : nodes_to_delete) {
            delete n;
        }
    }

    ~list_element_deallocator() { clear(); }
};

template <typename ListSut> struct IntrusiveListCommands {
    static void rc_assert_equal(ListModel const &m, ListSut const &sut) {
        RC_ASSERT(m.empty() == sut.list.empty());

        if (!m.empty() && !sut.list.empty()) {
            RC_ASSERT(m.front() == sut.list.front().value);
            RC_ASSERT(m.back() == sut.list.back().value);
        }

        auto mi = m.cbegin();
        auto si = sut.list.cbegin();

        while (mi != m.cend() && si != sut.list.cend()) {
            RC_ASSERT(*mi == si->value);
            mi++;
            si++;
        }

        RC_ASSERT(mi == m.cend());
        RC_ASSERT(si == sut.list.cend());
    }

    struct PushBack : rc::state::Command<ListModel, ListSut> {
        int value;

        PushBack() : value{*rc::gen::inRange(0, 100)} {}

        void checkPreconditions(ListModel const &) const override {}

        void apply(ListModel &m) const override { m.push_back(value); }

        void run(ListModel const &m, ListSut &sut) const override {
            rc_assert_equal(m, sut);
            sut.list.push_back(new int_node{value});
        }

        void show(std::ostream &os) const override {
            os << "push_back(" << value << ")";
        }
    };

    struct PopBack : rc::state::Command<ListModel, ListSut> {
        void checkPreconditions(ListModel const &m) const override {
            RC_PRE(!m.empty());
        }

        void apply(ListModel &m) const override { m.pop_back(); }

        void run(ListModel const &m, ListSut &sut) const override {
            rc_assert_equal(m, sut);
            delete sut.list.pop_back();
        }

        void show(std::ostream &os) const override { os << "pop_back()"; }
    };

    struct PushFront : rc::state::Command<ListModel, ListSut> {
        int value;

        PushFront() : value{*rc::gen::inRange(0, 100)} {}

        void checkPreconditions(ListModel const &) const override {}

        void apply(ListModel &m) const override { m.push_front(value); }

        void run(ListModel const &m, ListSut &sut) const override {
            rc_assert_equal(m, sut);
            sut.list.push_front(new int_node{value});
        }

        void show(std::ostream &os) const override {
            os << "push_front(" << value << ")";
        }
    };

    struct PopFront : rc::state::Command<ListModel, ListSut> {
        void checkPreconditions(ListModel const &m) const override {
            RC_PRE(!m.empty());
        }

        void apply(ListModel &m) const override { m.pop_front(); }

        void run(ListModel const &m, ListSut &sut) const override {
            rc_assert_equal(m, sut);
            delete sut.list.pop_front();
        }

        void show(std::ostream &os) const override { os << "pop_front()"; }
    };

    struct Clear : rc::state::Command<ListModel, ListSut> {
        void checkPreconditions(ListModel const &) const override {}

        void apply(ListModel &m) const override { m.clear(); }

        void run(ListModel const &m, ListSut &sut) const override {
            rc_assert_equal(m, sut);
            sut.clear();
        }

        void show(std::ostream &os) const override { os << "clear()"; }
    };

    struct Remove : rc::state::Command<ListModel, ListSut> {
        int index;

        Remove() : index{*rc::gen::inRange(0, 100)} {}

        void checkPreconditions(ListModel const &m) const override {
            RC_PRE(!m.empty());
        }

        void apply(ListModel &m) const override {
            auto wrapped_index = index % static_cast<int>(m.size());

            auto iter = m.begin();
            std::advance(iter, wrapped_index);
            m.erase(iter);
        }

        void run(ListModel const &m, ListSut &sut) const override {
            rc_assert_equal(m, sut);

            auto wrapped_index = index % static_cast<int>(m.size());

            auto iter = sut.list.begin();
            std::advance(iter, wrapped_index);
            auto p = &(*iter);

            sut.list.remove(p);

            delete p;
        }

        void show(std::ostream &os) const override {
            os << "remove(pointer at index " << index << ")";
        }
    };
};

TEST_CASE("push and pop, front and back, clear",
          "[intrusive_list_properties]") {
    using ListSut = list_element_deallocator<stdx::intrusive_list<int_node>>;
    using Cmds = IntrusiveListCommands<ListSut>;

    rc::prop("", [] {
        ListModel initialState;
        ListSut sut;
        rc::state::check(
            initialState, sut,
            rc::state::gen::execOneOfWithArgs<Cmds::PushBack, Cmds::PopBack,
                                              Cmds::PushFront, Cmds::PopFront,
                                              Cmds::Clear, Cmds::Remove>());
    });
}

TEST_CASE("push and pop, front and back",
          "[intrusive_forward_list_properties]") {
    using ListSut =
        list_element_deallocator<stdx::intrusive_forward_list<int_node>>;
    using Cmds = IntrusiveListCommands<ListSut>;

    rc::prop("", [] {
        ListModel initialState;
        ListSut sut;
        rc::state::check(
            initialState, sut,
            rc::state::gen::execOneOfWithArgs<Cmds::PushBack, Cmds::PushFront,
                                              Cmds::PopFront, Cmds::Clear>());
    });
}
