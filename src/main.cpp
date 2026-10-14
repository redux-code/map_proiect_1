// Wires the HTTP routes of the common contract and of theme 1 (contact book).
// The rules live in the headers; this file only moves data between the
// HTTP request/response and the handlers in contact_api.hpp.

#include <chrono>
#include <iostream>

#include <httplib.h>

#include "app.hpp"
#include "contact_api.hpp"
#include "contact_book.hpp"

namespace {

constexpr const char* kJson = "application/json; charset=utf-8";

void Send(httplib::Response& response, const map_project::ApiResponse& result) {
    response.status = result.status;
    if (result.status != 204) {
        response.set_content(result.body.dump(), kJson);
    }
}

}  // namespace

int main() {
    using namespace map_project;

    const auto started_at = std::chrono::steady_clock::now();
    Store store;
    ContactBook book;
    httplib::Server server;

    server.Get("/health", [&](const httplib::Request&, httplib::Response& response) {
        response.set_content(HealthBody(started_at).dump(), kJson);
    });

    server.Get("/version", [](const httplib::Request&, httplib::Response& response) {
        response.set_content(VersionBody().dump(), kJson);
    });

    server.Post("/reset", [&](const httplib::Request&, httplib::Response& response) {
        store.Reset();
        book.Reset();  // without this, contacts would survive /reset
        response.status = 204;
    });

    server.Get("/", [](const httplib::Request&, httplib::Response& response) {
        response.set_content(HomePage(), "text/html; charset=utf-8");
    });

    server.Post("/contacts", [&](const httplib::Request& request, httplib::Response& response) {
        Send(response, CreateContact(book, request.body));
    });

    server.Get(R"(/contacts/(\d+))",
               [&](const httplib::Request& request, httplib::Response& response) {
                   Send(response, GetContact(book, request.matches[1].str()));
               });

    // httplib calls this for every 4xx/5xx, including the ones our routes return.
    // Only fill in a body when the route did not write one (unknown path).
    server.set_error_handler([](const httplib::Request&, httplib::Response& response) {
        if (response.status == 404 && response.body.empty()) {
            response.set_content(ErrorBody("not_found", "route does not exist").dump(), kJson);
        }
    });

    std::cout << kAppName << " " << kAppVersion << " starting on port 8080" << std::endl;
    if (!server.listen("0.0.0.0", 8080)) {
        std::cerr << "failed to bind port 8080" << std::endl;
        return 1;
    }
    return 0;
}