#pragma once
#include <gtkmm.h>
#include <array>
#include <set>
#include "common.hpp"
#include "uuid.hpp"
#include "uuid_path.hpp"
#include "pool.hpp"
namespace horizon {


	class PoolBrowserDialog: public Gtk::Dialog {
		public:
		PoolBrowserDialog(Gtk::Window *parent, ObjectType type, Pool *ipool);
			class PoolBrowser *get_browser();

		private :
			Pool *pool;
			class PoolBrowser *browser = nullptr;
	};
}
