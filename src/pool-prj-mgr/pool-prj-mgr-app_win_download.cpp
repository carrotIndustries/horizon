#include "pool-prj-mgr-app_win.hpp"
#include "util/github_client.hpp"
#include "pool-update/pool-update.hpp"
#include <thread>
#include <git2.h>
#include <git2/clone.h>
#include "pool/pool.hpp"

namespace horizon {

void PoolProjectManagerAppWindow::handle_do_download()
{

    std::string dest_dir = download_dest_dir_button->get_filename();
    if (dest_dir.size()) {
        button_cancel->set_sensitive(false);
        button_do_download->set_sensitive(false);
        download_error = false;
        download_revealer->set_reveal_child(true);
        download_spinner->start();
        downloading = true;
        std::thread dl_thread(&PoolProjectManagerAppWindow::download_thread, this,
                              download_gh_username_entry->get_text(), download_gh_repo_entry->get_text(), dest_dir);
        dl_thread.detach();
    }
    else {
        Gtk::MessageDialog md(*this, "Destination directory needs to be set", false /* use_markup */,
                              Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
        md.run();
    }
}

void PoolProjectManagerAppWindow::download_thread(std::string gh_username, std::string gh_repo, std::string dest_dir)
{
    try {

        {
            Glib::Dir dd(dest_dir);
            for (const auto &it : dd) {
                (void)sizeof it;
                throw std::runtime_error("destination dir is not empty");
            }
        }

        {
            std::lock_guard<std::mutex> lock(download_mutex);
            download_status = "Fetching clone URL...";
        }
        download_dispatcher.emit();

        GitHubClient client;
        json repo = client.get_repo(gh_username, gh_repo);
        std::string clone_url = repo.at("clone_url");

        git_repository *cloned_repo = NULL;
        git_clone_options clone_opts = GIT_CLONE_OPTIONS_INIT;
        git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;

        checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;
        clone_opts.checkout_opts = checkout_opts;

        {
            std::lock_guard<std::mutex> lock(download_mutex);
            download_status = "Cloning repository...";
        }
        download_dispatcher.emit();

        auto remote_dir = Glib::build_filename(dest_dir, ".remote");
        Gio::File::create_for_path(remote_dir)->make_directory_with_parents();
        int error = git_clone(&cloned_repo, clone_url.c_str(), remote_dir.c_str(), &clone_opts);

        if (error != 0) {
            auto gerr = giterr_last();
            throw std::runtime_error("git error " + std::to_string(gerr->klass) + " " + std::string(gerr->message));
        }

        {
            std::lock_guard<std::mutex> lock(download_mutex);
            download_status = "Updating remote pool...";
        }
        download_dispatcher.emit();

        pool_update(remote_dir);
        Pool pool_remote(remote_dir);

        {
            std::lock_guard<std::mutex> lock(download_mutex);
            download_status = "Copying...";
        }
        download_dispatcher.emit();

        {
            SQLite::Query q(pool_remote.db, "SELECT filename FROM all_items_view");
            while (q.step()) {
                std::string filename = q.get<std::string>(0);
                auto dirname = Glib::build_filename(dest_dir, Glib::path_get_dirname(filename));
                if (!Glib::file_test(dirname, Glib::FILE_TEST_IS_DIR)) {
                    Gio::File::create_for_path(dirname)->make_directory_with_parents();
                }
                Gio::File::create_for_path(Glib::build_filename(remote_dir, filename))
                        ->copy(Gio::File::create_for_path(Glib::build_filename(dest_dir, filename)));
            }
        }
        {
            SQLite::Query q(pool_remote.db, "SELECT DISTINCT model_filename FROM models");
            while (q.step()) {
                std::string filename = q.get<std::string>(0);
                auto remote_filename = Glib::build_filename(remote_dir, filename);
                if (Glib::file_test(remote_filename, Glib::FILE_TEST_IS_REGULAR)) {
                    auto dirname = Glib::build_filename(dest_dir, Glib::path_get_dirname(filename));
                    if (!Glib::file_test(dirname, Glib::FILE_TEST_IS_DIR)) {
                        Gio::File::create_for_path(dirname)->make_directory_with_parents();
                    }
                    Gio::File::create_for_path(remote_filename)
                            ->copy(Gio::File::create_for_path(Glib::build_filename(dest_dir, filename)));
                }
            }
        }

        Gio::File::create_for_path(Glib::build_filename(remote_dir, "pool.json"))
                ->copy(Gio::File::create_for_path(Glib::build_filename(dest_dir, "pool.json")));

        {
            std::lock_guard<std::mutex> lock(download_mutex);
            download_status = "Updating local pool...";
        }
        download_dispatcher.emit();

        pool_update(dest_dir);

        git_repository_free(cloned_repo);


        {
            std::lock_guard<std::mutex> lock(download_mutex);
            download_error = false;
            downloading = false;
        }
        download_dispatcher.emit();
    }
    catch (const std::exception &e) {
        {
            std::lock_guard<std::mutex> lock(download_mutex);
            downloading = false;
            download_error = true;
            download_status = "Error: " + std::string(e.what());
        }
        download_dispatcher.emit();
    }
    catch (const Gio::Error &e) {
        {
            std::lock_guard<std::mutex> lock(download_mutex);
            downloading = false;
            download_error = true;
            download_status = "Error: " + std::string(e.what());
        }
        download_dispatcher.emit();
    }
}

} // namespace horizon
