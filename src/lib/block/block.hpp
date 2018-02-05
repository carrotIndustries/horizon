#pragma once
#include "bus.hpp"
#include "component.hpp"
#include "json.hpp"
#include "net.hpp"
#include "net_class.hpp"
#include "pool/pool.hpp"
#include "util/uuid.hpp"
#include <fstream>
#include <map>
#include <set>
#include <vector>

namespace horizon {
using json = nlohmann::json;

/**
 * A block is one level of hierarchy in the netlist.
 * Right now, horizon doesn't support hierarchical designs, but provisions have
 * been made
 * where necessary.
 *
 * A block stores Components (instances of Entities), Buses and Nets.
 */
class Block {
public:
    Block(const UUID &uu, const json &, Pool &pool);
    Block(const UUID &uu);
    static Block new_from_file(const std::string &filename, Pool &pool);
    Net *get_net(const UUID &uu);
    UUID uuid;
    std::string name;
    std::map<UUID, Net> nets;
    std::map<UUID, Bus> buses;
    std::map<UUID, Component> components;
    std::map<UUID, NetClass> net_classes;
    uuid_ptr<NetClass> net_class_default = nullptr;

    Block(const Block &block);
    void operator=(const Block &block);

    void merge_nets(Net *net, Net *into);

    /**
     * deletes unreferenced nets
     */
    void vacuum_nets();

    /**
     * Takes pins specified by pins and moves them over to net
     * \param pins UUIDPath of component, gate and pin UUID
     * \param net the destination Net or nullptr for new Net
     */
    Net *extract_pins(const std::set<UUIDPath<3>> &pins, Net *net = nullptr);

    void update_connection_count();

    void update_diffpairs();

    /**
     * creates new net
     * \returns pointer to new Net
     */
    Net *insert_net();

    json serialize();

private:
    void update_refs();
};
} // namespace horizon
