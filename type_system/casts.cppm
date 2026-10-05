export module types.casts;

import types.info;
import types.structure;
import types.registry;
import types.data.primitive;

export enum class CastSeverity {
    IDENTITY,
    PROMOTION,
    DEMOTION,
    IMPOSSIBLE
};

export namespace casts
{
    // por hora, apenas compara tipos primitivos. No futuro, faz-se lookup da arvore de hierarquia das classes
    inline CastSeverity getCastSeverity(const TypeInfo& fromType, const TypeInfo& toType, const TypeRegistry& registry)
    {
        if (fromType == toType) return CastSeverity::IDENTITY;

        if (!fromType.isPrimitive() || !toType.isPrimitive()) {
            return CastSeverity::IMPOSSIBLE;
        }

        auto& from = static_cast<const PrimitiveInfo&>(registry.getType(fromType.id));
        auto& to = static_cast<const PrimitiveInfo&>(registry.getType(toType.id));
        
        if (from.isVoid() || to.isVoid()) {
            if (to.isBool()) return CastSeverity::DEMOTION;
            return CastSeverity::IMPOSSIBLE;
        }
        
        if (from.isBool() || to.isBool()) return CastSeverity::DEMOTION;

        if (from.isFloat() && to.isInteger()) return CastSeverity::DEMOTION;
        if (from.isInteger() && to.isFloat()) return CastSeverity::PROMOTION;

        int fromSize = from.getSizeInBytes();
        int toSize = to.getSizeInBytes();

        if (fromSize < toSize) return CastSeverity::PROMOTION;
        if (fromSize > toSize) return CastSeverity::DEMOTION;

        if (from.isUnsigned() != to.isUnsigned()) return CastSeverity::DEMOTION;

        return CastSeverity::IDENTITY;
    }

    inline TypeInfo decideStaticType(const TypeInfo& left, const TypeInfo& right, const TypeRegistry& registry)
    {
        if (!left.isPrimitive() || !right.isPrimitive())
        {
            if (left == right) return left;
            return TypeInfo::errorType();
        }

        auto& l = static_cast<const PrimitiveInfo&>(registry.getType(left.id));
        auto& r = static_cast<const PrimitiveInfo&>(registry.getType(right.id));

        if (l.isVoid() || r.isVoid()) {
            return TypeInfo::errorType();
        }

        if (l.isFloat() || r.isFloat()) 
        {
            if (l.kind == PrimitiveKind::DOUBLE || r.kind == PrimitiveKind::DOUBLE) 
                return registry.getPrimitiveType(PrimitiveKind::DOUBLE);
            return registry.getPrimitiveType(PrimitiveKind::FLOAT);
        }

        if (l.getSizeInBytes() >= r.getSizeInBytes()) return registry.getPrimitiveType(l.kind);
        return registry.getPrimitiveType(r.kind);
    }
}
