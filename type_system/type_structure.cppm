module;

#include <cstddef>

export module types.structure;

export using TypeID = std::size_t;
export namespace ReservedIDs {
    inline constexpr TypeID UNRESOLVED = 0;
    inline constexpr TypeID ERROR = 1;
};

export enum class PrimitiveKind {
    VOID,           // 0 bits
    BYTE, CHAR,     // 8bits
    SMALL, USMALL,  // 16bits
    INT, UINT,      // 32bits
    LONG, ULONG,    // 64bits
    FLOAT, DOUBLE,  // FP32/64
    BOOL            // 8bits addressable; 1bit logic
};

export enum class TypeCategory
{
    INVALID,
    PRIMITIVE,
    STRUCT,
    CLASS,
    INTERFACE
};
