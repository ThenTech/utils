#include "test_settings.hpp"

#ifdef ENABLE_TESTS
#include "../utils_lib/external/catch.hpp"

#include "../utils_lib/utils_memory.hpp"

TEST_CASE("Test utils::memory::allocVar", "[utils][utils::memory]") {
    int *test     = utils::memory::allocVar<int>();
    int *test_val = utils::memory::allocVar<int>(0xDEADBEEF);

    REQUIRE(test != nullptr);
    *test = 42;
    CHECK(*test == 42);

    REQUIRE(test_val != nullptr);
    CHECK(*test_val == 0xDEADBEEF);

    utils::memory::deallocVar(test);
    utils::memory::deallocVar(test_val);
}

TEST_CASE("Test utils::memory::new_unique_var", "[utils][utils::memory]") {
    auto test     = utils::memory::new_unique_var<int>();
    auto test_val = utils::memory::new_unique_var<int>(0xDEADBEEF);

    REQUIRE(test != nullptr);
    *test = 42;
    CHECK(*test == 42);

    REQUIRE(test_val != nullptr);
    CHECK(*test_val == 0xDEADBEEF);
}

TEST_CASE("Test utils::memory::allocArray", "[utils][utils::memory]") {
    auto test_0  = utils::memory::allocArray<int>(0);
    auto test_10 = utils::memory::allocArray<int>(10);

    REQUIRE(test_0  != nullptr);
    REQUIRE(test_10 != nullptr);

    for (int i = 0; i < 10; i++) {
        CHECK(test_10[i] == 0);
    }

    std::iota(test_10, test_10 + 10, 0);
    for (int i = 0; i < 10; i++) {
        CHECK(test_10[i] == i);
    }

    utils::memory::deallocArray(test_0 );
    utils::memory::deallocArray(test_10);
}

TEST_CASE("Test utils::memory::allocFlatArray", "[utils][utils::memory]") {
    auto test_0   = utils::memory::allocFlatArray<int>(0);
    auto test_10  = utils::memory::allocFlatArray<int>(10);
    auto test_2_2 = utils::memory::allocFlatArray<int>(2, 2);

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

    utils::memory::deallocArray(test_0 );
    utils::memory::deallocArray(test_10);
    utils::memory::deallocArray(test_2_2);
}

TEST_CASE("Test utils::memory::reallocArray", "[utils][utils::memory]") {
    size_t length = 0;
    auto test = utils::memory::allocArray<int>(length);
    REQUIRE(test != nullptr);

    auto old = test;
    utils::memory::reallocArray(test, length, 10ull);
    REQUIRE(test != nullptr);
    REQUIRE(test != old);
    REQUIRE(length == 10);

    std::iota(test, test + 10, 0);
    for (int i = 0; i < 10; i++) {
        CHECK(test[i] == i);
    }

    old = test;
    utils::memory::reallocArray(test, length, 5ull);
    REQUIRE(test != nullptr);
    REQUIRE(test != old);
    REQUIRE(length == 5);
    for (int i = 0; i < 5; i++) {
        CHECK(test[i] == i);
    }

    old = test;
    utils::memory::reallocArray(test, length, 10ull);
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
    utils::memory::reallocArray(empty, length, 10ull);
    REQUIRE(empty != nullptr);
    REQUIRE(length == 10);
    for (int i = 0; i < 10; i++) {
        CHECK(empty[i] == 0);
    }

    utils::memory::deallocArray(test);
    utils::memory::deallocArray(empty);
}

TEST_CASE("Test utils::memory::new_unique_array", "[utils][utils::memory]") {
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

TEST_CASE("Test utils::memory::new_unique_flat_array", "[utils][utils::memory]") {
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

// TODO Other allocator tests
// T** allocArray(size_t x, size_t y)
// deallocArray(T** a, size_t y)
// T*** allocArray(size_t x, size_t y, size_t z)
// deallocArray(T*** a, size_t y, size_t z)

// deallocVector
// new_unique_vector
// deallocMap

#endif
