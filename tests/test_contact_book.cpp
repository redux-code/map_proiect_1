#include <doctest/doctest.h>

#include <optional>
#include <string>

#include "contact_book.hpp"

using namespace map_project;

TEST_CASE("first contact gets id 1 and the next one id 2") {
    ContactBook book;
    CHECK(book.Add("Ana", "ana@example.com", "0722334455", "work").contact->Id() == 1);
    CHECK(book.Add("Ion", "ion@example.com", "0722334456", "work").contact->Id() == 2);
}

TEST_CASE("same email in different letter case is a conflict") {
    ContactBook book;
    book.Add("Ana Pop", "ana@example.com", "0722334455", "work");
    const AddResult second = book.Add("Ana P.", "Ana@Example.COM", "0733445566", "friends");
    CHECK(second.status == AddStatus::kConflict);
    CHECK(book.Total() == 1);
}

TEST_CASE("invalid data is rejected and does not use up an id") {
    ContactBook book;
    CHECK(book.Add("", "ana@example.com", "0722334455", "work").status == AddStatus::kInvalid);
    CHECK(book.Add("Ana", "ana@example.com", "0722334455", "work").contact->Id() == 1);
}

TEST_CASE("stored email keeps the letter case written by the user") {
    ContactBook book;
    book.Add("Ana", "Ana@Example.COM", "0722334455", "work");
    CHECK(book.Find(1)->Email() == "Ana@Example.COM");
}

TEST_CASE("find returns nothing for an unknown id") {
    ContactBook book;
    CHECK_FALSE(book.Find(999).has_value());
}

TEST_CASE("remove deletes once, then reports the contact as missing") {
    ContactBook book;
    book.Add("Ana", "ana@example.com", "0722334455", "work");
    CHECK(book.Remove(1));
    CHECK_FALSE(book.Remove(1));
    CHECK(book.Total() == 0);
}

TEST_CASE("ids are not reused after a delete") {
    ContactBook book;
    book.Add("Ana", "ana@example.com", "0722334455", "work");
    book.Remove(1);
    CHECK(book.Add("Ion", "ion@example.com", "0722334456", "work").contact->Id() == 2);
}

TEST_CASE("list is sorted by name, ties broken by id") {
    ContactBook book;
    book.Add("Zed", "z@example.com", "0722334455", "work");
    book.Add("Ana", "a1@example.com", "0722334456", "work");
    book.Add("Ana", "a2@example.com", "0722334457", "work");
    const auto contacts = book.List(std::nullopt, std::nullopt);
    REQUIRE(contacts.size() == 3);
    CHECK(contacts[0].Id() == 2);
    CHECK(contacts[1].Id() == 3);
    CHECK(contacts[2].Name() == "Zed");
}

TEST_CASE("category and search filters are both applied") {
    ContactBook book;
    book.Add("Ana Pop", "ana@example.com", "0722334455", "work");
    book.Add("Ana Dan", "dan@example.com", "0722334456", "friends");
    book.Add("Ion", "ion@example.com", "0722334457", "work");
    const auto contacts = book.List(std::string("work"), std::string("ANA"));
    REQUIRE(contacts.size() == 1);
    CHECK(contacts[0].Id() == 1);
}

TEST_CASE("search on an empty book returns an empty list") {
    ContactBook book;
    CHECK(book.List(std::nullopt, std::string("ana")).empty());
}

TEST_CASE("counts are grouped by category") {
    ContactBook book;
    book.Add("Ana", "ana@example.com", "0722334455", "work");
    book.Add("Ion", "ion@example.com", "0722334456", "work");
    book.Add("Dan", "dan@example.com", "0722334457", "other");
    const auto counts = book.CountByCategory();
    CHECK(counts.at("work") == 2);
    CHECK(counts.at("other") == 1);
}

TEST_CASE("reset empties the book and restarts ids from 1") {
    ContactBook book;
    book.Add("Ana", "ana@example.com", "0722334455", "work");
    book.Reset();
    CHECK(book.Total() == 0);
    CHECK(book.Add("Ion", "ion@example.com", "0722334456", "work").contact->Id() == 1);
}