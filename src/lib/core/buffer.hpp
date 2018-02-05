#pragma once
#include "block/component.hpp"
#include "block/net.hpp"
#include "canvas/selectables.hpp"
#include "common/arc.hpp"
#include "common/hole.hpp"
#include "common/junction.hpp"
#include "common/line.hpp"
#include "common/polygon.hpp"
#include "common/shape.hpp"
#include "common/text.hpp"
#include "cores.hpp"
#include "json.hpp"
#include "package/pad.hpp"
#include "pool/symbol.hpp"
#include "schematic/net_label.hpp"
#include "schematic/power_symbol.hpp"
#include "schematic/schematic_symbol.hpp"
#include "util/uuid.hpp"
#include <map>
#include <set>

namespace horizon {
class Buffer {
public:
    Buffer(class Core *co);
    void clear();
    void load_from_symbol(std::set<SelectableRef> selection);

    std::map<UUID, Text> texts;
    std::map<UUID, Junction> junctions;
    std::map<UUID, Line> lines;
    std::map<UUID, Arc> arcs;
    std::map<UUID, Pad> pads;
    std::map<UUID, Polygon> polygons;
    std::map<UUID, Component> components;
    std::map<UUID, SchematicSymbol> symbols;
    std::map<UUID, SymbolPin> pins;
    std::map<UUID, Net> nets;
    std::map<UUID, LineNet> net_lines;
    std::map<UUID, Hole> holes;
    std::map<UUID, Shape> shapes;
    std::map<UUID, PowerSymbol> power_symbols;
    std::map<UUID, NetLabel> net_labels;

    json serialize();

private:
    Cores core;
    NetClass net_class_dummy;
};
} // namespace horizon
