#pragma once
#include "block/component.hpp"
#include "core.hpp"
#include "imp-util/imp_interface.hpp"
#include <forward_list>

namespace horizon {

class ToolManageBuses : public ToolBase {
public:
    ToolManageBuses(Core *c, ToolID tid);
    ToolResponse begin(const ToolArgs &args) override;
    ToolResponse update(const ToolArgs &args) override;
    bool can_begin() override;

private:
};
} // namespace horizon
