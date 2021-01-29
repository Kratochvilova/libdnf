#include <libdnf/comps/comps.hpp>
#include <libdnf/comps/comps_impl.hpp>

extern "C" {
#include <solv/pool.h>
#include <solv/repo.h>
#include <solv/repo_comps.h>
}


namespace libdnf::comps {


Comps::Comps(libdnf::Base & base) : base{base}, p_impl{new Impl()} {}


Comps::~Comps() {}


void Comps::load_from_file(const std::string & path, const char * reponame) {
    Pool * pool = p_impl->get_pool();
    Repo * repo = 0;
    Id repoid;
    // Search for repo named reponame
    FOR_REPOS(repoid, repo) {
        if (!strcasecmp(repo->name, reponame)) {
            break;
        }
    }
    // If repo named reponame doesn't exist, create new repo
    if (!repo) {
        repo = repo_create(pool, reponame);
    }

    FILE * xml_doc = fopen(path.c_str(), "r");
    // Load comps from the file
    // TODO(pkratoch): libsolv doesn't support environments yet
    repo_add_comps(repo, xml_doc, 0);
    fclose(xml_doc);
}


}  // namespace libdnf::comps

