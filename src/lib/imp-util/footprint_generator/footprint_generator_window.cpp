#include "footprint_generator_window.hpp"
#include "footprint_generator_base.hpp"
#include "footprint_generator_dual.hpp"
#include "footprint_generator_grid.hpp"
#include "footprint_generator_quad.hpp"
#include "footprint_generator_single.hpp"
#include "widgets/spin_button_dim.hpp"
#include <pangomm/layout.h>

namespace horizon {

FootprintGeneratorWindow::FootprintGeneratorWindow(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder> &x)
    : Gtk::Window(cobject)
{
}

void FootprintGeneratorWindow::update_can_generate()
{
    auto w = dynamic_cast<FootprintGeneratorBase *>(stack->get_visible_child());
    if (w)
        generate_button->set_sensitive(w->property_can_generate());
    else
        generate_button->set_sensitive(false);
}

FootprintGeneratorWindow *FootprintGeneratorWindow::create(Gtk::Window *p, CorePackage *c)
{
    FootprintGeneratorWindow *w;
    Glib::RefPtr<Gtk::Builder> x = Gtk::Builder::create();
    x->add_from_resource(
            "/net/carrotIndustries/horizon/src/lib/imp-util/footprint_generator/"
            "footprint_generator.ui");
    x->get_widget_derived("window", w);
    x->get_widget("stack", w->stack);
    x->get_widget("generate_button", w->generate_button);
    w->set_transient_for(*p);
    w->core = c;

    {
        auto gen = Gtk::manage(new FootprintGeneratorDual(c));
        gen->show();
        gen->property_can_generate().signal_changed().connect(
                sigc::mem_fun(w, &FootprintGeneratorWindow::update_can_generate));
        w->stack->add(*gen, "dual", "Dual");
    }
    {
        auto gen = Gtk::manage(new FootprintGeneratorSingle(c));
        gen->show();
        gen->property_can_generate().signal_changed().connect(
                sigc::mem_fun(w, &FootprintGeneratorWindow::update_can_generate));
        w->stack->add(*gen, "single", "Single");
    }
    {
        auto gen = Gtk::manage(new FootprintGeneratorQuad(c));
        gen->show();
        gen->property_can_generate().signal_changed().connect(
                sigc::mem_fun(w, &FootprintGeneratorWindow::update_can_generate));
        w->stack->add(*gen, "quad", "Quad");
    }
    {
        auto gen = Gtk::manage(new FootprintGeneratorGrid(c));
        gen->show();
        gen->property_can_generate().signal_changed().connect(
                sigc::mem_fun(w, &FootprintGeneratorWindow::update_can_generate));
        w->stack->add(*gen, "grid", "Grid");
    }
    w->stack->property_visible_child().signal_changed().connect(
            sigc::mem_fun(w, &FootprintGeneratorWindow::update_can_generate));
    w->generate_button->signal_clicked().connect([w] {
        auto gen = dynamic_cast<FootprintGeneratorBase *>(w->stack->get_visible_child());
        if (gen) {
            if (gen->generate()) {
                w->core->rebuild();
                w->signal_generated().emit();
                w->hide();
            }
        }
    });
    return w;
}
} // namespace horizon
