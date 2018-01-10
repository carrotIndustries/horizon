#include "duplicate_entity.hpp"
#include "duplicate_unit.hpp"
#include "duplicate_part.hpp"
#include "duplicate_window.hpp"
#include "pool.hpp"
#include "part.hpp"
#include "unit.hpp"
#include "symbol.hpp"
#include "gtk_util.hpp"
#include "util.hpp"
#include "../part_wizard/location_entry.hpp"

namespace horizon {
	class DuplicatePackageWidget: public Gtk::Box {
		public:
			DuplicatePackageWidget(Pool *p, const UUID &pkg_uuid): Gtk::Box(Gtk::ORIENTATION_VERTICAL, 10), pool(p), pkg(pool->get_package(pkg_uuid)) {
				auto explain_label = Gtk::manage(new Gtk::Label);
				explain_label->get_style_context()->add_class("dim-label");
				explain_label->set_xalign(0);
				explain_label->set_text("The new part will reference the new package");
				explain_label->show();
				explain_label->set_margin_left(20);

				auto cb = Gtk::manage(new Gtk::CheckButton);
				auto la = Gtk::manage(new Gtk::Label);
				la->set_markup("<b>Package: "+pkg->name+"</b>");
				cb->add(*la);
				cb->show_all();
				cb->set_active(true);
				cb->signal_toggled().connect([this, cb, explain_label] {
					if(cb->get_active()) {
						explain_label->set_text("The new part will reference the new package");
					}
					else {
						explain_label->set_text("The new part will reference the existing package");
					}
					grid->set_visible(cb->get_active());
				});

				pack_start(*cb, false, false, 0);
				pack_start(*explain_label, false, false, 0);




				grid = Gtk::manage(new Gtk::Grid);
				grid->set_row_spacing(10);
				grid->set_column_spacing(10);
				grid->set_margin_left(20);
				int top = 0;

				name_entry = Gtk::manage(new Gtk::Entry);
				name_entry->set_text(pkg->name + " (Copy)");
				name_entry->set_hexpand(true);
				grid_attach_label_and_widget(grid, "Name", name_entry, top);

				location_entry = Gtk::manage(new LocationEntry);
				location_entry->set_filename(Glib::path_get_dirname(pool->get_filename(ObjectType::PACKAGE, pkg->uuid))+"-copy");
				grid_attach_label_and_widget(grid, "Filename", location_entry, top);

				grid->show_all();


				pack_start(*grid, true, true, 0);
			}

			UUID duplicate() {
				if(grid->get_visible()) {
					return DuplicatePartWidget::duplicate_package(pool, pkg->uuid, location_entry->get_filename(), name_entry->get_text());
				}
				else {
					return pkg->uuid;
				}
			}

		private:
			Pool *pool;
			const Package *pkg;
			Gtk::Entry *name_entry  = nullptr;
			class LocationEntry *location_entry = nullptr;
			Gtk::Grid *grid = nullptr;
	};

	DuplicatePartWidget::DuplicatePartWidget(Pool *p, const UUID &part_uuid,  Gtk::Box *ubox, DuplicateWindow *w): Gtk::Box(Gtk::ORIENTATION_VERTICAL, 10), pool(p), part(pool->get_part(part_uuid)), win(w) {
		auto la = Gtk::manage(new Gtk::Label);
		la->set_markup("<b>Part: "+part->get_MPN()+"</b>");
		la->set_xalign(0);
		la->show();
		pack_start(*la, false, false, 0);

		grid = Gtk::manage(new Gtk::Grid);
		grid->set_row_spacing(10);
		grid->set_column_spacing(10);
		int top = 0;

		mpn_entry = Gtk::manage(new Gtk::Entry);
		mpn_entry->set_text(part->get_MPN() + " (Copy)");
		mpn_entry->set_hexpand(true);
		grid_attach_label_and_widget(grid, "MPN", mpn_entry, top);

		manufacturer_entry = Gtk::manage(new Gtk::Entry);
		manufacturer_entry->set_text(part->get_manufacturer());
		manufacturer_entry->set_hexpand(true);
		grid_attach_label_and_widget(grid, "Manufacturer", manufacturer_entry, top);

		location_entry = Gtk::manage(new LocationEntry);
		location_entry->set_filename(DuplicateUnitWidget::insert_filename(pool->get_filename(ObjectType::PART, part->uuid), "-copy"));
		grid_attach_label_and_widget(grid, "Filename", location_entry, top);

		grid->show_all();
		grid->set_margin_bottom(10);

		pack_start(*grid, false, false, 0);

		dpw = Gtk::manage(new DuplicatePackageWidget(pool, part->package->uuid));
		pack_start(*dpw, false, false, 0);
		dpw->show();

		dew = Gtk::manage(new DuplicateEntityWidget(pool, part->entity->uuid, ubox, true));
		pack_start(*dew, false, false, 0);
		dew->show();
	}

	void DuplicatePartWidget::duplicate() {
		Part new_part(*part);
		new_part.uuid = UUID::random();
		new_part.attributes.at(Part::Attribute::MPN).second = mpn_entry->get_text();
		new_part.attributes.at(Part::Attribute::MANUFACTURER).second = manufacturer_entry->get_text();
		auto new_part_json = new_part.serialize();

		auto entity_uuid = dew->duplicate();
		new_part_json["entity"] = (std::string)entity_uuid;

		auto pkg_uuid = dpw->duplicate();
		new_part_json["package"] = (std::string)pkg_uuid;

		save_json_to_file(location_entry->get_filename(), new_part_json);

		win->duplicated = true;
		win->close();
	}

	UUID DuplicatePartWidget::duplicate_package(Pool *pool, const UUID &uu, const std::string &new_dir, const std::string &new_name) {
		auto padstack_dir = Glib::build_filename(new_dir, "padstacks");
		auto fi = Gio::File::create_for_path(padstack_dir);
		fi->make_directory_with_parents();
		Package pkg(*pool->get_package(uu));
		pkg.uuid = UUID::random();
		pkg.name = new_name;
		auto pkg_json = pkg.serialize();
		SQLite::Query q(pool->db, "SELECT uuid FROM padstacks WHERE package=?");
		q.bind(1, uu);
		while(q.step()) {
			UUID padstack_uuid(q.get<std::string>(0));
			auto padstack_filename = pool->get_filename(ObjectType::PADSTACK, padstack_uuid);
			auto padstack_basename = Glib::path_get_basename(padstack_filename);
			Padstack padstack(*pool->get_padstack(padstack_uuid));
			padstack.uuid = UUID::random();
			for(const auto &it: pkg.pads) {
				if(it.second.pool_padstack->uuid == padstack_uuid) {
					pkg_json["pads"][(std::string)it.first]["padstack"] = (std::string)padstack.uuid;
				}
			}
			save_json_to_file(Glib::build_filename(padstack_dir, padstack_basename), padstack.serialize());
		}
		std::string new_pkg_filename = Glib::build_filename(new_dir, "package.json");

		save_json_to_file(new_pkg_filename, pkg_json);
		return pkg.uuid;
	}
}


