#include "tool_draw_line_rectangle.hpp"
#include "imp_interface.hpp"
#include "polygon.hpp"
#include <iostream>

namespace horizon {

	ToolDrawLineRectangle::ToolDrawLineRectangle(Core *c, ToolID tid):ToolBase(c, tid) {
	}

	bool ToolDrawLineRectangle::can_begin() {
		return core.r->has_object_type(ObjectType::LINE);
	}

	void ToolDrawLineRectangle::update_junctions() {
		if(step == 1) {
			Coordi p0t, p1t;
			if(mode == Mode::CORNER) {
				p0t = first_pos;
				p1t = second_pos;
			}
			else {
				auto &center = first_pos;
				auto a = second_pos-center;
				p0t = center-a;
				p1t = second_pos;
			}
			Coordi p0 = Coordi::min(p0t, p1t);
			Coordi p1 = Coordi::max(p0t, p1t);
			junctions[0]->position = p0;
			junctions[1]->position = {p0.x, p1.y};
			junctions[2]->position = p1;
			junctions[3]->position = {p1.x, p0.y};
		}
	}

	ToolResponse ToolDrawLineRectangle::begin(const ToolArgs &args) {
		std::cout << "tool draw line\n";


		for(int i=0; i<4; i++) {
			junctions[i] = core.r->insert_junction(UUID::random());
			junctions[i]->temp = true;
		}

		for(int i=0; i<4; i++) {
			auto line = core.r->insert_line(UUID::random());
			lines.insert(line);
			line->layer = args.work_layer;
			line->from = junctions[i];
			line->to = junctions[(i+1)%4];
		}
		first_pos = args.coords;

		update_tip();
		return ToolResponse();
	}

	void ToolDrawLineRectangle::update_tip() {
		std::stringstream ss;
		ss << "<b>LMB:</b>";
		if(mode == Mode::CENTER) {
			if(step == 0) {
				ss << "place center";
			}
			else {
				ss << "place corner";
			}
		}
		else {
			if(step == 0) {
				ss << "place first corner";
			}
			else {
				ss << "place second corner";
			}
		}
		ss << " <b>RMB:</b>cancel";
		ss << " <b>c:</b>switch mode";

		ss << " <i>";
		if(mode == Mode::CENTER) {
			ss << "from center";
		}
		else {
			ss << "corners";
		}
		ss << " </i>";

		imp->tool_bar_set_tip(ss.str());
	}

	ToolResponse ToolDrawLineRectangle::update(const ToolArgs &args) {
		if(args.type == ToolEventType::MOVE) {
			if(step == 0) {
				first_pos = args.coords;
			}
			else if(step == 1) {
				second_pos = args.coords;
				update_junctions();
			}
		}
		else if(args.type == ToolEventType::CLICK) {
			if(args.button == 1) {
				if(step==0) {
					step=1;
				}
				else {
					for(int i=0; i<4; i++) {
						junctions[i]->temp = false;
					}
					core.r->commit();
					return ToolResponse::end();
				}
			}
			else if(args.button == 3) {
				core.r->revert();
				return ToolResponse::end();
			}
		}
		else if(args.type == ToolEventType::KEY) {
			if(args.key == GDK_KEY_c) {
				mode = mode==Mode::CENTER?Mode::CORNER:Mode::CENTER;
				update_junctions();
			}
			else if(args.key == GDK_KEY_Escape) {
				core.r->revert();
				return ToolResponse::end();
			}
		}
		else if(args.type == ToolEventType::LAYER_CHANGE) {
			for(auto it: lines) {
				it->layer = args.work_layer;
			}
		}
		update_tip();
		return ToolResponse();
	}

}
