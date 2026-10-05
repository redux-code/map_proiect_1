#include <doctest/doctest.h>

#include "contact.hpp"

using namespace map_project;

TEST_CASE("contact returns the values it was created with") {
    const Contact contact(1, "Ana Pop", "ana@example.com", "0722334455", "work");
    CHECK(contact.Id() == 1);
    CHECK(contact.Name() == "Ana Pop");
    CHECK(contact.Email() == "ana@example.com");
    CHECK(contact.Phone() == "0722334455");
    CHECK(contact.Category() == "work");
}

TEST_CASE("contact keeps the email exactly as written") {
    const Contact contact(2, "Ana", "Ana@Example.COM", "0722334455", "other");
    CHECK(contact.Email() == "Ana@Example.COM");
}