#ifndef JSON_PRINTER_HPP
#define JSON_PRINTER_HPP

#include <variant>
#include <vector>
#include <iostream>

void PrintJsonString(std::ostream &out, std::string_view str);

namespace JSON_ContextManagement {
    enum ObjectIsWaitingFor {
        KEY,
        VALUE
    };

    template<typename ParentType, ObjectIsWaitingFor objectIsWaitingFor = ObjectIsWaitingFor::KEY>
    class ObjectContext;

    template<typename ParentType = void>
    class ArrayContext;

    template<typename ParentType>
    struct ParentNodeHolder {
        ParentType *parent;
    };

    template<>
    struct ParentNodeHolder<void> {
    };

    class ObjectBase {
        // Plug used to obtain the covariance
        // between Object and ObjectContext<Parent, ObjectIsWaitingFor::VALUE>
    };

    class Object;

    class Array {
    public:
        using Union = std::variant<
                int64_t,
                std::string,
                bool,
                std::monostate,
                std::unique_ptr<Object>,
                std::unique_ptr<Array>
        >;

        virtual ~Array() = default;

        void PrintToStream(std::ostream &stream) const;

        static inline void PrintToStream(const Union &value, std::ostream &stream);

    protected:
        Array() = default;

        std::vector<Union> values;
    };

    class Object {
    public:
        virtual ~Object() = default;

        virtual ObjectBase &Key(std::string_view key) = 0;

        virtual void PrintToStream(std::ostream &stream) const = 0;

        void PrintToStream(std::ostream &stream, const std::vector<Array::Union> &values) const;

    protected:
        Object() = default;

        std::vector<std::string> keys;
    };

    template<template<typename, ObjectIsWaitingFor ...> typename MainType, typename ParentType, ObjectIsWaitingFor ...wt>
    class ArrayInterface : public ParentNodeHolder<ParentType>, public Array {
        using T = MainType<ParentType, wt...>;
    public:
        T &Number(int64_t number) {
            values.emplace_back(number);
            return CastToParent();
        }

        T &String(std::string_view string) {
            values.emplace_back(std::string(string));
            return CastToParent();
        }

        T &Boolean(bool value) {
            values.emplace_back(value);
            return CastToParent();
        }

        T &Null() {
            values.emplace_back(std::monostate());
            return CastToParent();
        }

        ArrayContext<T> &BeginArray() {
            auto new_context = std::make_unique<ArrayContext<T>>(static_cast<T *>(this));
            const auto new_ptr = new_context.get();
            values.emplace_back(std::move(new_context));
            return *new_ptr;
        }

        ObjectContext<T> &BeginObject() {
            auto new_context = std::make_unique<ObjectContext<T>>(static_cast<T *>(this));
            const auto new_ptr = new_context.get();
            values.emplace_back(std::move(new_context));
            return *new_ptr;
        };

    protected:
        inline T &CastToParent() {
            return static_cast<T &>(*this);
        }

        ArrayInterface() = default;
    };

    template<>
    class ArrayContext<void> : public ArrayInterface<ArrayContext, void> {
    public:
        explicit ArrayContext(std::ostream &stream) : stream(stream) {
            stream << std::boolalpha;
        }

        static inline void EndArray() {
        }

        ~ArrayContext() final {
            PrintToStream(stream);
        }

    private:
        std::ostream &stream;
    };

    template<typename ParentType>
    class ArrayContext : public ArrayInterface<ArrayContext, ParentType> {
    public:
        explicit ArrayContext(ParentType *parent) {
            this->parent = parent;
        }

        ParentType &EndArray() const {
            return *(this->parent);
        }

        ~ArrayContext() final = default;
    };

    template<typename ParentType>
    class ObjectContext<ParentType, ObjectIsWaitingFor::VALUE> :
            public ArrayInterface<ObjectContext, ParentType, ObjectIsWaitingFor::KEY>, public ObjectBase {
    protected:
        ObjectContext() = default;
    };

    template<typename ParentType>
    class ObjectContext<ParentType, ObjectIsWaitingFor::KEY> :
            protected ObjectContext<ParentType, ObjectIsWaitingFor::VALUE>, public Object {
    public:
        ObjectContext() = default;

        explicit ObjectContext(ParentType *parent) {
            this->parent = parent;
        }

        ObjectContext<ParentType, ObjectIsWaitingFor::VALUE> &Key(std::string_view key) final {
            keys.emplace_back(key);
            return static_cast<ObjectContext<ParentType, ObjectIsWaitingFor::VALUE> &>(*this);
        }

        ParentType &EndObject() const {
            return *(this->parent);
        }

        ~ObjectContext() final = default;

    protected:
        void PrintToStream(std::ostream &stream) const final {
            Object::PrintToStream(stream, this->values);
        }
    };

    using ValueWaitingTopObjectContext = ObjectContext<void, ObjectIsWaitingFor::VALUE>;

    template<>
    class ObjectContext<void, ObjectIsWaitingFor::KEY> :
            protected ValueWaitingTopObjectContext, public Object {
    public:
        explicit ObjectContext(std::ostream &stream) : stream(stream) {
            stream << std::boolalpha;
        }

        ValueWaitingTopObjectContext &Key(std::string_view key) final {
            keys.emplace_back(key);
            return static_cast<ValueWaitingTopObjectContext &>(*this);
        }

        static inline void EndObject() {
        }

        ~ObjectContext() final {
            PrintToStream(stream);
        }

        template<template<typename, ObjectIsWaitingFor ...> typename, typename, ObjectIsWaitingFor ...>
        friend
        class ArrayInterface;

    private:
        void PrintToStream(std::ostream &ostream) const final {
            Object::PrintToStream(ostream, values);
        }

        std::ostream &stream;
    };
}

using ArrayContext = JSON_ContextManagement::ArrayContext<void>;

using ObjectContext = JSON_ContextManagement::ObjectContext<void, JSON_ContextManagement::ObjectIsWaitingFor::KEY>;

ArrayContext PrintJsonArray(std::ostream &out);

ObjectContext PrintJsonObject(std::ostream &out);

#endif //JSON_PRINTER_HPP