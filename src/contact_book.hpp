// The contact book: owns all contacts and enforces the rules that depend on
// the other contacts (unique email, server-assigned ids, filtering, sorting).
#pragma once

#include <algorithm>
#include <cctype>
#include <map>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

#include "contact.hpp"
#include "contacts.hpp"

namespace map_project {

enum class AddStatus { kCreated, kInvalid, kConflict };

// What Add() reports back: the outcome, an error message (when it failed)
// and the created contact (when it succeeded).
struct AddResult {
    AddStatus status;
    std::string message;
    std::optional<Contact> contact;
};

// Lowercase copy of a string. Used only for comparisons: the original
// text is what gets stored and returned to the user.
inline std::string ToLower(std::string text) {
    std::transform(text.begin(), text.end(), text.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return text;
}

class ContactBook {
public:
    AddResult Add(const std::string& name, const std::string& email,
                  const std::string& phone, const std::string& category) {
        // Validation does not touch shared data, so it runs before the lock.
        for (const ValidationError& error :
             {ValidateName(name), ValidateEmail(email), ValidatePhone(phone),
              ValidateCategory(category)}) {
            if (error) {
                return {AddStatus::kInvalid, *error, std::nullopt};
            }
        }

        // From here on, checking the email and inserting must happen under the
        // same lock, otherwise two requests could both pass the check.
        std::lock_guard<std::mutex> lock(mutex_);
        const std::string normalized = ToLower(email);
        const bool duplicate = std::any_of(
            contacts_.begin(), contacts_.end(),
            [&](const Contact& c) { return ToLower(c.Email()) == normalized; });
        if (duplicate) {
            return {AddStatus::kConflict, "a contact with this email already exists",
                    std::nullopt};
        }
        contacts_.emplace_back(next_id_++, name, email, phone, category);
        return {AddStatus::kCreated, "", contacts_.back()};
    }

    // Returns a copy, so the caller never holds a reference into the vector
    // while another request modifies it.
    std::optional<Contact> Find(int id) const {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const Contact& contact : contacts_) {
            if (contact.Id() == id) {
                return contact;
            }
        }
        return std::nullopt;
    }

    bool Remove(int id) {
        std::lock_guard<std::mutex> lock(mutex_);
        const auto it = std::remove_if(contacts_.begin(), contacts_.end(),
                                       [id](const Contact& c) { return c.Id() == id; });
        if (it == contacts_.end()) {
            return false;
        }
        contacts_.erase(it, contacts_.end());
        return true;
    }

    // Both filters are optional; when both are given, both apply.
    // Sorted by name ascending, ties broken by id ascending.
    std::vector<Contact> List(const std::optional<std::string>& category,
                              const std::optional<std::string>& query) const {
        std::vector<Contact> result;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            const std::string needle = query ? ToLower(*query) : "";
            for (const Contact& contact : contacts_) {
                if (category && contact.Category() != *category) {
                    continue;
                }
                if (query && ToLower(contact.Name()).find(needle) == std::string::npos) {
                    continue;
                }
                result.push_back(contact);
            }
        }
        std::sort(result.begin(), result.end(), [](const Contact& a, const Contact& b) {
            if (a.Name() != b.Name()) {
                return a.Name() < b.Name();
            }
            return a.Id() < b.Id();
        });
        return result;
    }

    std::size_t Total() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return contacts_.size();
    }

    std::map<std::string, std::size_t> CountByCategory() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::map<std::string, std::size_t> counts;
        for (const Contact& contact : contacts_) {
            ++counts[contact.Category()];
        }
        return counts;
    }

    // Empties the book and restarts the ids from 1, as /reset requires.
    void Reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        contacts_.clear();
        next_id_ = 1;
    }

private:
    mutable std::mutex mutex_;  // mutable so const methods can lock it
    std::vector<Contact> contacts_;
    int next_id_ = 1;
};

}  // namespace map_project