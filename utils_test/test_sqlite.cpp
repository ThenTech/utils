#include "test_settings.hpp"

#ifdef ENABLE_TESTS
#include "../utils_lib/external/doctest.hpp"

#include "../utils_lib/utils_sqlite.hpp"
#include "../utils_lib/utils_json.hpp"
#include "../utils_lib/utils_io.hpp"
#include "../utils_lib/utils_algorithm.hpp"
#include "../utils_lib/utils_random.hpp"

struct Entry {
    int id;
    std::string key;
    std::string val;
};

TEST_CASE("Test utils::sqlite") {
    UTILS_PROFILE_SCOPE("utils::sqlite");
    static const std::string dbfile = "test_db.sqlite";

    // Delete to be sure
    bool db_file_exists = utils::io::fs::exists(dbfile);
    if (db_file_exists) {
        utils::io::fs::remove(dbfile);
    }

    auto storage = utils::sqlite::make_storage(std::string(dbfile),
        utils::sqlite::make_table("items",
            utils::sqlite::make_column("id", &Entry::id,
                                       utils::sqlite::autoincrement(),
                                       utils::sqlite::primary_key()),
            utils::sqlite::make_column("key", &Entry::key),
            utils::sqlite::make_column("val", &Entry::val)
        )
    );

    // Check if saved
    storage.sync_schema();
    db_file_exists = utils::io::fs::exists(dbfile);
    REQUIRE(db_file_exists);

    REQUIRE(utils::algorithm::contains(storage.table_names(), "items"));

    utils::json out;
    out["test_1"] = "some value";
    out["test_2"] = "some other value 3.14/*-?";

    storage.transaction([&] () mutable {
        for (auto& [key, value] : out.items()) {
            storage.insert(std::move(Entry{-1, key, value.dump()}));
        }
        return true;
    });

    CHECK(storage.count<Entry>() == 2);
    CHECK(storage.group_concat(&Entry::key) == "test_1,test_2");

    // Cleanup
    if (db_file_exists) {
        utils::io::fs::remove(dbfile);
    }
}

#endif
