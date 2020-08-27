#include "test_settings.hpp"

#ifdef ENABLE_TESTS
#include "../utils_lib/external/doctest.hpp"

#include "../utils_lib/utils_memory.hpp"
#include <numeric>


TEST_CASE("Test utils::memory::bit_cast") {
    const int test = 0x40080000;

    struct Bytes { std::byte arr[4]; };
    Bytes b = {std::byte(0x00), std::byte(0x00), std::byte(0x08), std::byte(0x40)};

    REQUIRE(int(b.arr[3]) == 0x40);
    REQUIRE(int(b.arr[2]) == 0x08);
    REQUIRE(int(b.arr[1]) == 0x00);
    REQUIRE(int(b.arr[0]) == 0x00);

    REQUIRE(test == utils::memory::bit_cast<int>(test));
    REQUIRE(doctest::Approx(2.125) == double(utils::memory::bit_cast<float>(test)));
    REQUIRE(doctest::Approx(2.125) == double(utils::memory::bit_cast<float>(b)));
}

TEST_CASE("Test utils::memory::allocVar") {
    int *test     = utils::memory::new_var<int>();
    int *test_val = utils::memory::new_var<int>(0xDEADBEEF);

    REQUIRE(test != nullptr);
    *test = 42;
    CHECK(*test == 42);

    REQUIRE(test_val != nullptr);
    CHECK(*test_val == 0xDEADBEEF);

    utils::memory::delete_var(test);
    utils::memory::delete_var(test_val);
}

TEST_CASE("Test utils::memory::new_unique_var") {
    auto test     = utils::memory::new_unique_var<int>();
    auto test_val = utils::memory::new_unique_var<int>(0xDEADBEEF);

    REQUIRE(test != nullptr);
    *test = 42;
    CHECK(*test == 42);

    REQUIRE(test_val != nullptr);
    CHECK(*test_val == 0xDEADBEEF);
}

TEST_CASE("Test utils::memory::allocArray") {
    auto test_0  = utils::memory::new_array<int>(0);
    auto test_10 = utils::memory::new_array<int>(10);

    REQUIRE(test_0  != nullptr);
    REQUIRE(test_10 != nullptr);

    for (int i = 0; i < 10; i++) {
        CHECK(test_10[i] == 0);
    }

    std::iota(test_10, test_10 + 10, 0);
    for (int i = 0; i < 10; i++) {
        CHECK(test_10[i] == i);
    }

    utils::memory::delete_array(test_0 );
    utils::memory::delete_array(test_10);
}

TEST_CASE("Test utils::memory::allocFlatArray") {
    auto test_0   = utils::memory::new_flat_array<int>(0);
    auto test_10  = utils::memory::new_flat_array<int>(10);
    auto test_2_2 = utils::memory::new_flat_array<int>(2, 2);

    REQUIRE(test_0   != nullptr);
    REQUIRE(test_10  != nullptr);
    REQUIRE(test_2_2 != nullptr);

    std::iota(test_10, test_10 + 10, 0);
    for (int i = 0; i < 10; i++) {
        CHECK(test_10[i] == i);
    }

    std::iota(test_2_2, test_2_2 + 2 * 2, 0);
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            CHECK(test_2_2[j * 2 + i] == j * 2 + i);
        }
    }

    utils::memory::delete_array(test_0 );
    utils::memory::delete_array(test_10);
    utils::memory::delete_array(test_2_2);
}

TEST_CASE("Test utils::memory::reallocArray") {
    size_t length = 0;
    auto test = utils::memory::new_array<int>(length);
    REQUIRE(test != nullptr);

    auto old = test;
    utils::memory::realloc_array(test, length, 10ull);
    REQUIRE(test != nullptr);
    REQUIRE(test != old);
    REQUIRE(length == 10);

    std::iota(test, test + 10, 0);
    for (int i = 0; i < 10; i++) {
        CHECK(test[i] == i);
    }

    old = test;
    utils::memory::realloc_array(test, length, 5ull);
    REQUIRE(test != nullptr);
    REQUIRE(test != old);
    REQUIRE(length == 5);
    for (int i = 0; i < 5; i++) {
        CHECK(test[i] == i);
    }

    old = test;
    utils::memory::realloc_array(test, length, 10ull);
    REQUIRE(test != nullptr);
    REQUIRE(test != old);
    REQUIRE(length == 10);
    for (int i = 0; i < 5; i++) {
        CHECK(test[i] == i);
    }
    for (int i = 5; i < 10; i++) {
        CHECK(test[i] == 0);
    }

    int *empty = nullptr;
    length = 0;
    utils::memory::realloc_array(empty, length, 10ull);
    REQUIRE(empty != nullptr);
    REQUIRE(length == 10);
    for (int i = 0; i < 10; i++) {
        CHECK(empty[i] == 0);
    }

    utils::memory::delete_array(test);
    utils::memory::delete_array(empty);
}

TEST_CASE("Test utils::memory::new_unique_array") {
    auto test_0  = utils::memory::new_unique_array<int>(0);
    auto test_10 = utils::memory::new_unique_array<int>(10);

    REQUIRE(test_0.get()  != nullptr);
    REQUIRE(test_10.get() != nullptr);

    for (size_t i = 0; i < 10; i++) {
        CHECK(test_10[i] == 0);
    }

    std::iota(test_10.get(), test_10.get() + 10, 0);
    for (size_t i = 0; i < 10; i++) {
        CHECK(test_10[i] == i);
    }
}

TEST_CASE("Test utils::memory::new_unique_flat_array") {
    auto test_0   = utils::memory::new_unique_flat_array<int>(0);
    auto test_10  = utils::memory::new_unique_flat_array<int>(10);
    auto test_2_2 = utils::memory::new_unique_flat_array<int>(2, 2);

    REQUIRE(test_0.get()   != nullptr);
    REQUIRE(test_10.get()  != nullptr);
    REQUIRE(test_2_2.get() != nullptr);

    std::iota(test_10.get(), test_10.get() + 10, 0);
    for (size_t i = 0; i < 10; i++) {
        CHECK(test_10[i] == i);
    }

    std::iota(test_2_2.get(), test_2_2.get() + 2 * 2, 0);
    for (size_t i = 0; i < 2; i++) {
        for (size_t j = 0; j < 2; j++) {
            CHECK(test_2_2[j * 2 + i] == j * 2 + i);
        }
    }
}

TEST_CASE("Test utils::memory::deallocContainer") {
    auto uv_p = utils::memory::new_var<std::vector<int*>>();
    REQUIRE(uv_p != nullptr);
    for (int i = 0; i < 10; i++) {
        uv_p->push_back(new int(i));
    }
    REQUIRE(uv_p->size() == 10);
    utils::memory::delete_container(uv_p);

    auto um_p = utils::memory::new_var<std::map<std::string, int*>>();
    REQUIRE(um_p != nullptr);
    for (int i = 0; i < 10; i++) {
        (*um_p)[std::to_string(i)] = new int(i);
    }
    REQUIRE(um_p->size() == 10);
    for (int i = 0; i < 10; i++) {
        CHECK(i == *(*um_p)[std::to_string(i)]);
    }
    utils::memory::delete_container(um_p);
}

TEST_CASE("Test utils::memory::new_unique_container") {
    auto uv_p = utils::memory::new_unique_vector<int>();
    REQUIRE(uv_p.get() != nullptr);

    for (int i = 0; i < 10; i++) {
        uv_p->push_back(new int(i));
    }
    for (int i = 0; i < 10; i++) {
        CHECK(i == *uv_p->at(size_t(i)));
    }

    auto um_p = utils::memory::new_unique_container<std::map<std::string, int*>>();
    REQUIRE(um_p.get() != nullptr);
    for (int i = 0; i < 10; i++) {
        (*um_p)[std::to_string(i)] = new int(i);
    }
    for (int i = 0; i < 10; i++) {
        CHECK(i == *(*um_p)[std::to_string(i)]);
    }

    auto uv = utils::memory::new_unique_container<std::vector<int>>();
    REQUIRE(uv.get() != nullptr);
    for (int i = 0; i < 10; i++) {
        uv->push_back(i);
    }
    for (int i = 0; i < 10; i++) {
        CHECK(i == uv->at(size_t(i)));
    }

    auto um = utils::memory::new_unique_container<std::map<std::string, int>>();
    REQUIRE(um.get() != nullptr);
    for (int i = 0; i < 10; i++) {
        (*um)[std::to_string(i)] = i;
    }
    for (int i = 0; i < 10; i++) {
        CHECK(i == (*um)[std::to_string(i)]);
    }
}

// TODO Other allocator tests
// T** allocArray(size_t x, size_t y)
// deallocArray(T** a, size_t y)
// T*** allocArray(size_t x, size_t y, size_t z)
// deallocArray(T*** a, size_t y, size_t z)

#endif
