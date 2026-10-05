module;

#include <string>
#include <utility>

export module types.data;

import types.info;
import types.nameSpace;
import types.structure;

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
