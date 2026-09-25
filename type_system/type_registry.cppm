module;

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <utility>
#include <optional>
#include <stdexcept>

export module types.registry;

import types.info;
import types.structure;

namespace {
    inline constexpr TypeID FIRST_TYPE_ID = ReservedIDs::ERROR + 1;

    [[nodiscard]] TypeID primitiveToID(PrimitiveKind kind) {
        return FIRST_TYPE_ID + static_cast<TypeID>(kind);
    }

    [[nodiscard]] const char* primitiveName(PrimitiveKind kind)
    {
        switch (kind)
        {
        case PrimitiveKind::VOID:   return "void";
        case PrimitiveKind::BYTE:   return "byte";
        case PrimitiveKind::CHAR:   return "char";
        case PrimitiveKind::SMALL:  return "small";
        case PrimitiveKind::USMALL: return "usmall";
        case PrimitiveKind::INT:    return "int";
        case PrimitiveKind::UINT:   return "uint";
        case PrimitiveKind::LONG:   return "long";
        case PrimitiveKind::ULONG:  return "ulong";
        case PrimitiveKind::FLOAT:  return "float";
        case PrimitiveKind::DOUBLE: return "double";
        case PrimitiveKind::BOOL:   return "bool";
        }

        throw std::logic_error("Unexpected PrimitiveKind");
    }
};

export class TypeRegistry
{
private:
    std::vector<std::unique_ptr<TypeData>> typesById;
    std::unordered_map<std::string, TypeID> idsByFullName;

    void registerPrimitive(PrimitiveKind kind)
    {
        const TypeID id = primitiveToID(kind);
        const std::string name = primitiveName(kind);

        if (id >= typesById.size()) typesById.resize(id + 1);
        if (typesById[id] != nullptr) throw std::logic_error("Primitive type ID already registered");
        if (idsByFullName.contains(name)) throw std::logic_error("Primitive type name already registered: " + name);
        
        typesById[id] = std::make_unique<PrimitiveInfo>(id, kind, name);
        idsByFullName.emplace(name, id);
    }

public:
    TypeRegistry() 
    {
        typesById.resize(FIRST_TYPE_ID);

        registerPrimitive(PrimitiveKind::VOID);
        registerPrimitive(PrimitiveKind::BYTE);
        registerPrimitive(PrimitiveKind::CHAR);
        registerPrimitive(PrimitiveKind::SMALL);
        registerPrimitive(PrimitiveKind::USMALL);
        registerPrimitive(PrimitiveKind::INT);
        registerPrimitive(PrimitiveKind::UINT);
        registerPrimitive(PrimitiveKind::LONG);
        registerPrimitive(PrimitiveKind::ULONG);
        registerPrimitive(PrimitiveKind::FLOAT);
        registerPrimitive(PrimitiveKind::DOUBLE);
        registerPrimitive(PrimitiveKind::BOOL);
    }
    
    TypeRegistry(const TypeRegistry&) = delete;
    TypeRegistry& operator=(const TypeRegistry&) = delete;

    [[nodiscard]] TypeInfo getPrimitiveType(PrimitiveKind kind) const {
        return TypeInfo(primitiveToID(kind), TypeCategory::PRIMITIVE);
    }

    [[nodiscard]] const PrimitiveInfo& getPrimitiveData(PrimitiveKind kind) const 
    { 
        const TypeID id = primitiveToID(kind);
        const TypeData& data = getType(id);
        return static_cast<const PrimitiveInfo&>(data);
    }

    [[nodiscard]] const TypeData& getType(TypeID id) const 
    {
        const TypeData* type = findType(id);
        if (type == nullptr)
            throw std::out_of_range("TypeRegistry received an invalid TypeID");
        return *type;
    }

    [[nodiscard]] const TypeData* findType(TypeID id) const 
    {
        if (id >= typesById.size())
            return nullptr;
        return typesById[id].get();
    }

    [[nodiscard]] const TypeData* findType(const std::string& fullName) const 
    {
        auto it = idsByFullName.find(fullName);
        if (it == idsByFullName.end()) return nullptr;
        return findType(it->second);
    }

    [[nodiscard]] std::optional<TypeID> findID(const std::string& fullName) const 
    {
        auto it = idsByFullName.find(fullName);
        if (it == idsByFullName.end()) return std::nullopt;
        return it->second;
    }
};
