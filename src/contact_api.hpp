// HTTP-facing logic for the contact routes, without the HTTP server itself.
// Each handler takes the raw request data (body, path id) and returns the status
// code and the JSON body. main.cpp only copies these into the real response,
// so every rule here can be unit tested without opening a socket.
#pragma once

#include <charconv>
#include <optional>
#include <string>
#include <string_view>

#include <nlohmann/json.hpp>

#include "app.hpp"
#include "contact.hpp"
#include "contact_book.hpp"

namespace map_project {

struct ApiResponse {
    int status;
    nlohmann::json body;  // ignored for 204 responses
};

inline nlohmann::json ContactToJson(const Contact& contact) {
    return {{"id", contact.Id()},
            {"name", contact.Name()},
            {"email", contact.Email()},
            {"phone", contact.Phone()},
            {"category", contact.Category()}};
}

// Turns the {id} part of the path into a number. Anything that is not a
// positive int (letters, overflow) simply cannot match an existing contact.
inline std::optional<int> ParseId(std::string_view text) {
    int value = 0;
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value);
    if (error != std::errc() || end != text.data() + text.size() || value < 1) {
        return std::nullopt;
    }
    return value;
}

// POST /contacts
inline ApiResponse CreateContact(ContactBook& book, const std::string& request_body) {
    // The third argument turns parse errors into a "discarded" value instead of
    // an exception, so a broken body becomes a 400 rather than crashing the request.
    const nlohmann::json input = nlohmann::json::parse(request_body, nullptr, false);
    if (input.is_discarded() || !input.is_object()) {
        return {400, ErrorBody("validation", "body must be a JSON object")};
    }
    for (const char* field : {"name", "email", "phone", "category"}) {
        if (!input.contains(field) || !input[field].is_string()) {
            return {400, ErrorBody("validation",
                                   std::string(field) + " is required and must be a string")};
        }
    }

    const AddResult result =
        book.Add(input["name"].get<std::string>(), input["email"].get<std::string>(),
                 input["phone"].get<std::string>(), input["category"].get<std::string>());
    switch (result.status) {
        case AddStatus::kCreated:
            return {201, ContactToJson(*result.contact)};
        case AddStatus::kConflict:
            return {409, ErrorBody("conflict", result.message)};
        case AddStatus::kInvalid:
            break;
    }
    return {400, ErrorBody("validation", result.message)};
}

// GET /contacts/{id}
inline ApiResponse GetContact(const ContactBook& book, std::string_view id_text) {
    const std::optional<int> id = ParseId(id_text);
    const std::optional<Contact> contact = id ? book.Find(*id) : std::nullopt;
    if (!contact) {
        return {404, ErrorBody("not_found", "contact does not exist")};
    }
    return {200, ContactToJson(*contact)};
}

}  // namespace map_project