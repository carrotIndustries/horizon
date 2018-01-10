#pragma once
#include <gtkmm.h>
#include <memory>
#include "uuid.hpp"
#include "part.hpp"
#include "unit.hpp"
#include "entity.hpp"
#include "symbol.hpp"
#include "package.hpp"
#include "padstack.hpp"

#include "pool.hpp"
#include "util/editor_process.hpp"
#include <zmq.hpp>

namespace horizon {

	class PoolManagerProcess: public sigc::trackable {
		public:
			enum class Type {IMP_SYMBOL, IMP_PADSTACK, IMP_PACKAGE, UNIT, ENTITY, PART};
			PoolManagerProcess(Type ty, const std::vector<std::string>& args, const std::vector<std::string>& env, class Pool *pool);
			Type type;
			std::unique_ptr<EditorProcess> proc=nullptr;
			class EditorWindow *win = nullptr;
			typedef sigc::signal<void, int, bool> type_signal_exited;
			type_signal_exited signal_exited() {return s_signal_exited;}
			void reload();
		private:
			type_signal_exited s_signal_exited;
	};

	class PoolNotebook: public Gtk::Notebook {
		friend class PoolRemoteBox;
		public:
			PoolNotebook(const std::string &bp, class PoolManagerAppWindow *aw);
			void populate();
			void spawn(PoolManagerProcess::Type type, const std::vector<std::string> &args);
			bool can_close();
			void prepare_close();
			void pool_update(std::function<void()> cb = nullptr);
			~PoolNotebook();

		private:
			const std::string base_path;
			Pool pool;
			class PoolManagerAppWindow *appwin;
			std::map<std::string, PoolManagerProcess> processes;
			std::map<ObjectType, class PoolBrowser*> browsers;
			class PartWizard *part_wizard = nullptr;
			class DuplicateWindow *duplicate_window = nullptr;

			zmq::context_t &zctx;
			zmq::socket_t sock_pool_update;
			std::string sock_pool_update_ep;
			sigc::connection sock_pool_update_conn;
			bool pool_updating = false;
			void pool_updated(bool success);
			std::string pool_update_last_file;
			unsigned int pool_update_n_files = 0;
			unsigned int pool_update_n_files_last = 0;
			std::function<void()> pool_update_done_cb = nullptr;

			void show_duplicate_window(ObjectType ty, const UUID &uu);

			void handle_create_unit();
			void handle_edit_unit(const UUID &uu);
			void handle_create_symbol_for_unit(const UUID &uu);
			void handle_create_entity_for_unit(const UUID &uu);
			void handle_duplicate_unit(const UUID &uu);

			void handle_edit_symbol(const UUID &uu);
			void handle_create_symbol();
			void handle_duplicate_symbol(const UUID &uu);

			void handle_edit_entity(const UUID &uu);
			void handle_create_entity();
			void handle_duplicate_entity(const UUID &uu);

			void handle_edit_padstack(const UUID &uu);
			void handle_create_padstack();
			void handle_duplicate_padstack(const UUID &uu);

			void handle_edit_package(const UUID &uu);
			void handle_create_package();
			void handle_create_padstack_for_package(const UUID &uu);
			void handle_duplicate_package(const UUID &uu);
			void handle_part_wizard(const UUID &uu);

			void handle_edit_part(const UUID &uu);
			void handle_create_part();
			void handle_create_part_from_part(const UUID &uu);
			void handle_duplicate_part(const UUID &uu);

			Gtk::Button *add_action_button(const std::string &label, Gtk::Box *bbox, sigc::slot0<void>);
			Gtk::Button *add_action_button(const std::string &label, Gtk::Box *bbox, class PoolBrowser *br, sigc::slot1<void, UUID>);

			void handle_delete(ObjectType ty, const UUID &uu);
			void handle_copy_path(ObjectType ty, const UUID &uu);
			void add_context_menu(class PoolBrowser *br);

			std::string remote_repo;
			class PoolRemoteBox *remote_box = nullptr;

			void go_to(ObjectType type, const UUID &uu);
	};
}
