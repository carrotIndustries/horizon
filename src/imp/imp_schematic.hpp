#pragma once
#include "imp.hpp"

namespace horizon {
class ImpSchematic : public ImpBase {
    friend class ImpInterface;

public:
    ImpSchematic(const std::string &schematic_filename, const std::string &block_filename, const PoolParams &params);
    void update_highlights() override;

protected:
    void construct() override;
    bool handle_broadcast(const json &j) override;
    void handle_maybe_drag() override;
    void update_action_sensitivity() override;

    ActionCatalogItem::Availability get_editor_type_for_action() const override
    {
        return ActionCatalogItem::AVAILABLE_IN_SCHEMATIC;
    };
    ObjectType get_editor_type() const override
    {
        return ObjectType::SCHEMATIC;
    }

    std::string get_hud_text(std::set<SelectableRef> &sel) override;

private:
    void canvas_update() override;
    CoreSchematic core_schematic;
    int handle_ask_net_merge(class Net *net, class Net *into);
    int handle_ask_delete_component(class Component *comp);
    void handle_select_sheet(Sheet *sh);
    void handle_remove_sheet(Sheet *sh);
    void handle_core_rebuilt();
    void handle_tool_change(ToolID id) override;
    void handle_export_pdf();
    void handle_move_to_other_sheet(const ActionConnection &conn);
    void handle_highlight_group_tag(const ActionConnection &conn);
    std::string last_pdf_filename;
    UUID part_from_project_manager;

    std::map<UUID, std::pair<float, Coordf>> sheet_views;
    class SheetBox *sheet_box;
    void handle_selection_cross_probe();
    bool cross_probing_enabled = false;

    Coordf cursor_pos_drag_begin;
    Target target_drag_begin;

    class BOMExportWindow *bom_export_window;

    void handle_drag();

    int get_board_pid();
};
} // namespace horizon
