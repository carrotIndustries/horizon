#include "tool_assign_part.hpp"
#include "core_schematic.hpp"
#include "imp-util/imp_interface.hpp"
#include "pool/part.hpp"
#include <iostream>

namespace horizon {

ToolAssignPart::ToolAssignPart(Core *c, ToolID tid) : ToolBase(c, tid)
{
}

bool ToolAssignPart::can_begin()
{
    return get_entity() != nullptr;
}

const Entity *ToolAssignPart::get_entity()
{
    const Entity *entity = nullptr;
    for (const auto &it : core.r->selection) {
        if (it.type == ObjectType::SCHEMATIC_SYMBOL) {
            auto sym = core.c->get_schematic_symbol(it.uuid);
            if (entity) {
                if (entity != sym->component->entity) {
                    return nullptr;
                }
            }
            else {
                entity = sym->component->entity;
                comp = sym->component;
            }
        }
    }
    return entity;
}

ToolResponse ToolAssignPart::begin(const ToolArgs &args)
{
    std::cout << "tool assing part\n";
    const Entity *entity = get_entity();

    if (!entity) {
        return ToolResponse::end();
    }
    UUID part_uuid = comp->part ? comp->part->uuid : UUID();
    auto r = imp->dialogs.select_part(core.r->m_pool, entity->uuid, part_uuid, true);
    if (r.first) {
        const Part *part = nullptr;
        if (r.second) {
            part = core.r->m_pool->get_part(r.second);
        }

        for (const auto &it : args.selection) {
            if (it.type == ObjectType::SCHEMATIC_SYMBOL) {
                auto sym = core.c->get_schematic_symbol(it.uuid);
                if (sym->component->entity == entity) {
                    sym->component->part = part;
                }
            }
        }

        core.r->commit();
    }
    return ToolResponse::end();
}
ToolResponse ToolAssignPart::update(const ToolArgs &args)
{
    return ToolResponse();
}
} // namespace horizon
