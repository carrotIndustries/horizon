#include "gate_editor.hpp"
#include "part_wizard.hpp"
#include "location_entry.hpp"
#include "util/util.hpp"
#include "util/str_util.hpp"

namespace horizon {

GateEditorWizard::GateEditorWizard(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &x, Gate *g,
                                   PartWizard *pa)
    : Gtk::Box(cobject), parent(pa), gate(g)
{
    x->get_widget("gate_label", gate_label);
    x->get_widget("gate_edit_symbol", edit_symbol_button);
    x->get_widget("gate_suffix", suffix_entry);
    x->get_widget("gate_unit_name", unit_name_entry);
    x->get_widget("gate_unit_name_from_mpn", unit_name_from_mpn_button);

    gate_label->set_text("Gate: " + gate->name);

    unit_name_entry->signal_changed().connect(sigc::mem_fun(parent, &PartWizard::update_can_finish));
    suffix_entry->signal_changed().connect(sigc::mem_fun(parent, &PartWizard::update_can_finish));

    {
        Gtk::Button *from_part_button;
        unit_location_entry = PartWizard::pack_location_entry(x, "gate_unit_location_box", &from_part_button);
        from_part_button->set_label("From part");
        from_part_button->signal_clicked().connect([this] {
            auto rel = get_suffixed_filename_from_part();
            unit_location_entry->set_filename(Glib::build_filename(parent->pool_base_path, "units", rel));
        });
        unit_location_entry->set_filename(Glib::build_filename(parent->pool_base_path, "units"));
        unit_location_entry->signal_changed().connect(sigc::mem_fun(parent, &PartWizard::update_can_finish));
    }

    {
        Gtk::Button *from_part_button;
        symbol_location_entry = PartWizard::pack_location_entry(x, "gate_symbol_location_box", &from_part_button);
        from_part_button->set_label("From part");
        from_part_button->signal_clicked().connect([this] {
            auto rel = get_suffixed_filename_from_part();
            symbol_location_entry->set_filename(Glib::build_filename(parent->pool_base_path, "symbols", rel));
        });
        symbol_location_entry->set_filename(Glib::build_filename(parent->pool_base_path, "symbols"));
        symbol_location_entry->signal_changed().connect(sigc::mem_fun(parent, &PartWizard::update_can_finish));
    }

    unit_name_from_mpn_button->signal_clicked().connect(
            [this] { unit_name_entry->set_text(parent->part_mpn_entry->get_text()); });
}

std::string GateEditorWizard::get_suffixed_filename_from_part()
{
    auto rel = parent->get_rel_part_filename();
    std::string suffix = suffix_entry->get_text();
    trim(suffix);
    if (suffix.size() && endswith(rel, ".json")) {
        rel.insert(rel.size() - 5, "-" + suffix);
    }
    return rel;
}


GateEditorWizard *GateEditorWizard::create(Gate *g, PartWizard *pa)
{
    GateEditorWizard *w;
    Glib::RefPtr<Gtk::Builder> x = Gtk::Builder::create();
    x->add_from_resource(
            "/net/carrotIndustries/horizon/src/pool-mgr/part_wizard/"
            "part_wizard.ui");
    std::cout << "create gate ed" << std::endl;
    x->get_widget_derived("gate_editor", w, g, pa);
    w->reference();
    return w;
}
} // namespace horizon
