#include "json_printer.hpp"
#include "test_runner.hpp"

#include <sstream>

namespace JSON_printer {
    void TestArray() {
        std::ostringstream output;

        {
            auto json = PrintJsonArray(output);
            json
                    .Number(5)
                    .Number(6)
                    .BeginArray()
                    .Number(7)
                    .EndArray()
                    .Number(8)
                    .String("bingo!");
        }

        ASSERT_EQUAL(output.str(), R"([5,6,[7],8,"bingo!"])");
    }

    void TestObject() {
        std::ostringstream output;

        {
            auto json = PrintJsonObject(output);
            json
                    .Key("id1").Number(1234)
                    .Key("id2").Boolean(false)
                    .Key("").Null()
                    .Key("\"").String("\\");
        }

        ASSERT_EQUAL(output.str(), R"({"id1":1234,"id2":false,"":null,"\"":"\\"})");
    }

    void TestAutoClose() {
        std::ostringstream output;

        {
            auto json = PrintJsonArray(output);
            json.BeginArray().BeginObject();
        }

        ASSERT_EQUAL(output.str(), R"([[{}]])");
    }
}

void TestJSON_Printer() {
    JSON_printer::TestArray();
    JSON_printer::TestObject();
    JSON_printer::TestAutoClose();
}

int main() {
    TestJSON_Printer();
    std::cout << "All tests passed!\n";
    return 0;
}