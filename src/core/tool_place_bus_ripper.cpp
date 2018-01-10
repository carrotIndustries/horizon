#include "tool_place_bus_ripper.hpp"
#include <iostream>
#include "core_schematic.hpp"
#include "imp_interface.hpp"
#include "tool_helper_move.hpp"

namespace horizon {

	ToolPlaceBusRipper::ToolPlaceBusRipper(Core *c, ToolID tid): ToolPlaceJunction(c, tid) {
	}

	bool ToolPlaceBusRipper::can_begin() {
		return core.c;
	}


	bool ToolPlaceBusRipper::begin_attached() {
		std::cout << "tool place bus ripper\n";

		bool r;
		UUID bus_uuid;
		std::tie(r, bus_uuid) = imp->dialogs.select_bus(core.c->get_schematic()->block);
		if(!r) {
			return false;
		}
		bus = &core.c->get_schematic()->block->buses.at(bus_uuid);

		for(auto &it: bus->members) {
			bus_members.push_back(&it.second);
		}
		if(bus_members.size()==0)
			return false;
		std::sort(bus_members.begin(), bus_members.end(), [](auto a, auto b){return a->name < b->name;});
		imp->tool_bar_set_tip("<b>LMB:</b>place bus ripper <b>RMB:</b>delete current ripper and finish <b>e:</b>mirror");
		return true;
	}

	void ToolPlaceBusRipper::create_attached() {
		if(ri)
			ri->temp = false;
		auto uu = UUID::random();
		ri = &core.c->get_sheet()->bus_rippers.emplace(uu, uu).first->second;
		ri->bus = bus;
		ri->temp = true;
		ri->bus_member = bus_members.at(bus_member_current);
		ri->junction = temp;
		bus_member_current++;
		bus_member_current %= bus_members.size();
	}

	void ToolPlaceBusRipper::delete_attached() {
		if(ri) {
			core.c->get_sheet()->bus_rippers.erase(ri->uuid);
			temp->bus = nullptr;
			ri = nullptr;
		}
	}

	bool ToolPlaceBusRipper::check_line(LineNet *li) {
		if(li->net)
			return false;
		return li->bus == bus;
	}

	bool ToolPlaceBusRipper::update_attached(const ToolArgs &args) {
		if(args.type == ToolEventType::CLICK) {
			if(args.button == 1) {
				if(args.target.type == ObjectType::JUNCTION) {
					Junction *j = core.r->get_junction(args.target.path.at(0));
					if(j->bus != bus) {
						imp->tool_bar_flash("junction connected to wrong bus");
						return true;
					}
					ri->junction = j;
					create_attached();
				}
				else {
					for(auto it: core.c->get_net_lines()) {
						if(it->coord_on_line(temp->position)) {
							std::cout << "on line" << std::endl;
							if(it->bus == bus) {
								core.c->get_sheet()->split_line_net(it, temp);
								temp->temp = false;
								junctions_placed.push_front(temp);
								create_junction(args.coords);
								create_attached();
								return true;
							}
							else {
								imp->tool_bar_flash("line connected to wrong bus");
								return true;
							}
						}
					}
					imp->tool_bar_flash("can't place bus ripper nowhere");
				}
				return true;
			}
		}
		else if(args.type == ToolEventType::KEY) {
			if(args.key == GDK_KEY_space) {
				bool r;
				UUID bus_member_uuid;

				std::tie(r, bus_member_uuid) = imp->dialogs.select_bus_member(core.c->get_schematic()->block, bus->uuid);
				if(!r)
					return true;
				Bus::Member *bus_member = &bus->members.at(bus_member_uuid);

				auto p = std::find(bus_members.begin(), bus_members.end(), bus_member);
				bus_member_current = p-bus_members.begin();
				delete_attached();
				create_attached();

				return true;
			}
			if(args.key == GDK_KEY_e) {
				ri->mirror ^= true;
			}
		}

		return false;
	}

}
