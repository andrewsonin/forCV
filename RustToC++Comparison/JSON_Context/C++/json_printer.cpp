#include "json_printer.hpp"
#include "json.hpp"
#include "visitor.hpp"
#include "type_checking.hpp"

#define JSON_NULL "null"

void PrintJsonString(std::ostream &out, std::string_view str) {
    out << nlohmann::json(str);
}

ArrayContext PrintJsonArray(std::ostream &out) {
    return ArrayContext(out);
}

ObjectContext PrintJsonObject(std::ostream &out) {
    return ObjectContext(out);
}

void JSON_ContextManagement::Array::PrintToStream(std::ostream &stream) const {
    stream << '[';
    if (!values.empty()) {
        auto values_it = values.begin();
        const auto values_last_it = std::prev(values.end());
        if (values_it != values_last_it) {
            do {
                PrintToStream(*values_it, stream);
                stream << ',';
            } while (++values_it != values_last_it);
        }
        PrintToStream(*values_it, stream);
    }
    stream << ']';
}

void
JSON_ContextManagement::Array::PrintToStream(
        const JSON_ContextManagement::Array::Union &value, std::ostream &stream) {
    std::visit(
            make_visitor(
                    [&](const auto &v) {
                        if constexpr (IsSmartPointer<decltype(v)>::value) {
                            v->PrintToStream(stream);
                        } else {
                            stream << v;
                        }
                    },
                    [&](const std::string &str) {
                        PrintJsonString(stream, str);
                    },
                    [&](const std::monostate &) {
                        stream << JSON_NULL;
                    }
            ),
            value
    );
}

void
JSON_ContextManagement::Object::PrintToStream(
        std::ostream &stream, const std::vector<Array::Union> &values) const {
    stream << '{';
    if (!values.empty()) {
        auto values_it = values.begin();
        auto keys_it = keys.begin();
        if (const auto keys_last_it = std::prev(keys.end()); keys_it != keys_last_it) {
            do {
                Array::PrintToStream(*keys_it, stream);
                stream << ':';
                Array::PrintToStream(*values_it++, stream);
                stream << ',';
            } while (++keys_it != keys_last_it);
        }
        Array::PrintToStream(*keys_it, stream);
        stream << ':';
        if (keys.size() == values.size()) {
            Array::PrintToStream(*values_it, stream);
        } else {
            stream << JSON_NULL;
        }
    }
    stream << '}';
}
