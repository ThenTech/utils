#include "test_settings.hpp"

#ifdef ENABLE_TESTS
#include "../utils_lib/external/doctest.hpp"

#include "../utils_lib/utils_ini.hpp"


TEST_CASE("Test utils::ini") {
    utils::ini::ConfigReader reader;

    reader.SetValue("   TEST    ", "een int", 6);
    reader.SetValue(" TEST ", "nog een int", -99);
    reader.SetValue("floats", "f1", 0.212);
    reader.SetValue("  floats\n", "pi", 3.141596);
    reader.SetValue<std::string>("str", "s1", "verder met dit");
    reader.SetValue<std::string>("str", "path", "./hier/volgt/een/file/kijk.txt");

    SUBCASE("Test utils::ini sections and keys are set") {
        REQUIRE(reader.SectionSize() == 3);
        REQUIRE(reader.hasSection("TEST"));
        REQUIRE(reader["TEST"]);
        REQUIRE(reader.hasSection("floats"));
        REQUIRE(reader.hasSection("str"));

        REQUIRE(reader.SectionKeySize("TEST") == 2);
        CHECK(reader.hasSectionKey("TEST", "een int"));
        CHECK(reader("TEST", "een int"));
        CHECK(reader.hasSectionKey("TEST", "nog een int"));
        REQUIRE(reader.SectionKeySize("floats") == 2);
        CHECK(reader.hasSectionKey("floats", "f1"));
        CHECK(reader.hasSectionKey("floats", "pi"));
        REQUIRE(reader.SectionKeySize("str") == 2);
        CHECK(reader.hasSectionKey("str", "s1"));
        CHECK(reader.hasSectionKey("str", "path"));

        CHECK(  6 == reader.GetValue<int>("TEST", "een int"));
        CHECK(-99 == reader.GetValue<int>("TEST", "nog een int"));
        CHECK(doctest::Approx(0.212)    == reader.GetValue<double>("floats", "f1"));
        CHECK(doctest::Approx(3.141596) == reader.GetValue<double>("floats", "pi"));
        CHECK("verder met dit" == reader.GetValue<std::string>("str", "s1"));
        CHECK("./hier/volgt/een/file/kijk.txt" == reader.GetValue<std::string>("str", "path"));
    }

    SUBCASE("Test utils::ini creation from stream") {
        std::stringstream streamed;
        streamed << reader;

        utils::ini::ConfigReader reader2(streamed);
        REQUIRE(reader2.hasSection("TEST"));
        REQUIRE(reader2.hasSection("floats"));
        REQUIRE(reader2.hasSection("str"));

        CHECK(reader2.hasSectionKey("TEST", "een int"));
        CHECK(reader2.hasSectionKey("TEST", "nog een int"));
        CHECK(reader2.hasSectionKey("floats", "f1"));
        CHECK(reader2.hasSectionKey("floats", "pi"));
        CHECK(reader2.hasSectionKey("str", "s1"));
        CHECK(reader2.hasSectionKey("str", "path"));

        reader2.ForEachSection([&](const std::string& s){
            CHECK(reader2.hasSection(s));
        });

        reader2.ForEachSectionKey("TEST", [&](const std::string& k, const utils::ini::ConfigReader<>::Value& v){
            CHECK(reader2.hasSectionKey("TEST", k));
            CHECK(std::holds_alternative<std::string>(v));
        });

        std::stringstream streamed2;
        streamed2 << reader2;
        CHECK(streamed.str() == streamed2.str());

        reader2.save("./tmp");
        utils::ini::ConfigReader reader3("./tmp");
        std::stringstream streamed3;
        streamed3 << reader3;
        CHECK(streamed.str() == streamed3.str());
        utils::io::fs::remove("./tmp");

        reader3.save();
        std::stringstream().swap(streamed3);
        auto fc = utils::io::file_to_string("./tmp");
        streamed3 << *fc;
        CHECK(streamed.str() == streamed3.str());
        utils::io::fs::remove("./tmp");
    }

    SUBCASE("Test utils::ini section") {
        reader.CreateSection("add_section");
        REQUIRE(reader.hasSection("add_section"));
        reader.RemoveSection("add_section");
        REQUIRE_FALSE(reader.hasSection("add_section"));
    }

    SUBCASE("Test utils::ini creation of key, and section that did not exist") {
        reader.CreateSectionKey("add_section_and_key", "sect_key");
        REQUIRE(reader.hasSection("add_section_and_key"));
        CHECK(reader.hasSectionKey("add_section_and_key", "sect_key"));
        reader.RemoveSectionKey("add_section_and_key", "sect_key");
        REQUIRE(reader.hasSection("add_section_and_key"));
        CHECK_FALSE(reader.hasSectionKey("add_section_and_key", "sect_key"));
    }

    SUBCASE("Test utils::ini creation of key, and section that did exist") {
        reader.CreateSectionKey("TEST", "temp");
        REQUIRE(reader.hasSection("TEST"));
        CHECK(reader.hasSectionKey("TEST", "een int"));
        CHECK(reader.hasSectionKey("TEST", "nog een int"));
        CHECK(reader.hasSectionKey("TEST", "temp"));
        reader.RemoveSectionKey("TEST", "temp");
        CHECK(reader.hasSectionKey("TEST", "een int"));
        CHECK(reader.hasSectionKey("TEST", "nog een int"));
        CHECK_FALSE(reader.hasSectionKey("TEST", "temp"));
    }

    SUBCASE("Test utils::ini value") {
        reader.CreateSectionKey("TEST", "temp");
        REQUIRE(reader.GetValue<int>("TEST", "temp") == 0);
        reader.SetValue("TEST", "temp", 889879);
        REQUIRE(reader.GetValue<int>("TEST", "temp") == 889879);
        reader.SetValue<std::string>("TEST", "temp", "Hello World!");
        REQUIRE(reader.GetValue<std::string>("TEST", "temp") == "Hello World!");

        reader.SetValue("FOO", "temp2", 0.2f);
        REQUIRE(reader.SectionKeySize("FOO") == 1);
        REQUIRE(reader.hasSection("FOO"));
        REQUIRE(reader.hasSectionKey("FOO", "temp2"));
        REQUIRE(doctest::Approx(0.2) == reader.GetValue<float>("FOO", "temp2"));
        REQUIRE("0.2" == reader.GetValue<std::string>("FOO", "temp2"));
        REQUIRE(0  == reader.GetValue<int>("FOO", "temp2"));
        REQUIRE(3  == reader.GetValue<int>("floats", "pi"));
    }

    SUBCASE("Test utils::ini boolean value") {
        reader.SetValue<std::string>("FLAGS", "true1", "true");
        reader.SetValue<std::string>("FLAGS", "true2", "True");
        reader.SetValue<std::string>("FLAGS", "true3", "YES");
        reader.SetValue<std::string>("FLAGS", "true4", "on");
        reader.SetValue<std::string>("FLAGS", "true5", "1");
        reader.SetValue("FLAGS", "true6", 1);
        reader.SetValue("FLAGS", "true7", true);
        reader.SetValue<std::string>("FLAGS", "false1", "false");
        reader.SetValue<std::string>("FLAGS", "false2", "False");
        reader.SetValue<std::string>("FLAGS", "false3", "NO");
        reader.SetValue<std::string>("FLAGS", "false4", "off");
        reader.SetValue<std::string>("FLAGS", "false5", "0");
        reader.SetValue("FLAGS", "false6", 0);
        reader.SetValue("FLAGS", "false7", false);
        reader.SetValue<std::string>("FLAGS", "false8", "maybe");
        CHECK(reader.GetValue<bool>("FLAGS", "true1"));
        CHECK(reader.GetValue<bool>("FLAGS", "true2"));
        CHECK(reader.GetValue<bool>("FLAGS", "true3"));
        CHECK(reader.GetValue<bool>("FLAGS", "true4"));
        CHECK(reader.GetValue<bool>("FLAGS", "true5"));
        CHECK(reader.GetValue<bool>("FLAGS", "true6"));
        CHECK(reader.GetValue<bool>("FLAGS", "true7"));
        CHECK_FALSE(reader.GetValue<bool>("FLAGS", "false1"));
        CHECK_FALSE(reader.GetValue<bool>("FLAGS", "false2"));
        CHECK_FALSE(reader.GetValue<bool>("FLAGS", "false3"));
        CHECK_FALSE(reader.GetValue<bool>("FLAGS", "false4"));
        CHECK_FALSE(reader.GetValue<bool>("FLAGS", "false5"));
        CHECK_FALSE(reader.GetValue<bool>("FLAGS", "false6"));
        CHECK_FALSE(reader.GetValue<bool>("FLAGS", "false7"));
        CHECK_FALSE(reader.GetValue<bool>("FLAGS", "false8"));
    }

    SUBCASE("Test utils::ini remove empty section") {
        reader.SetValue("FOO", "temp2", 0.2f);
        reader.RemoveSectionKey("FOO", "temp2", true);
        CHECK_FALSE(reader.hasSection("FOO"));
    }

    SUBCASE("Test utils::ini get non existing value") {
        REQUIRE_THROWS_AS(reader.GetValue<int>("TEST", "does not exist"), utils::exceptions::KeyDoesNotExistException);
        REQUIRE_THROWS_AS(reader.GetValue<int>("does not exist", "pi"), utils::exceptions::KeyDoesNotExistException);
    }

    SUBCASE("Test utils::ini invalid file input/output") {
        // FIXME Why not throwing utils::exceptions::FileReadException ?
        REQUIRE_THROWS_AS(utils::ini::ConfigReader(std::string("")), std::exception);

        REQUIRE_THROWS_AS(reader.save(""), utils::exceptions::FileWriteException);

        // FIXME Why not throwing utils::exceptions::FileReadException ?
        REQUIRE_NOTHROW(reader.save("<\\:?//>"));
    }
}

#endif
