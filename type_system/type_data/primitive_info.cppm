module;

#include <string>
#include <utility>
#include <climits>

export module types.data.primitive;

import error;
import types.data;
import types.info;
import types.structure;

export class PrimitiveInfo : public TypeData
{
public:
    const PrimitiveKind kind;

    PrimitiveInfo(TypeID id, PrimitiveKind primitiveKind, std::string name) : 
        TypeData(id, TypeCategory::PRIMITIVE, std::move(name), ""), kind(primitiveKind) { 
            setEnabled(true);
        }

    std::size_t getSizeInBytes() const override
    {
        if (!isEnabled()) {
            error::report("Internal compilation error: Querying bytesize of type not enabled");
            return 0;
        }

        switch (kind)
        {
        case PrimitiveKind::VOID:
            return 0;
        case PrimitiveKind::BYTE:
        case PrimitiveKind::CHAR:
        case PrimitiveKind::BOOL:
            return 1;
        case PrimitiveKind::SMALL:
        case PrimitiveKind::USMALL:
            return 2;
        case PrimitiveKind::INT:
        case PrimitiveKind::UINT:
        case PrimitiveKind::FLOAT:
            return 4;
        case PrimitiveKind::DOUBLE:
        case PrimitiveKind::LONG:
        case PrimitiveKind::ULONG:
            return 8;
        default:
            error::report("Unexpected type");
            return 0;
        }
    }

    [[nodiscard]] bool isFloat()   const { return kind == PrimitiveKind::FLOAT || kind == PrimitiveKind::DOUBLE; }
    [[nodiscard]] bool isInteger() const { return !isFloat() && kind != PrimitiveKind::BOOL && kind != PrimitiveKind::VOID; }
    [[nodiscard]] bool isBool()    const { return kind == PrimitiveKind::BOOL; }
    [[nodiscard]] bool isVoid()    const { return kind == PrimitiveKind::VOID; }
    
    [[nodiscard]] bool isUnsigned() const {
        return kind == PrimitiveKind::CHAR || kind == PrimitiveKind::USMALL || kind == PrimitiveKind::UINT || kind == PrimitiveKind::ULONG;
    }

    // MAY REQUIRE CAST TO UNSIGNED LONG LONG
    std::pair<long long, long long> getBounds() const
    {
        if (isFloat())
        {
            error::report("Internal compilation error: integer bounds requested for floating-point type");
            return std::make_pair<long long, long long>(0LL, 0LL);
        }

        if (kind == PrimitiveKind::BOOL) return std::make_pair<long long, long long>(0, 1);
        if (kind == PrimitiveKind::VOID) return std::make_pair<long long, long long>(0, 0);
        
        if (isUnsigned())
        {
            if (kind != PrimitiveKind::ULONG)
                return std::make_pair<long long, long long>(
                    0LL, (1LL << (8 * getSizeInBytes())) - 1
                );
            else
                return std::make_pair<long long, long long>(
                    0LL, ULLONG_MAX
                );
        }
        else
        {
            if (kind != PrimitiveKind::LONG)
            {
                int n = 8 * getSizeInBytes() - 1;
                return std::make_pair<long long, long long>(
                    -(1LL << n), (1LL << n) - 1
                );
            }
            else
                return std::make_pair<long long, long long>(
                    LLONG_MIN, LLONG_MAX
                );
        }
    }
};
