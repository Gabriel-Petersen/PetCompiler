module;

#include <string>
#include <type_traits>

export module ast.evaluation;

import error;

export enum class EvaluationType
{
    INT,
    FLOAT,
    BOOL
};

export struct Evaluation
{
    EvaluationType type;

    union Data
    {
        double dVal;
        long long iVal;
        bool bVal;

        constexpr Data() : iVal(0) { }
    } data;

    Evaluation() : type(EvaluationType::INT) { data.iVal = 0; }
    Evaluation(long long value) : type(EvaluationType::INT) { data.iVal = value; }
    Evaluation(double value) : type(EvaluationType::FLOAT) { data.dVal = value; }
    Evaluation(bool value) : type(EvaluationType::BOOL) { data.bVal = value; }

    [[nodiscard]] bool isTrue() const
    {
        switch (type)
        {
        case EvaluationType::BOOL:
            return data.bVal;
        case EvaluationType::INT:
            return data.iVal != 0;
        case EvaluationType::FLOAT:
            return data.dVal != 0.0;
        }

        errorR::report(
            "Unexpected EvaluationType: " +
            std::to_string(static_cast<int>(type))
        );

        return false;
    }

    template<typename T>
    [[nodiscard]] T getNumber() const
    {
        static_assert(
            std::is_arithmetic_v<T>,
            "Evaluation::getNumber<T>() requires an arithmetic type"
        );

        switch (type)
        {
        case EvaluationType::INT:   return static_cast<T>(data.iVal);
        case EvaluationType::FLOAT: return static_cast<T>(data.dVal);
        case EvaluationType::BOOL:  return static_cast<T>(data.bVal);
        }

        error::report(
            "Unexpected EvaluationType: " +
            std::to_string(static_cast<int>(type))
        );

        return T{};
    }
};