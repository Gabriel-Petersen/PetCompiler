#pragma once // file casts.h

#include "type_info.h"

namespace types
{
    enum class CastSeverity {
        IDENTITY,    // Tipos iguais, nenhuma conversão necessária
        PROMOTION,   // Seguro (ex: INT para LONG, ou INT para DOUBLE)
        DEMOTION,    // Risco de perda de dados (ex: LONG para INT, DOUBLE para FLOAT)
        IMPOSSIBLE   // Tipos incompatíveis (ex: BOOL para FLOAT)
    };

    // Função pura para julgar a conversão de um tipo origem para um destino
    inline CastSeverity getCastSeverity(TypeInfo from, TypeInfo to)
    {
        if (from.kind == to.kind) return CastSeverity::IDENTITY;
        
        if (from.isBool() || to.isBool()) return CastSeverity::DEMOTION;

        if (from.isFloat() && to.isInteger()) return CastSeverity::DEMOTION; // Float para Int sempre perde precisão
        if (from.isInteger() && to.isFloat()) return CastSeverity::PROMOTION; // Int para Float é promoção limpa

        // Comparação de tamanhos de Inteiros ou de Floats
        int fromSize = from.getSizeInBytes();
        int toSize = to.getSizeInBytes();

        if (fromSize < toSize) return CastSeverity::PROMOTION;
        if (fromSize > toSize) return CastSeverity::DEMOTION;

        // Se têm o mesmo tamanho mas sinais diferentes (ex: INT para UINT), tratamos como risco (Demotion)
        if (from.isUnsigned() != to.isUnsigned()) return CastSeverity::DEMOTION;

        return CastSeverity::IDENTITY;
    }

    inline TypeInfo decideStaticType(TypeInfo l, TypeInfo r)
    {
        // Se houver float envolvido, o maior float ganha
        if (l.isFloat() || r.isFloat()) 
        {
            if (l.kind == TypeKind::DOUBLE || r.kind == TypeKind::DOUBLE) 
                return { TypeKind::DOUBLE };
            return { TypeKind::FLOAT };
        }

        // Se forem inteiros, o de maior tamanho em bytes ganha
        if (l.getSizeInBytes() >= r.getSizeInBytes()) return l;
        return r;
    }
}