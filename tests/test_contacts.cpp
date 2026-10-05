#include <doctest/doctest.h>

#include <string>

#include "contacts.hpp"

using namespace map_project;

TEST_CASE("name of exactly 50 characters is accepted") {
    CHECK_FALSE(ValidateName(std::string(50, 'a')).has_value());
}

TEST_CASE("name of 51 characters is rejected") {
    CHECK(ValidateName(std::string(51, 'a')).has_value());
}

TEST_CASE("empty name is rejected") {
    CHECK(ValidateName("").has_value());
}

TEST_CASE("diacritics count as one character each") {
    std::string name;
    for (int i = 0; i < 50; ++i) {
        name += "\xC4\x83";  // "ă" encoded as two bytes in UTF-8
    }
    CHECK_FALSE(ValidateName(name).has_value());
}

TEST_CASE("email with one at sign and a dot after it is accepted") {
    CHECK_FALSE(ValidateEmail("ana@example.com").has_value());
}

TEST_CASE("email without at sign is rejected") {
    CHECK(ValidateEmail("ana.example.com").has_value());
}

TEST_CASE("email with two at signs is rejected") {
    CHECK(ValidateEmail("ana@@example.com").has_value());
}

TEST_CASE("email without text before the at sign is rejected") {
    CHECK(ValidateEmail("@example.com").has_value());
}

TEST_CASE("email with no dot after the at sign is rejected") {
    CHECK(ValidateEmail("ana@example").has_value());
    CHECK(ValidateEmail("a.b@example").has_value());
}

TEST_CASE("phone with exactly ten digits is accepted") {
    CHECK_FALSE(ValidatePhone("0722334455").has_value());
}

TEST_CASE("phone with a leading plus and ten digits is accepted") {
    CHECK_FALSE(ValidatePhone("+0722334455").has_value());
}

TEST_CASE("phone with nine or eleven digits is rejected") {
    CHECK(ValidatePhone("072233445").has_value());
    CHECK(ValidatePhone("07223344556").has_value());
}

TEST_CASE("phone with letters is rejected") {
    CHECK(ValidatePhone("07223344ab").has_value());
}

TEST_CASE("known categories are accepted") {
    CHECK_FALSE(ValidateCategory("family").has_value());
    CHECK_FALSE(ValidateCategory("friends").has_value());
    CHECK_FALSE(ValidateCategory("work").has_value());
    CHECK_FALSE(ValidateCategory("other").has_value());
}

TEST_CASE("unknown category is rejected") {
    CHECK(ValidateCategory("colleagues").has_value());
}