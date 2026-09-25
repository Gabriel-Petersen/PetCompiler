module;

#include <climits>
#include <cstddef>
#include <string>
#include <utility>

export module types.info;

import error;
import types.structure;
import types.nameSpace;

export struct TypeInfo
{
    TypeID id;
    TypeCategory category;

    TypeInfo() : id(ReservedIDs::UNRESOLVED), category(TypeCategory::INVALID) { }
    explicit TypeInfo(const TypeID _id, const TypeCategory _category) : id(_id), category(_category) { }

    [[nodiscard]] bool isValid() const { return category != TypeCategory::INVALID; }
    [[nodiscard]] bool isResolved() const { return id != ReservedIDs::UNRESOLVED; }
    [[nodiscard]] bool isError() const { return id == ReservedIDs::ERROR; }
    [[nodiscard]] bool isPrimitive() const { return category == TypeCategory::PRIMITIVE; }
    bool operator==(const TypeInfo& o) const { return id == o.id; }
    bool operator!=(const TypeInfo& o) const { return id != o.id; }

    [[nodiscard]] static TypeInfo unresolvedType() {
        return TypeInfo(ReservedIDs::UNRESOLVED, TypeCategory::INVALID);
    }

    [[nodiscard]] static TypeInfo errorType() {
        return TypeInfo(ReservedIDs::ERROR, TypeCategory::INVALID);
    }
};

export class TypeData
{
private:
    bool enabled;

public:
    const TypeID id;
    const TypeCategory category;
    const std::string name;
    const Namespace nameSpace;
    const std::string fullNameCache;

    explicit TypeData(TypeID typeId, TypeCategory typeCategory, std::string typeName, const std::string& namespaceString) : 
        enabled(false), id(typeId), category(typeCategory), name(std::move(typeName)), nameSpace(namespaceString),
        fullNameCache(nameSpace.fullName.empty() ? name : nameSpace.fullName + "." + name)
    { }

    void setEnabled(bool _enabled) { enabled = _enabled; }

    [[nodiscard]] bool isEnabled() const { return enabled; }
    
    // if (!enabled) show error and return 0
    virtual std::size_t getSizeInBytes() const = 0;
    virtual ~TypeData() = default;
};

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
