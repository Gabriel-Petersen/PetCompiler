module;

#include "stl.h"

export module types.info;

import error;

export enum class TypeKind {
    VOID,           // 0 bits
    BYTE, CHAR,     // 8bits
    SMALL, USMALL,  // 16bits
    INT, UINT,      // 32bits
    LONG, ULONG,    // 64bits
    FLOAT, DOUBLE,  // FP32/64
    BOOL            // 8bits addressable
};

export struct TypeInfo
{
    TypeKind kind;

    int getSizeInBytes() {
        switch (kind)
        {
        case TypeKind::VOID:
            return 0;
        case TypeKind::BYTE:
        case TypeKind::CHAR:
        case TypeKind::BOOL:
            return 1;
        case TypeKind::SMALL:
        case TypeKind::USMALL:
            return 2;
        case TypeKind::INT:
        case TypeKind::UINT:
        case TypeKind::FLOAT:
            return 4;
        case TypeKind::DOUBLE:
        case TypeKind::LONG:
        case TypeKind::ULONG:
            return 8;
        default:
            errorR::report("Unexpected type");
            return 0;
        }
    }

    bool isFloat()   { return kind == TypeKind::FLOAT || kind == TypeKind::DOUBLE; }
    bool isInteger() { return !isFloat() && kind != TypeKind::BOOL && kind != TypeKind::VOID; }
    bool isBool()    { return kind == TypeKind::BOOL; }
    bool isVoid()    { return kind == TypeKind::VOID; }
    
    bool isUnsigned() {
        return kind == TypeKind::CHAR || kind == TypeKind::USMALL || kind == TypeKind::UINT || kind == TypeKind::ULONG;
    }

    // MAY REQUIRE CAST TO UNSIGNED LONG LONG
    std::pair<long long, long long> getBounds() 
    {
        if (kind == TypeKind::BOOL) return std::make_pair<long long, long long>(0, 1);
        if (kind == TypeKind::VOID) return std::make_pair<long long, long long>(0, 0);
        
        if (isUnsigned())
        {
            if (kind != TypeKind::ULONG)
                return std::make_pair<long long, long long>(
                    0, (1LL << (8 * getSizeInBytes())) - 1
                );
            else
                return std::make_pair<long long, long long>(
                    0, ULLONG_MAX
                );
        }
        else
        {
            if (kind != TypeKind::LONG)
            {
                int n = 8 * getSizeInBytes();
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
