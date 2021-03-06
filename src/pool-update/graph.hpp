#pragma once
#include "util/uuid.hpp"
#include <map>
#include <set>
#include <string>
#include <vector>

namespace horizon {

class PoolUpdateNode {
public:
    PoolUpdateNode(const UUID &uu, const std::string &filename, const std::set<UUID> &dependencies);

    const UUID uuid;
    const std::string filename;

    std::set<UUID> dependencies;
    std::set<class PoolUpdateNode *> dependants;
};

std::set<UUID> uuids_from_missing(const std::set<std::pair<const PoolUpdateNode *, UUID>> &missing);

class PoolUpdateGraph {
public:
    PoolUpdateGraph();
    void add_node(const UUID &uu, const std::string &filename, const std::set<UUID> &dependencies);
    void dump(const std::string &filename);
    std::set<std::pair<const PoolUpdateNode *, UUID>> update_dependants();
    std::set<const PoolUpdateNode *> get_not_visited(const std::set<UUID> &visited);

    const PoolUpdateNode &get_root() const;

private:
    std::map<UUID, PoolUpdateNode> nodes;
    PoolUpdateNode root_node;
};
} // namespace horizon
