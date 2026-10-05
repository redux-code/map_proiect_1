// A single entry of the contact book.
// It only holds data: validation is done before a Contact is created,
// and uniqueness rules belong to the container that owns the contacts.
#pragma once

#include <string>
#include <utility>

namespace map_project {

class Contact {
public:
    Contact(int id, std::string name, std::string email, std::string phone,
            std::string category)
        : id_(id),
          name_(std::move(name)),
          email_(std::move(email)),
          phone_(std::move(phone)),
          category_(std::move(category)) {}

    int Id() const { return id_; }
    const std::string& Name() const { return name_; }
    // Returned exactly as the user wrote it; comparisons use a normalized copy.
    const std::string& Email() const { return email_; }
    const std::string& Phone() const { return phone_; }
    const std::string& Category() const { return category_; }

private:
    int id_;
    std::string name_;
    std::string email_;
    std::string phone_;
    std::string category_;
};

}  // namespace map_project