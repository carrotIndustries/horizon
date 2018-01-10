#pragma once
#include <gtkmm.h>
#include "project/project.hpp"
#include "util/editor_process.hpp"
#include "util/window_state_store.hpp"
#include <memory>
#include <zmq.hpp>

namespace horizon {

	class ProjectManagerViewCreate: public sigc::trackable {
		public:
			ProjectManagerViewCreate(const Glib::RefPtr<Gtk::Builder>& refBuilder, class ProjectManagerAppWindow *w);
			void clear();
			std::pair<bool, std::string> create();
			typedef sigc::signal<void, bool> type_signal_valid_change;
			type_signal_valid_change signal_valid_change() {return s_signal_valid_change;}
			void populate_pool_combo(const Glib::RefPtr<Gtk::Application> &app);

		private:
			ProjectManagerAppWindow *win = nullptr;
			Gtk::Entry *project_name_entry = nullptr;
			Gtk::Entry *project_description_entry = nullptr;
			Gtk::FileChooserButton *project_path_chooser = nullptr;
			Gtk::Label *project_dir_label = nullptr;
			Gtk::ComboBoxText *project_pool_combo = nullptr;
			void update();

			type_signal_valid_change s_signal_valid_change;
	};

	class ProjectManagerViewProject: public sigc::trackable {
		public:
			ProjectManagerViewProject(const Glib::RefPtr<Gtk::Builder>& refBuilder, class ProjectManagerAppWindow *w);
			Gtk::Entry *entry_project_title = nullptr;
			Gtk::Label *label_pool_name = nullptr;
			Gtk::InfoBar *info_bar = nullptr;
			Gtk::Label *info_bar_label = nullptr;

		private:
			ProjectManagerAppWindow *win = nullptr;
			Gtk::Button *button_top_schematic = nullptr;
			Gtk::Button *button_board = nullptr;
			Gtk::Button *button_part_browser= nullptr;
			Gtk::Button *button_pool_cache = nullptr;

			void handle_button_top_schematic();
			void handle_button_board();
			void handle_button_part_browser();
			void handle_button_pool_cache();


	};

	class ProjectManagerProcess {
		public:
			enum class Type {IMP_SCHEMATIC, IMP_BOARD, IMP_PADSTACK};
			ProjectManagerProcess(Type ty, const std::vector<std::string>& args, const std::vector<std::string>& env);
			Type type;
			std::unique_ptr<EditorProcess> proc=nullptr;

	};

	class ProjectManagerAppWindow : public Gtk::ApplicationWindow {
		friend class ProjectManagerViewProject;
		friend class ProjectManagerViewCreate;
		public:
			ProjectManagerAppWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder, class ProjectManagerApplication *app);
			~ProjectManagerAppWindow();

			static ProjectManagerAppWindow* create(class ProjectManagerApplication *app);

			void open_file_view(const Glib::RefPtr<Gio::File>& file);
			void spawn_imp(ProjectManagerProcess::Type type, const UUID &pool_uuid, const std::vector<std::string> &args);
			bool close_project();
		protected:
			Glib::RefPtr<Gtk::Builder> builder;
			Gtk::Stack *stack = nullptr;
			Gtk::Button *button_open = nullptr;
			Gtk::Button *button_new = nullptr;
			Gtk::Button *button_close = nullptr;
			Gtk::Button *button_cancel = nullptr;
			Gtk::Button *button_create = nullptr;
			Gtk::Button *button_save = nullptr;
			Gtk::HeaderBar *header = nullptr;
			Gtk::ListBox *recent_listbox = nullptr;
			Gtk::Label *label_gitversion = nullptr;

			std::unique_ptr<Project> project= nullptr;
			std::string project_filename;
			std::map<std::string, ProjectManagerProcess> processes;
			class PartBrowserWindow *part_browser_window = nullptr;
			class PoolCacheWindow *pool_cache_window = nullptr;


			enum class ViewMode {OPEN, PROJECT, CREATE};
			void set_view_mode(ViewMode mode);

			void handle_open();
			void handle_new();
			void handle_cancel();
			void handle_create();
			void handle_close();
			void handle_save();
			void handle_place_part(const UUID &uu);
			json handle_req(const json &j);
			ProjectManagerViewCreate view_create;
			ProjectManagerViewProject view_project;

			bool on_delete_event(GdkEventAny *ev) override;
			bool check_pools();
			void update_recent_items();

			zmq::socket_t sock_project;
			std::string sock_project_ep;

			sigc::connection sock_project_conn;

			WindowStateStore state_store;

	};
};




