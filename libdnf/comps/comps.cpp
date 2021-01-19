#include <libdnf/comps/comps.hpp>
#include <libdnf/comps/comps_impl.hpp>

extern "C" {
#include <solv/pool.h>
#include <solv/repo.h>
#include <solv/repo_comps.h>
}

/*
#include <libdnf/comps/group/group-private.hpp>
#include <libdnf/utils/xml.hpp>

#include "libdnf/common/sack/query.hpp"

#include "libdnf/comps/group/group.hpp"
#include "libdnf/comps/group/query.hpp"

extern "C" {
#include <solv/repodata.h>
}

#include <cstring>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <unistd.h>
*/

namespace libdnf::comps {


Comps::Comps(libdnf::Base & base) : base{base}, p_impl{new Impl()} {}


Comps::~Comps() {}

/*
void Comps::load_installed() {
    auto core = get_group_sack().new_group();
    core->set_id("core");
    core->set_name("Core");
    core->add_repo("@System");
    // TODO(dmach): load from transaction database
}
*/


void Comps::load_from_file(const std::string & path, const char * reponame) {
    Pool * pool = p_impl->get_pool();
    Repo * repo = 0;
    Id repoid;
    FOR_REPOS(repoid, repo) {
        if (!strcasecmp(repo->name, reponame)) {
            break;
        }
    }
    if (!repo) {
        repo = repo_create(pool, reponame);
    }
    FILE * xml_doc = fopen(path.c_str(), "r");
    // TODO(pkratoch): libsolv doesn't support environments yet
    repo_add_comps(repo, xml_doc, 0);
    fclose(xml_doc);
}


}  // namespace libdnf::comps

