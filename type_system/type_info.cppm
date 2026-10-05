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

export struct SymbolInfo {
    TypeInfo tpInfo;
    AccessMode access;
};
