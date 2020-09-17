/*
Copyright (C) 2019-2020 Red Hat, Inc.

This file is part of microdnf: https://github.com/rpm-software-management/libdnf/

Microdnf is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Microdnf is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with microdnf.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "grouplist.hpp"

#include "../../context.hpp"

#include <libdnf/comps/comps.hpp>
#include <libdnf/comps/group/group.hpp>
#include <libdnf/comps/group/sack.hpp>
#include <libdnf/comps/group/query.hpp>
#include <libdnf/conf/option_string.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <libsmartcols/libsmartcols.h>
#include <set>
#include <unistd.h>


namespace microdnf {

// group list table columns
enum { COL_GROUP_ID, COL_GROUP_NAME, COL_INSTALLED };


static struct libscols_table * create_grouplist_table() {
    struct libscols_table * table = scols_new_table();
    if (isatty(1)) {
        scols_table_enable_colors(table, 1);
        scols_table_enable_maxout(table, 1);
    }
    struct libscols_column * cl = scols_table_new_column(table, "group id", 0.4, 0);
    scols_column_set_cmpfunc(cl, scols_cmpstr_cells, NULL);
    scols_table_new_column(table, "group name", 0.5, SCOLS_FL_TRUNC);
    scols_table_new_column(table, "installed", 0.1, SCOLS_FL_RIGHT);
    return table;
}


static void add_line_into_table(
    struct libscols_table * table, const char * id, const char * name, bool installed) {
    struct libscols_line * ln = scols_table_new_line(table, NULL);
    scols_line_set_data(ln, COL_GROUP_ID, id);
    scols_line_set_data(ln, COL_GROUP_NAME, name);
    scols_line_set_data(ln, COL_INSTALLED, installed ? "yes" : "no");
    if (installed) {
        struct libscols_cell * cl = scols_line_get_cell(ln, COL_INSTALLED);
        scols_cell_set_color(cl, "green");
    }
}


using namespace libdnf::cli;
void CmdGrouplist::set_argument_parser(Context & ctx) {
    available_option = dynamic_cast<libdnf::OptionBool *>(
        ctx.arg_parser.add_init_value(std::unique_ptr<libdnf::OptionBool>(new libdnf::OptionBool(false))));

    installed_option = dynamic_cast<libdnf::OptionBool *>(
        ctx.arg_parser.add_init_value(std::unique_ptr<libdnf::OptionBool>(new libdnf::OptionBool(false))));

    hidden_option = dynamic_cast<libdnf::OptionBool *>(
        ctx.arg_parser.add_init_value(std::unique_ptr<libdnf::OptionBool>(new libdnf::OptionBool(false))));

    auto available = ctx.arg_parser.add_new_named_arg("available");
    available->set_long_name("available");
    available->set_short_description("show only available groups");
    available->set_const_value("true");
    available->link_value(available_option);

    auto installed = ctx.arg_parser.add_new_named_arg("installed");
    installed->set_long_name("installed");
    installed->set_short_description("show only installed groups");
    installed->set_const_value("true");
    installed->link_value(installed_option);

    auto hidden = ctx.arg_parser.add_new_named_arg("hidden");
    hidden->set_long_name("hidden");
    hidden->set_short_description("show also hidden groups");
    hidden->set_const_value("true");
    hidden->link_value(hidden_option);

    patterns_to_show_options = ctx.arg_parser.add_new_values();
    auto keys = ctx.arg_parser.add_new_positional_arg(
        "groups_to_show",
        ArgumentParser::PositionalArg::UNLIMITED,
        ctx.arg_parser.add_init_value(std::unique_ptr<libdnf::Option>(new libdnf::OptionString(nullptr))),
        patterns_to_show_options);
    keys->set_short_description("List of groups to show");

    auto conflict_args =
        ctx.arg_parser.add_conflict_args_group(std::unique_ptr<std::vector<ArgumentParser::Argument *>>(
            new std::vector<ArgumentParser::Argument *>{available, installed}));

    available->set_conflict_arguments(conflict_args);
    installed->set_conflict_arguments(conflict_args);

    auto grouplist = ctx.arg_parser.add_new_command("grouplist");
    grouplist->set_short_description("display groups");
    grouplist->set_description("");
    grouplist->set_named_args_help_header("Optional arguments:");
    grouplist->set_positional_args_help_header("Positional arguments:");
    grouplist->set_parse_hook_func([this, &ctx](
                                [[maybe_unused]] ArgumentParser::Argument * arg,
                                [[maybe_unused]] const char * option,
                                [[maybe_unused]] int argc,
                                [[maybe_unused]] const char * const argv[]) {
        ctx.select_command(this);
        return true;
    });

    grouplist->register_named_arg(available);
    grouplist->register_named_arg(installed);
    grouplist->register_named_arg(hidden);
    grouplist->register_positional_arg(keys);

    ctx.arg_parser.get_root_command()->register_command(grouplist);
}


void list_groups(const std::set<libdnf::comps::GroupWeakPtr> & group_list) {
    struct libscols_table * table = create_grouplist_table();
    for (auto group_weak_ptr: group_list) {
        add_line_into_table(
            table,
            group_weak_ptr->get_id().c_str(),
            group_weak_ptr->get_name().c_str(),
            group_weak_ptr->get_installed());
    }
    auto cl = scols_table_get_column(table, COL_GROUP_ID);
    scols_sort_table(table, cl);
    scols_print_table(table);
    scols_unref_table(table);
}


void CmdGrouplist::run([[maybe_unused]] Context & ctx) {
    std::vector<std::string> patterns_to_show;
    if (patterns_to_show_options->size() > 0) {
        patterns_to_show.reserve(patterns_to_show_options->size());
        for (auto & pattern : *patterns_to_show_options) {
            patterns_to_show.emplace_back(dynamic_cast<libdnf::OptionString *>(pattern.get())->get_value());
        }
    }

    // Load group sack
    // TODO(pkratoch): use comps from base and real repositories
    libdnf::comps::Comps comps;
    comps.load_installed();
    std::filesystem::path data_path = PROJECT_SOURCE_DIR "/test/libdnf/comps/data/";
    comps.load_from_file(data_path / "core.xml");
    comps.load_from_file(data_path / "standard.xml");
    libdnf::comps::GroupSack & group_sack = comps.get_group_sack();

    libdnf::comps::GroupQuery group_query = group_sack.new_query();


    // Not hidden -> filter uservisible
    if (not hidden_option->get_value()) {
        group_query.ifilter_uservisible(true);
    }

    libdnf::comps::GroupQuery installed_query = group_query;
    installed_query.ifilter_installed(true);
    const std::set<libdnf::comps::GroupWeakPtr> & installed_groups = installed_query.list();

    // Listing only installed groups -> list them and return
    if (installed_option->get_value()) {
        list_groups(installed_groups);
        return;
    }

    libdnf::comps::GroupQuery available_query = group_query;
    available_query.ifilter_installed(false);
    const std::set<libdnf::comps::GroupWeakPtr> & available_groups = available_query.list();

    std::unordered_map<std::string, libdnf::comps::GroupWeakPtr> group_map;

    // Listing only available groups -> add available groups to a map, but remove already installed groups
    if (available_option->get_value()) {
        for (auto group: available_groups) {
            group_map.insert(std::pair<std::string, libdnf::comps::GroupWeakPtr>(group->get_id(), group));
        }
        for (auto group: installed_groups) {
            group_map.erase(group->get_id());
        }
    }

    // Listing all groups -> add to a map first installed groups and then available groups
    // If a group is both installed and available, only the installed version will be listed
    else {
        for (auto group: installed_groups) {
            group_map.insert(std::pair<std::string, libdnf::comps::GroupWeakPtr>(group->get_id(), group));
        }
        for (auto group: available_groups) {
            group_map.insert(std::pair<std::string, libdnf::comps::GroupWeakPtr>(group->get_id(), group));
        }
    }

    std::set<libdnf::comps::GroupWeakPtr> group_set;
    for (auto it: group_map) {
        group_set.insert(it.second);
    }
    list_groups(group_set);
}

}  // namespace microdnf
