#pragma once
#include "uuid.hpp"
#include "json.hpp"
#include "unit.hpp"
#include "symbol.hpp"
#include "block.hpp"
#include "uuid_ptr.hpp"
#include "placement.hpp"
#include "uuid_provider.hpp"
#include "pool.hpp"
#include "padstack.hpp"
#include "parameter/set.hpp"
#include <vector>
#include <map>
#include <fstream>

namespace horizon {
	using json = nlohmann::json;

	class Pad: public UUIDProvider {
		public :
			Pad(const UUID &uu, const json &, Pool &pool);
			Pad(const UUID &uu, const Padstack *ps);
			UUID uuid;
			const Padstack *pool_padstack;
			Padstack padstack;
			Placement placement;
			std::string name;
			ParameterSet parameter_set;

			uuid_ptr<Net> net = nullptr;
			UUID net_segment;

			virtual UUID get_uuid() const;
			json serialize() const;
	};

}
