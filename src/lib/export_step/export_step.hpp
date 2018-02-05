#pragma once
#include <string>
#include <functional>

namespace horizon {
void export_step(const std::string &filename, const class Board &brd, class Pool &pool, bool include_models,
                 std::function<void(std::string)> progress_cb);
}
