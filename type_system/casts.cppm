export module types.casts;

import types.info;

export enum class CastSeverity {
        IDENTITY,
        PROMOTION,
        DEMOTION,
        IMPOSSIBLE
};

export namespace casts
{
    inline CastSeverity getCastSeverity(TypeInfo from, TypeInfo to)
    {
        if (from.kind == to.kind) return CastSeverity::IDENTITY;
        
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

    inline TypeInfo decideStaticType(TypeInfo l, TypeInfo r)
    {
        if (l.isFloat() || r.isFloat()) 
        {
            if (l.kind == TypeKind::DOUBLE || r.kind == TypeKind::DOUBLE) 
                return { TypeKind::DOUBLE };
            return { TypeKind::FLOAT };
        }

        if (l.getSizeInBytes() >= r.getSizeInBytes()) return l;
        return r;
    }
}