#include "core.hpp"
#include "tool_add_part.hpp"
#include "tool_add_vertex.hpp"
#include "tool_assign_part.hpp"
#include "tool_bend_line_net.hpp"
#include "tool_delete.hpp"
#include "tool_disconnect.hpp"
#include "tool_drag_keep_slope.hpp"
#include "tool_draw_arc.hpp"
#include "tool_draw_dimension.hpp"
#include "tool_draw_line.hpp"
#include "tool_draw_line_net.hpp"
#include "tool_draw_line_rectangle.hpp"
#include "tool_draw_polygon.hpp"
#include "tool_draw_polygon_rectangle.hpp"
#include "tool_draw_track.hpp"
#include "tool_edit_board_hole.hpp"
#include "tool_edit_line_rectangle.hpp"
#include "tool_edit_pad_parameter_set.hpp"
#include "tool_edit_parameter_program.hpp"
#include "tool_edit_plane.hpp"
#include "tool_edit_shape.hpp"
#include "tool_edit_symbol_pin_names.hpp"
#include "tool_edit_via.hpp"
#include "tool_enter_datum.hpp"
#include "tool_import_dxf.hpp"
#include "tool_lock.hpp"
#include "tool_manage_buses.hpp"
#include "tool_map_package.hpp"
#include "tool_map_pin.hpp"
#include "tool_map_symbol.hpp"
#include "tool_move.hpp"
#include "tool_move_net_segment.hpp"
#include "tool_paste.hpp"
#include "tool_place_board_hole.hpp"
#include "tool_place_bus_label.hpp"
#include "tool_place_bus_ripper.hpp"
#include "tool_place_hole.hpp"
#include "tool_place_junction.hpp"
#include "tool_place_net_label.hpp"
#include "tool_place_pad.hpp"
#include "tool_place_power_symbol.hpp"
#include "tool_place_shape.hpp"
#include "tool_place_text.hpp"
#include "tool_place_via.hpp"
#include "tool_rotate_arbitrary.hpp"
#include "tool_route_track.hpp"
#include "tool_route_track_interactive.hpp"
#include "tool_select_more.hpp"
#include "tool_set_diffpair.hpp"
#include "tool_set_via_net.hpp"
#include "tool_smash.hpp"
#include "tool_update_all_planes.hpp"
#include "tool_generate_courtyard.hpp"
#include "tool_set_group.hpp"
#include "tool_copy_placement.hpp"
#include "tool_copy_tracks.hpp"
#include "tool_swap_nets.hpp"
#include "tool_line_loop_to_polygon.hpp"
#include "tool_change_unit.hpp"
#include "tool_set_nc_all.hpp"
#include "tool_set_nc.hpp"
#include "tool_add_keepout.hpp"
#include "tool_change_symbol.hpp"

#include "common/dimension.hpp"
#include "logger/logger.hpp"
#include <memory>
#include "nlohmann/json.hpp"
#include "imp/action_catalog.hpp"
#include "util/util.hpp"

namespace horizon {

std::unique_ptr<ToolBase> Core::create_tool(ToolID tool_id)
{
    switch (tool_id) {
    case ToolID::MOVE:
    case ToolID::MOVE_EXACTLY:
    case ToolID::MIRROR_X:
    case ToolID::MIRROR_Y:
    case ToolID::ROTATE:
    case ToolID::MOVE_KEY:
    case ToolID::MOVE_KEY_UP:
    case ToolID::MOVE_KEY_DOWN:
    case ToolID::MOVE_KEY_LEFT:
    case ToolID::MOVE_KEY_RIGHT:
    case ToolID::MOVE_KEY_UP_FINE:
    case ToolID::MOVE_KEY_DOWN_FINE:
    case ToolID::MOVE_KEY_LEFT_FINE:
    case ToolID::MOVE_KEY_RIGHT_FINE:
    case ToolID::MOVE_KEY_UP_COARSE:
    case ToolID::MOVE_KEY_DOWN_COARSE:
    case ToolID::MOVE_KEY_LEFT_COARSE:
    case ToolID::MOVE_KEY_RIGHT_COARSE:
        return std::make_unique<ToolMove>(this, tool_id);

    case ToolID::PLACE_JUNCTION:
        return std::make_unique<ToolPlaceJunction>(this, tool_id);

    case ToolID::DRAW_LINE:
        return std::make_unique<ToolDrawLine>(this, tool_id);

    case ToolID::DELETE:
        return std::make_unique<ToolDelete>(this, tool_id);

    case ToolID::DRAW_ARC:
        return std::make_unique<ToolDrawArc>(this, tool_id);

    case ToolID::MAP_PIN:
        return std::make_unique<ToolMapPin>(this, tool_id);

    case ToolID::MAP_SYMBOL:
        return std::make_unique<ToolMapSymbol>(this, tool_id);

    case ToolID::DRAW_NET:
        return std::make_unique<ToolDrawLineNet>(this, tool_id);

    case ToolID::ADD_COMPONENT:
        return std::make_unique<ToolAddPart>(this, tool_id);

    case ToolID::PLACE_TEXT:
        return std::make_unique<ToolPlaceText>(this, tool_id);

    case ToolID::PLACE_NET_LABEL:
        return std::make_unique<ToolPlaceNetLabel>(this, tool_id);

    case ToolID::DISCONNECT:
        return std::make_unique<ToolDisconnect>(this, tool_id);

    case ToolID::BEND_LINE_NET:
        return std::make_unique<ToolBendLineNet>(this, tool_id);

    case ToolID::SELECT_NET_SEGMENT:
    case ToolID::MOVE_NET_SEGMENT:
    case ToolID::MOVE_NET_SEGMENT_NEW:
        return std::make_unique<ToolMoveNetSegment>(this, tool_id);

    case ToolID::PLACE_POWER_SYMBOL:
        return std::make_unique<ToolPlacePowerSymbol>(this, tool_id);

    case ToolID::EDIT_SYMBOL_PIN_NAMES:
        return std::make_unique<ToolEditSymbolPinNames>(this, tool_id);

    case ToolID::PLACE_BUS_LABEL:
        return std::make_unique<ToolPlaceBusLabel>(this, tool_id);

    case ToolID::PLACE_BUS_RIPPER:
        return std::make_unique<ToolPlaceBusRipper>(this, tool_id);

    case ToolID::MANAGE_BUSES:
    case ToolID::MANAGE_NET_CLASSES:
    case ToolID::EDIT_STACKUP:
    case ToolID::ANNOTATE:
    case ToolID::EDIT_SCHEMATIC_PROPERTIES:
    case ToolID::MANAGE_POWER_NETS:
    case ToolID::EDIT_FRAME_PROPERTIES:
    case ToolID::TOGGLE_GROUP_TAG_VISIBLE:
        return std::make_unique<ToolManageBuses>(this, tool_id);

    case ToolID::DRAW_POLYGON:
        return std::make_unique<ToolDrawPolygon>(this, tool_id);

    case ToolID::ENTER_DATUM:
        return std::make_unique<ToolEnterDatum>(this, tool_id);

    case ToolID::PLACE_HOLE:
        return std::make_unique<ToolPlaceHole>(this, tool_id);

    case ToolID::PLACE_PAD:
        return std::make_unique<ToolPlacePad>(this, tool_id);

    case ToolID::PASTE:
        return std::make_unique<ToolPaste>(this, tool_id);

    case ToolID::ASSIGN_PART:
        return std::make_unique<ToolAssignPart>(this, tool_id);

    case ToolID::MAP_PACKAGE:
        return std::make_unique<ToolMapPackage>(this, tool_id);

    case ToolID::DRAW_TRACK:
        return std::make_unique<ToolDrawTrack>(this, tool_id);

    case ToolID::PLACE_VIA:
        return std::make_unique<ToolPlaceVia>(this, tool_id);

    case ToolID::ROUTE_TRACK:
        return std::make_unique<ToolRouteTrack>(this, tool_id);

    case ToolID::DRAG_KEEP_SLOPE:
        return std::make_unique<ToolDragKeepSlope>(this, tool_id);

    case ToolID::ADD_PART:
        return std::make_unique<ToolAddPart>(this, tool_id);

    case ToolID::SMASH:
        return std::make_unique<ToolSmash>(this, tool_id);

    case ToolID::UNSMASH:
        return std::make_unique<ToolSmash>(this, tool_id);

    case ToolID::PLACE_SHAPE:
        return std::make_unique<ToolPlaceShape>(this, tool_id);

    case ToolID::EDIT_SHAPE:
        return std::make_unique<ToolEditShape>(this, tool_id);

    case ToolID::IMPORT_DXF:
        return std::make_unique<ToolImportDXF>(this, tool_id);

    case ToolID::EDIT_PARAMETER_PROGRAM:
        return std::make_unique<ToolEditParameterProgram>(this, tool_id);

    case ToolID::EDIT_PARAMETER_SET:
        return std::make_unique<ToolEditParameterProgram>(this, tool_id);

    case ToolID::EDIT_PAD_PARAMETER_SET:
        return std::make_unique<ToolEditPadParameterSet>(this, tool_id);

    case ToolID::DRAW_POLYGON_RECTANGLE:
        return std::make_unique<ToolDrawPolygonRectangle>(this, tool_id);

    case ToolID::DRAW_LINE_RECTANGLE:
        return std::make_unique<ToolDrawLineRectangle>(this, tool_id);

    case ToolID::EDIT_LINE_RECTANGLE:
        return std::make_unique<ToolEditLineRectangle>(this, tool_id);

    case ToolID::ROUTE_TRACK_INTERACTIVE:
    case ToolID::ROUTE_DIFFPAIR_INTERACTIVE:
    case ToolID::DRAG_TRACK_INTERACTIVE:
    case ToolID::TUNE_TRACK:
    case ToolID::TUNE_DIFFPAIR:
    case ToolID::TUNE_DIFFPAIR_SKEW:
        return std::make_unique<ToolRouteTrackInteractive>(this, tool_id);

    case ToolID::EDIT_VIA:
        return std::make_unique<ToolEditVia>(this, tool_id);

    case ToolID::ROTATE_ARBITRARY:
    case ToolID::SCALE:
        return std::make_unique<ToolRotateArbitrary>(this, tool_id);

    case ToolID::ADD_PLANE:
        return std::make_unique<ToolEditPlane>(this, tool_id);

    case ToolID::EDIT_PLANE:
    case ToolID::CLEAR_PLANE:
    case ToolID::UPDATE_PLANE:
        return std::make_unique<ToolEditPlane>(this, tool_id);

    case ToolID::CLEAR_ALL_PLANES:
    case ToolID::UPDATE_ALL_PLANES:
        return std::make_unique<ToolUpdateAllPlanes>(this, tool_id);

    case ToolID::DRAW_DIMENSION:
        return std::make_unique<ToolDrawDimension>(this, tool_id);

    case ToolID::SET_DIFFPAIR:
    case ToolID::CLEAR_DIFFPAIR:
        return std::make_unique<ToolSetDiffpair>(this, tool_id);

    case ToolID::SELECT_MORE:
    case ToolID::SELECT_MORE_NO_VIA:
        return std::make_unique<ToolSelectMore>(this, tool_id);

    case ToolID::SET_VIA_NET:
    case ToolID::CLEAR_VIA_NET:
        return std::make_unique<ToolSetViaNet>(this, tool_id);

    case ToolID::LOCK:
    case ToolID::UNLOCK:
    case ToolID::UNLOCK_ALL:
        return std::make_unique<ToolLock>(this, tool_id);

    case ToolID::ADD_VERTEX:
        return std::make_unique<ToolAddVertex>(this, tool_id);

    case ToolID::PLACE_BOARD_HOLE:
        return std::make_unique<ToolPlaceBoardHole>(this, tool_id);

    case ToolID::EDIT_BOARD_HOLE:
        return std::make_unique<ToolEditBoardHole>(this, tool_id);

    case ToolID::GENERATE_COURTYARD:
        return std::make_unique<ToolGenerateCourtyard>(this, tool_id);

    case ToolID::SET_GROUP:
    case ToolID::SET_NEW_GROUP:
    case ToolID::CLEAR_GROUP:
    case ToolID::SET_TAG:
    case ToolID::SET_NEW_TAG:
    case ToolID::CLEAR_TAG:
    case ToolID::RENAME_GROUP:
    case ToolID::RENAME_TAG:
        return std::make_unique<ToolSetGroup>(this, tool_id);

    case ToolID::COPY_PLACEMENT:
        return std::make_unique<ToolCopyPlacement>(this, tool_id);

    case ToolID::COPY_TRACKS:
        return std::make_unique<ToolCopyTracks>(this, tool_id);

    case ToolID::SWAP_NETS:
        return std::make_unique<ToolSwapNets>(this, tool_id);

    case ToolID::LINE_LOOP_TO_POLYGON:
        return std::make_unique<ToolLineLoopToPolygon>(this, tool_id);

    case ToolID::CHANGE_UNIT:
        return std::make_unique<ToolChangeUnit>(this, tool_id);

    case ToolID::SET_ALL_NC:
    case ToolID::CLEAR_ALL_NC:
        return std::make_unique<ToolSetNotConnectedAll>(this, tool_id);

    case ToolID::SET_NC:
    case ToolID::CLEAR_NC:
        return std::make_unique<ToolSetNotConnected>(this, tool_id);

    case ToolID::ADD_KEEPOUT:
    case ToolID::EDIT_KEEPOUT:
        return std::make_unique<ToolAddKeepout>(this, tool_id);

    case ToolID::CHANGE_SYMBOL:
        return std::make_unique<ToolChangeSymbol>(this, tool_id);

    default:
        return nullptr;
    }
}

ToolResponse Core::tool_begin(ToolID tool_id, const ToolArgs &args, class ImpInterface *imp, bool transient)
{
    if (!args.keep_selection) {
        selection.clear();
        selection = args.selection;
    }
    update_rules(); // write rules to board, so tool has the current rules
    try {
        tool = create_tool(tool_id);
        {
            auto sp = tool->get_settings_proxy();
            if (sp != nullptr) {
                auto tid = tool->get_tool_id_for_settings();
                auto j = s_signal_load_tool_settings.emit(tid);
                if (j != nullptr)
                    sp->load_from_json(j);
            }
        }
        tool->set_imp_interface(imp);
        if (transient)
            tool->set_transient();
        if (!tool->can_begin()) { // check if we can actually use this tool
            tool.reset();
            return ToolResponse();
        }
    }
    catch (const std::exception &e) {
        Logger::log_critical("exception thrown in tool constructor of "
                                     + action_catalog.at({ActionID::TOOL, tool_id}).name,
                             Logger::Domain::CORE, e.what());
        return ToolResponse::end();
    }
    if (tool) {
        s_signal_tool_changed.emit(tool_id);
        ToolResponse r;
        try {
            r = tool->begin(args);
        }
        catch (const std::exception &e) {
            tool.reset();
            s_signal_tool_changed.emit(ToolID::NONE);
            Logger::log_critical("exception thrown in tool_begin of "
                                         + action_catalog.at({ActionID::TOOL, tool_id}).name,
                                 Logger::Domain::CORE, e.what());
            return ToolResponse::end();
        }
        if (r.end_tool) {
            auto tid = tool->get_tool_id_for_settings();
            auto settings = tool->get_settings_const();
            if (settings)
                s_signal_save_tool_settings.emit(tid, settings->serialize());
            tool.reset();
            s_signal_tool_changed.emit(ToolID::NONE);
            rebuild();
        }

        return r;
    }

    return ToolResponse();
}

std::pair<bool, bool> Core::tool_can_begin(ToolID tool_id, const std::set<SelectableRef> &sel)
{
    auto t = create_tool(tool_id);
    auto sel_saved = selection;
    selection = sel;
    auto r = t->can_begin();
    auto s = t->is_specific();
    selection = sel_saved;
    return {r, s};
}

bool Core::tool_handles_esc()
{
    if (tool) {
        return tool->handles_esc();
    }
    return false;
}

ToolResponse Core::tool_update(const ToolArgs &args)
{
    if (tool) {
        ToolResponse r;
        try {
            r = tool->update(args);
        }
        catch (const std::exception &e) {
            tool.reset();
            s_signal_tool_changed.emit(ToolID::NONE);
            Logger::log_critical("exception thrown in tool_update", Logger::Domain::CORE, e.what());
            return ToolResponse::end();
        }
        if (r.end_tool) {
            auto tid = tool->get_tool_id_for_settings();
            auto settings = tool->get_settings_const();
            if (settings)
                s_signal_save_tool_settings.emit(tid, settings->serialize());
            tool.reset();
            s_signal_tool_changed.emit(ToolID::NONE);
            rebuild();
        }
        return r;
    }
    return ToolResponse();
}

Junction *Core::insert_junction(const UUID &uu, bool work)
{
    auto map = get_junction_map(work);
    auto x = map->emplace(std::make_pair(uu, uu));
    return &(x.first->second);
}

Junction *Core::get_junction(const UUID &uu, bool work)
{
    auto map = get_junction_map(work);
    return &map->at(uu);
}

void Core::delete_junction(const UUID &uu, bool work)
{
    auto map = get_junction_map(work);
    map->erase(uu);
}

Line *Core::insert_line(const UUID &uu, bool work)
{
    auto map = get_line_map(work);
    auto x = map->emplace(std::make_pair(uu, uu));
    return &(x.first->second);
}

Line *Core::get_line(const UUID &uu, bool work)
{
    auto map = get_line_map(work);
    return &map->at(uu);
}

void Core::delete_line(const UUID &uu, bool work)
{
    auto map = get_line_map(work);
    map->erase(uu);
}

std::vector<Line *> Core::get_lines(bool work)
{
    auto *map = get_line_map(work);
    std::vector<Line *> r;
    if (!map)
        return r;
    for (auto &it : *map) {
        r.push_back(&it.second);
    }
    return r;
}

Arc *Core::insert_arc(const UUID &uu, bool work)
{
    auto map = get_arc_map(work);
    auto x = map->emplace(std::make_pair(uu, uu));
    return &(x.first->second);
}

Arc *Core::get_arc(const UUID &uu, bool work)
{
    auto map = get_arc_map(work);
    return &map->at(uu);
}

void Core::delete_arc(const UUID &uu, bool work)
{
    auto map = get_arc_map(work);
    map->erase(uu);
}

std::vector<Arc *> Core::get_arcs(bool work)
{
    auto *map = get_arc_map(work);
    std::vector<Arc *> r;
    if (!map)
        return r;
    for (auto &it : *map) {
        r.push_back(&it.second);
    }
    return r;
}

Text *Core::insert_text(const UUID &uu, bool work)
{
    auto map = get_text_map(work);
    auto x = map->emplace(uu, uu);
    return &(x.first->second);
}

Text *Core::get_text(const UUID &uu, bool work)
{
    auto map = get_text_map(work);
    return &map->at(uu);
}

void Core::delete_text(const UUID &uu, bool work)
{
    auto map = get_text_map(work);
    map->erase(uu);
}

Polygon *Core::insert_polygon(const UUID &uu, bool work)
{
    auto map = get_polygon_map(work);
    auto x = map->emplace(std::make_pair(uu, uu));
    return &(x.first->second);
}

Polygon *Core::get_polygon(const UUID &uu, bool work)
{
    auto map = get_polygon_map(work);
    return &map->at(uu);
}

void Core::delete_polygon(const UUID &uu, bool work)
{
    auto map = get_polygon_map(work);
    map->erase(uu);
}

Hole *Core::insert_hole(const UUID &uu, bool work)
{
    auto map = get_hole_map(work);
    auto x = map->emplace(std::make_pair(uu, uu));
    return &(x.first->second);
}

Hole *Core::get_hole(const UUID &uu, bool work)
{
    auto map = get_hole_map(work);
    return &map->at(uu);
}

void Core::delete_hole(const UUID &uu, bool work)
{
    auto map = get_hole_map(work);
    map->erase(uu);
}

Dimension *Core::insert_dimension(const UUID &uu)
{
    auto map = get_dimension_map();
    auto x = map->emplace(std::make_pair(uu, uu));
    return &(x.first->second);
}

Dimension *Core::get_dimension(const UUID &uu)
{
    auto map = get_dimension_map();
    return &map->at(uu);
}

void Core::delete_dimension(const UUID &uu)
{
    auto map = get_dimension_map();
    map->erase(uu);
}

Keepout *Core::insert_keepout(const UUID &uu)
{
    auto map = get_keepout_map();
    auto x = map->emplace(std::make_pair(uu, uu));
    return &(x.first->second);
}

Keepout *Core::get_keepout(const UUID &uu)
{
    auto map = get_keepout_map();
    return &map->at(uu);
}

void Core::delete_keepout(const UUID &uu)
{
    auto map = get_keepout_map();
    map->erase(uu);
}

std::vector<Keepout *> Core::get_keepouts()
{
    auto *map = get_keepout_map();
    std::vector<Keepout *> r;
    if (!map)
        return r;
    for (auto &it : *map) {
        r.push_back(&it.second);
    }
    return r;
}


void Core::rebuild(bool from_undo)
{
    if (!from_undo && !reverted) {
        while (history_current + 1 != (int)history.size()) {
            history.pop_back();
        }
        assert(history_current + 1 == (int)history.size());
        history_push();
        history_current++;
    }
    s_signal_rebuilt.emit();
    signal_can_undo_redo().emit();
    reverted = false;
}

void Core::undo()
{
    if (history_current) {
        history_current--;
        history_load(history_current);
        signal_rebuilt().emit();
        signal_can_undo_redo().emit();
        set_needs_save();
    }
}

void Core::redo()
{
    if (history_current + 1 == (int)history.size())
        return;
    std::cout << "can redo" << std::endl;
    history_current++;
    history_load(history_current);
    signal_rebuilt().emit();
    signal_can_undo_redo().emit();
    set_needs_save();
}

void Core::history_clear()
{
    history.clear();
    history_current = -1;
    signal_can_undo_redo().emit();
}

bool Core::can_redo() const
{
    return history_current + 1 != (int)history.size();
}

bool Core::can_undo() const
{
    return history_current;
}

void Core::set_property_begin()
{
    if (property_transaction)
        throw std::runtime_error("transaction already in progress");
    property_transaction = true;
}

void Core::set_property_commit()
{
    if (!property_transaction)
        throw std::runtime_error("no transaction in progress");
    rebuild();
    set_needs_save(true);
    property_transaction = false;
}

bool Core::get_property_transaction() const
{
    return property_transaction;
}

json Core::get_meta()
{
    json j;
    return j;
}

void Core::set_needs_save(bool v)
{
    if (v != needs_save) {
        needs_save = v;
        s_signal_needs_save.emit(v);
    }
}

void Core::set_needs_save()
{
    set_needs_save(true);
}

bool Core::get_needs_save() const
{
    return needs_save;
}

void Core::sort_search_results(std::list<Core::SearchResult> &results, const SearchQuery &q)
{
    results.sort([this, q](const auto &a, const auto &b) {
        auto da = this->get_display_name(a.type, a.uuid, a.sheet);
        auto db = this->get_display_name(b.type, b.uuid, b.sheet);
        auto ina = !Coordf(a.location).in_range(q.area_visible.first, q.area_visible.second);
        auto inb = !Coordf(b.location).in_range(q.area_visible.first, q.area_visible.second);

        if (ina > inb)
            return false;
        else if (ina < inb)
            return true;

        if (a.type > b.type)
            return false;
        else if (a.type < b.type)
            return true;

        auto c = strcmp_natural(da, db);
        if (c > 0)
            return false;
        else if (c < 0)
            return true;

        if (a.location.x > b.location.x)
            return false;
        else if (a.location.x < b.location.x)
            return true;

        return a.location.y > b.location.y;
    });
}
ToolBase::ToolBase(Core *c, ToolID tid) : core(c), tool_id(tid)
{
}

void ToolBase::set_imp_interface(ImpInterface *i)
{
    if (imp == nullptr) {
        imp = i;
    }
}

void ToolBase::set_transient()
{
    is_transient = true;
}

ToolSettingsProxy::~ToolSettingsProxy()
{
    tool->apply_settings();
}
} // namespace horizon
