#include "test_settings.hpp"

#ifdef ENABLE_TESTS
#include "../utils_lib/external/doctest.hpp"

#include "../utils_lib/utils_csv.hpp"


TEST_CASE("Test utils::csv") {
    utils::csv::Reader file;
    file.use_dialect("excel");
    file.read("./utils_lib/external/csv/tests/inputs/test_11_excel.csv");

    int data = 1;

    while (file.busy()) {
        if (file.ready()) {
            auto row = file.next_row();
            // Each row is a csv::unordered_flat_map (github.com/martinus/robin-hood-hashing)
            CHECK(row["a"] == std::to_string(data++));
            CHECK(row["b"] == std::to_string(data++));
            CHECK(row["c"] == std::to_string(data++));
        }
    }

    const auto cols = file.cols();
    CHECK(std::tie(cols[0], cols[1], cols[2]) == std::tie("a", "b", "c"));

    CHECK(file.shape() == std::pair<size_t, size_t>{2, 3});
}

#endif
