#pragma once
#include "common/common.hpp"
#include "nlohmann/json_fwd.hpp"
#include <string>
#include <vector>

namespace horizon {
using json = nlohmann::json;
void save_json_to_file(const std::string &filename, const json &j);
json load_json_from_file(const std::string &filename);
int orientation_to_angle(Orientation o);
std::string get_exe_dir();
void allow_set_foreground_window(int pid);
std::string coord_to_string(const Coordf &c, bool delta = false);
std::string dim_to_string(int64_t x, bool with_sign = true);
std::string angle_to_string(int angle, bool pos_only = true);

int64_t round_multiple(int64_t x, int64_t mul);

template <typename T, typename U> std::vector<T> dynamic_cast_vector(const std::vector<U> &cin)
{
    std::vector<T> out;
    out.reserve(cin.size());
    std::transform(cin.begin(), cin.end(), std::back_inserter(out), [](auto x) { return dynamic_cast<T>(x); });
    return out;
}

template <typename Map, typename F> static void map_erase_if(Map &m, F pred)
{
    for (typename Map::iterator i = m.begin(); (i = std::find_if(i, m.end(), pred)) != m.end(); m.erase(i++))
        ;
}

bool endswith(const std::string &haystack, const std::string &needle);

template <typename T> int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

int strcmp_natural(const std::string &a, const std::string &b);
void create_config_dir();
std::string get_config_dir();

void replace_backslash(std::string &path);
json json_from_resource(const std::string &rsrc);
/* Create string objects from name template. Empty fields and names are ignored. */
json json_from_fields(const std::vector<std::string> &field, const std::vector<std::string> &name);
} // namespace horizon
