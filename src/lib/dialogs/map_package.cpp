#include "map_package.hpp"
#include "pool/part.hpp"
#include <iostream>

namespace horizon {

void MapPackageDialog::ok_clicked()
{
    auto it = view->get_selection()->get_selected();
    if (it) {
        Gtk::TreeModel::Row row = *it;
        std::cout << row[list_columns.name] << std::endl;
        selection_valid = true;
        selected_uuid = row[list_columns.uuid];
    }
}

void MapPackageDialog::row_activated(const Gtk::TreeModel::Path &path, Gtk::TreeViewColumn *column)
{
    auto it = store->get_iter(path);
    if (it) {
        Gtk::TreeModel::Row row = *it;
        std::cout << "act " << row[list_columns.name] << std::endl;
        selection_valid = true;
        selected_uuid = row[list_columns.uuid];
        response(Gtk::ResponseType::RESPONSE_OK);
    }
}

MapPackageDialog::MapPackageDialog(Gtk::Window *parent, const std::vector<std::pair<Component *, bool>> &components)
    : Gtk::Dialog("Map Package", *parent, Gtk::DialogFlags::DIALOG_MODAL | Gtk::DialogFlags::DIALOG_USE_HEADER_BAR)
{
    Gtk::Button *button_ok = add_button("OK", Gtk::ResponseType::RESPONSE_OK);
    add_button("Cancel", Gtk::ResponseType::RESPONSE_CANCEL);
    set_default_response(Gtk::ResponseType::RESPONSE_OK);
    set_default_size(300, 300);

    button_ok->signal_clicked().connect(sigc::mem_fun(this, &MapPackageDialog::ok_clicked));

    store = Gtk::ListStore::create(list_columns);
    Gtk::TreeModel::Row row;
    for (auto it : components) {
        if (it.second == false) {
            row = *(store->append());
            row[list_columns.uuid] = it.first->uuid;
            row[list_columns.name] = it.first->refdes;
            row[list_columns.package] = it.first->part->package->name;
        }
    }

    view = Gtk::manage(new Gtk::TreeView(store));
    view->append_column("Component", list_columns.name);
    view->append_column("Package", list_columns.package);
    view->get_selection()->set_mode(Gtk::SelectionMode::SELECTION_BROWSE);
    view->signal_row_activated().connect(sigc::mem_fun(this, &MapPackageDialog::row_activated));


    auto sc = Gtk::manage(new Gtk::ScrolledWindow());
    sc->add(*view);
    get_content_area()->pack_start(*sc, true, true, 0);
    get_content_area()->set_border_width(0);

    show_all();
}
} // namespace horizon
