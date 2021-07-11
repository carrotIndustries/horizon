#include "tool_window.hpp"
#include "imp/imp_interface.hpp"
#include "util/gtk_util.hpp"

namespace horizon {

ToolWindow::ToolWindow(Gtk::Window *parent, ImpInterface *intf) : interface(intf)
{
    set_transient_for(*parent);
    set_type_hint(Gdk::WINDOW_TYPE_HINT_DIALOG);
    headerbar = Gtk::manage(new Gtk::HeaderBar);
    headerbar->set_show_close_button(false);
    headerbar->show();
    set_titlebar(*headerbar);
    install_esc_to_close(*this);

    auto sg = Gtk::SizeGroup::create(Gtk::SIZE_GROUP_HORIZONTAL);

    cancel_button = Gtk::manage(new Gtk::Button("Cancel"));
    headerbar->pack_start(*cancel_button);
    cancel_button->show();
    cancel_button->signal_clicked().connect([this] { hide(); });
    sg->add_widget(*cancel_button);


    ok_button = Gtk::manage(new Gtk::Button("OK"));
    ok_button->get_style_context()->add_class("suggested-action");
    headerbar->pack_end(*ok_button);
    ok_button->show();
    sg->add_widget(*ok_button);

    ok_button->signal_clicked().connect([this] { emit_event(ToolDataWindow::Event::OK); });

    signal_key_press_event().connect([this](GdkEventKey *ev) {
        if (ev->keyval == GDK_KEY_Escape) {
            hide();
            return true;
        }
        return false;
    });


    signal_hide().connect([this] { emit_event(ToolDataWindow::Event::CLOSE); });
}

void ToolWindow::emit_event(ToolDataWindow::Event ev)
{
    auto data = std::make_unique<ToolDataWindow>();
    data->event = ev;
    interface->tool_update_data(std::move(data));
}

void ToolWindow::set_title(const std::string &title)
{
    headerbar->set_title(title);
}

void ToolWindow::set_use_ok(bool okay)
{
    cancel_button->set_visible(okay);
    ok_button->set_visible(okay);
    headerbar->set_show_close_button(!okay);
}

} // namespace horizon
