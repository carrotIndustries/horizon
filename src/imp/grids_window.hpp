#pragma once
#include <gtkmm.h>
#include "util/uuid.hpp"
#include "util/changeable.hpp"
#include "common/common.hpp"
#include <set>
#include "nlohmann/json.hpp"
#include "grid_controller.hpp"

namespace horizon {
using json = nlohmann::json;

class GridsWindow : public Gtk::Window, public Changeable {
public:
    static GridsWindow *create(Gtk::Window *p, GridController &b);
    GridsWindow(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &x, GridController &b);

    json serialize();
    void load_from_json(const json &j);

private:
    GridController &grid_controller;


    class ListColumns : public Gtk::TreeModelColumnRecord {
    public:
        ListColumns()
        {
            Gtk::TreeModelColumnRecord::add(name);
            Gtk::TreeModelColumnRecord::add(settings);
        }
        Gtk::TreeModelColumn<Glib::ustring> name;
        Gtk::TreeModelColumn<GridSettings> settings;
    };
    ListColumns list_columns;

    Glib::RefPtr<Gtk::ListStore> store;
    Gtk::TreeView *treeview = nullptr;
};

} // namespace horizon
