#pragma once
#include "core.hpp"

namespace horizon {
class ToolHelperMapSymbol : public virtual ToolBase {
public:
    ToolHelperMapSymbol(class Core *c, ToolID tid) : ToolBase(c, tid)
    {
    }

protected:
    class SchematicSymbol *map_symbol(class Component *c, const class Gate *g);

private:
    std::map<UUID, UUID> placed_symbols; // unit to symbol
};
} // namespace horizon
