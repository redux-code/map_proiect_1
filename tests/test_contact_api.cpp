#include <doctest/doctest.h>

#include <string>

#include "contact_api.hpp"

using namespace map_project;

namespace {
const std::string kAna =
    R"({"name":"Ana Pop","email":"ana@example.com","phone":"0722334455","category":"work"})";
}

TEST_CASE("creating a valid contact returns 201 with the new id") {
    ContactBook book;
    const ApiResponse response = CreateContact(book, kAna);
    CHECK(response.status == 201);
    CHECK(response.body["id"] == 1);
    CHECK(response.body["name"] == "Ana Pop");
    CHECK(response.body["category"] == "work");
}

TEST_CASE("same email written differently returns 409 conflict") {
    ContactBook book;
    CreateContact(book, kAna);
    const ApiResponse response = CreateContact(
        book,
        R"({"name":"Ana P.","email":"Ana@Example.COM","phone":"0733445566","category":"friends"})");
    CHECK(response.status == 409);
    CHECK(response.body["error"] == "conflict");
}

TEST_CASE("invalid field value returns 400 validation") {
    ContactBook book;
    const ApiResponse response = CreateContact(
        book, R"({"name":"Ana","email":"ana@example.com","phone":"123","category":"work"})");
    CHECK(response.status == 400);
    CHECK(response.body["error"] == "validation");
}

TEST_CASE("missing field returns 400 validation") {
    ContactBook book;
    const ApiResponse response =
        CreateContact(book, R"({"name":"Ana","email":"ana@example.com","category":"work"})");
    CHECK(response.status == 400);
    CHECK(response.body["error"] == "validation");
}

TEST_CASE("field of the wrong type returns 400 validation") {
    ContactBook book;
    const ApiResponse response = CreateContact(
        book, R"({"name":"Ana","email":"ana@example.com","phone":722334455,"category":"work"})");
    CHECK(response.status == 400);
}

TEST_CASE("body that is not JSON returns 400 validation") {
    ContactBook book;
    CHECK(CreateContact(book, "not json").status == 400);
    CHECK(CreateContact(book, "[1,2,3]").status == 400);
}

TEST_CASE("an existing contact is returned with the email as written") {
    ContactBook book;
    CreateContact(
        book,
        R"({"name":"Ana","email":"Ana@Example.COM","phone":"0722334455","category":"work"})");
    const ApiResponse response = GetContact(book, "1");
    CHECK(response.status == 200);
    CHECK(response.body["email"] == "Ana@Example.COM");
}

TEST_CASE("unknown id returns 404 not_found") {
    ContactBook book;
    const ApiResponse response = GetContact(book, "999");
    CHECK(response.status == 404);
    CHECK(response.body["error"] == "not_found");
}

TEST_CASE("id that is not a positive number cannot be found") {
    CHECK_FALSE(ParseId("abc").has_value());
    CHECK_FALSE(ParseId("0").has_value());
    CHECK_FALSE(ParseId("99999999999999999999").has_value());
    CHECK(ParseId("42") == 42);
}