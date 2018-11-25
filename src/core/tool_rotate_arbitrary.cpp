#include "tool_rotate_arbitrary.hpp"
#include "canvas/canvas_gl.hpp"
#include "core_board.hpp"
#include "core_package.hpp"
#include "core_padstack.hpp"
#include "core_schematic.hpp"
#include "core_symbol.hpp"
#include "imp/imp_interface.hpp"
#include "util/accumulator.hpp"
#include "util/util.hpp"
#include <iostream>

namespace horizon {

ToolRotateArbitrary::ToolRotateArbitrary(Core *c, ToolID tid) : ToolBase(c, tid)
{
}

void ToolRotateArbitrary::expand_selection()
{
    std::set<SelectableRef> new_sel;
    for (const auto &it : core.r->selection) {
        switch (it.type) {
        case ObjectType::LINE: {
            Line *line = core.r->get_line(it.uuid);
            new_sel.emplace(line->from.uuid, ObjectType::JUNCTION);
            new_sel.emplace(line->to.uuid, ObjectType::JUNCTION);
        } break;
        case ObjectType::POLYGON_EDGE: {
            Polygon *poly = core.r->get_polygon(it.uuid);
            auto vs = poly->get_vertices_for_edge(it.vertex);
            new_sel.emplace(poly->uuid, ObjectType::POLYGON_VERTEX, vs.first);
            new_sel.emplace(poly->uuid, ObjectType::POLYGON_VERTEX, vs.second);
        } break;
        case ObjectType::POLYGON: {
            auto poly = core.r->get_polygon(it.uuid);
            int i = 0;
            for (const auto &itv : poly->vertices) {
                (void)sizeof itv;
                new_sel.emplace(poly->uuid, ObjectType::POLYGON_VERTEX, i);
                i++;
            }
        } break;

        case ObjectType::ARC: {
            Arc *arc = core.r->get_arc(it.uuid);
            new_sel.emplace(arc->from.uuid, ObjectType::JUNCTION);
            new_sel.emplace(arc->to.uuid, ObjectType::JUNCTION);
            new_sel.emplace(arc->center.uuid, ObjectType::JUNCTION);
        } break;

        case ObjectType::BOARD_PACKAGE: {
            BoardPackage *pkg = &core.b->get_board()->packages.at(it.uuid);
            for (const auto &itt : pkg->texts) {
                new_sel.emplace(itt->uuid, ObjectType::TEXT);
            }
        } break;

        default:;
        }
    }
    core.r->selection.insert(new_sel.begin(), new_sel.end());
}

ToolResponse ToolRotateArbitrary::begin(const ToolArgs &args)
{
    std::cout << "tool move\n";
    ref = args.coords;
    origin = args.coords;

    save_placements();
    annotation = imp->get_canvas()->create_annotation();
    annotation->set_visible(true);
    annotation->set_display(LayerDisplay(true, LayerDisplay::Mode::OUTLINE));

    update_tip();
    return ToolResponse();
}

ToolRotateArbitrary::~ToolRotateArbitrary()
{
    if (annotation) {
        imp->get_canvas()->remove_annotation(annotation);
        annotation = nullptr;
    }
}

bool ToolRotateArbitrary::can_begin()
{
    if (core.c) // dont' rotate arbitraryily in schematic
        return false;
    expand_selection();
    return core.r->selection.size() > 0;
}

void ToolRotateArbitrary::update_tip()
{
    if (state == State::ORIGIN) {
        imp->tool_bar_set_tip("<b>LMB:</b>set origin <b>RMB:</b>cancel");
    }
    else if (state == State::REF) {
        imp->tool_bar_set_tip("<b>LMB:</b>set ref <b>RMB:</b>cancel");
    }
    else if (state == State::ROTATE) {
        std::stringstream ss;
        ss << "<b>LMB:</b>finish <b>RMB:</b>cancel <b>s:</b>toggle snap <i>";
        ss << "Angle: " << angle_to_string(iangle, false);
        if (snap)
            ss << " (snapped)";
        ss << "</i>";
        imp->tool_bar_set_tip(ss.str());
    }
    else if (state == State::SCALE) {
        std::stringstream ss;
        ss << "<b>LMB:</b>finish <b>RMB:</b>cancel <i>";
        ss << "Scale: " << scale;
        ss << "</i>";
        imp->tool_bar_set_tip(ss.str());
    }
    // auto delta = last-origin;

    // imp->tool_bar_set_tip("<b>LMB:</b>place <b>RMB:</b>cancel <b>r:</b>rotate
    // <b>e:</b>mirror "+coord_to_string(delta, true));
}

void ToolRotateArbitrary::save_placements()
{
    for (const auto &it : core.r->selection) {
        switch (it.type) {
        case ObjectType::JUNCTION:
            placements[it] = Placement(core.r->get_junction(it.uuid)->position);
            break;
        case ObjectType::POLYGON_VERTEX:
            placements[it] = Placement(core.r->get_polygon(it.uuid)->vertices.at(it.vertex).position);
            break;
        case ObjectType::POLYGON_ARC_CENTER:
            placements[it] = Placement(core.r->get_polygon(it.uuid)->vertices.at(it.vertex).arc_center);
            break;
        case ObjectType::TEXT:
            placements[it] = core.r->get_text(it.uuid)->placement;
            break;
        case ObjectType::BOARD_PACKAGE:
            placements[it] = core.b->get_board()->packages.at(it.uuid).placement;
            break;
        default:;
        }
    }
}

static Placement rotate_placement(const Placement &p, const Coordi &origin, int angle)
{
    Placement q = p;
    q.shift -= origin;
    Placement t(origin, angle);
    t.accumulate(q);
    return t;
}

void ToolRotateArbitrary::apply_placements_rotation(int angle)
{
    for (const auto &it : placements) {
        switch (it.first.type) {
        case ObjectType::JUNCTION:
            core.r->get_junction(it.first.uuid)->position = rotate_placement(it.second, origin, angle).shift;
            break;
        case ObjectType::POLYGON_VERTEX:
            core.r->get_polygon(it.first.uuid)->vertices.at(it.first.vertex).position =
                    rotate_placement(it.second, origin, angle).shift;
            break;
        case ObjectType::POLYGON_ARC_CENTER:
            core.r->get_polygon(it.first.uuid)->vertices.at(it.first.vertex).arc_center =
                    rotate_placement(it.second, origin, angle).shift;
            break;
        case ObjectType::TEXT: {
            auto &pl = core.r->get_text(it.first.uuid)->placement;
            pl = rotate_placement(it.second, origin, angle);
            if (pl.mirror)
                pl.inc_angle(-2 * angle);
        } break;
        case ObjectType::BOARD_PACKAGE:
            core.b->get_board()->packages.at(it.first.uuid).placement = rotate_placement(it.second, origin, angle);
            break;
        default:;
        }
    }
}

static Placement scale_placement(const Placement &p, const Coordi &origin, double scale)
{
    Placement q = p;
    q.shift -= origin;
    q.shift.x *= scale;
    q.shift.y *= scale;
    q.shift += origin;
    return q;
}

void ToolRotateArbitrary::apply_placements_scale(double sc)
{
    for (const auto &it : placements) {
        switch (it.first.type) {
        case ObjectType::JUNCTION:
            core.r->get_junction(it.first.uuid)->position = scale_placement(it.second, origin, sc).shift;
            break;
        case ObjectType::POLYGON_VERTEX:
            core.r->get_polygon(it.first.uuid)->vertices.at(it.first.vertex).position =
                    scale_placement(it.second, origin, sc).shift;
            break;
        case ObjectType::POLYGON_ARC_CENTER:
            core.r->get_polygon(it.first.uuid)->vertices.at(it.first.vertex).arc_center =
                    scale_placement(it.second, origin, sc).shift;
            break;
        case ObjectType::TEXT:
            core.r->get_text(it.first.uuid)->placement = scale_placement(it.second, origin, sc);
            break;
        case ObjectType::BOARD_PACKAGE:
            core.b->get_board()->packages.at(it.first.uuid).placement = scale_placement(it.second, origin, sc);
            break;
        default:;
        }
    }
}

ToolResponse ToolRotateArbitrary::update(const ToolArgs &args)
{
    if (args.type == ToolEventType::MOVE) {
        if (state == State::ORIGIN) {
            origin = args.coords;
        }
        else if (state == State::REF) {
            ref = args.coords;
        }
        else if (state == State::SCALE) {
            double vr = sqrt((ref - origin).mag_sq());
            double v = sqrt((args.coords - origin).mag_sq());
            scale = v / vr;
            apply_placements_scale(scale);
        }
        else if (state == State::ROTATE) {
            auto rref = args.coords;
            annotation->clear();
            annotation->draw_line(origin, rref, ColorP::FROM_LAYER, 2);

            auto v = rref - origin;
            double angle = atan2(v.y, v.x);
            iangle = ((angle / (2 * M_PI)) * 65536);
            iangle += 65536 * 2;
            iangle %= 65536;
            if (snap)
                iangle = round_multiple(iangle, 8192);
            apply_placements_rotation(iangle);
        }
        update_tip();
        return ToolResponse::fast();
    }
    else if (args.type == ToolEventType::CLICK) {
        if (args.button == 1) {
            if (tool_id == ToolID::ROTATE_ARBITRARY) {
                if (state == State::ORIGIN) {
                    state = State::ROTATE;
                    update_tip();
                }
                else {
                    core.r->commit();
                    return ToolResponse::end();
                }
            }
            else { // scale
                if (state == State::ORIGIN) {
                    state = State::REF;
                    ref = args.coords;
                    update_tip();
                }
                else if (state == State::REF) {
                    state = State::SCALE;
                    update_tip();
                }
                else {
                    core.r->commit();
                    return ToolResponse::end();
                }
            }
        }
        else {
            core.r->revert();
            return ToolResponse::end();
        }
    }
    else if (args.type == ToolEventType::KEY) {
        if (args.key == GDK_KEY_s) {
            snap ^= true;
        }
        else if (args.key == GDK_KEY_Escape) {
            core.r->revert();
            return ToolResponse::end();
        }
    }
    return ToolResponse();
}
} // namespace horizon
