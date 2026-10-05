// Validation rules for the contact book (theme 1).
// Pure functions: no HTTP and no global state, so they are easy to unit test.
#pragma once

#include <algorithm>
#include <array>
#include <cctype>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>

namespace map_project {

inline constexpr std::size_t kMaxNameLength = 50;
inline constexpr std::size_t kPhoneDigits = 10;
inline constexpr std::array<std::string_view, 4> kCategories = {
    "family", "friends", "work", "other"};

/// Every validator returns std::nullopt when the value is valid,
/// otherwise the message that goes into the "message" field of the error body.
using ValidationError = std::optional<std::string>;

/// Number of characters (not bytes) in a UTF-8 string.
/// std::string::size() counts bytes, so "ă" would count as 2.
inline std::size_t CharacterCount(std::string_view text) {
    std::size_t count = 0;
    for (const unsigned char byte : text) {
        // The continuation bytes of a multi-byte character look like 10xxxxxx.
        if ((byte & 0xC0) != 0x80) {
            ++count;
        }
    }
    return count;
}

inline ValidationError ValidateName(std::string_view name) {
    const std::size_t length = CharacterCount(name);
    if (length < 1 || length > kMaxNameLength) {
        return "name must have between 1 and " + std::to_string(kMaxNameLength) +
               " characters";
    }
    return std::nullopt;
}

inline ValidationError ValidateEmail(std::string_view email) {
    const std::size_t at = email.find('@');
    // Exactly one '@': it exists, and there is no second one after it.
    const bool exactly_one_at =
        at != std::string_view::npos && email.find('@', at + 1) == std::string_view::npos;
    if (!exactly_one_at) {
        return "email must contain exactly one '@'";
    }
    if (at == 0) {
        return "email must have text before '@'";
    }
    if (email.substr(at + 1).find('.') == std::string_view::npos) {
        return "email must contain a dot after '@'";
    }
    return std::nullopt;
}

inline ValidationError ValidatePhone(std::string_view phone) {
    // An optional leading '+' is allowed and is not counted as a digit.
    if (!phone.empty() && phone.front() == '+') {
        phone.remove_prefix(1);
    }
    const bool all_digits = std::all_of(phone.begin(), phone.end(), [](unsigned char c) {
        return std::isdigit(c) != 0;
    });
    if (phone.size() != kPhoneDigits || !all_digits) {
        return "phone must have exactly 10 digits, optionally preceded by '+'";
    }
    return std::nullopt;
}

inline ValidationError ValidateCategory(std::string_view category) {
    if (std::find(kCategories.begin(), kCategories.end(), category) == kCategories.end()) {
        return "category must be one of: family, friends, work, other";
    }
    return std::nullopt;
}

}  // namespace map_project