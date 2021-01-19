#include "libdnf/comps/group/sack.hpp"
#include "libdnf/comps/group/sack_impl.hpp"
#include "libdnf/comps/group/group.hpp"
#include "libdnf/comps/group/group-private.hpp"
#include "libdnf/comps/group/query.hpp"
#include "libdnf/comps/comps.hpp"
#include "libdnf/comps/comps_impl.hpp"

extern "C" {
#include <solv/knownid.h>
#include <solv/pool.h>
#include <solv/solvable.h>
}

#include <map>
#include <iostream>

namespace libdnf::comps {


GroupSackWeakPtr GroupSack::get_weak_ptr() {
    return GroupSackWeakPtr(this, &p_impl->data_guard);
}


GroupSack::GroupSack(Comps & comps)
    : Sack()
    , comps{comps}
    , p_impl{new Impl()}
{}


GroupSack::~GroupSack() {}


GroupQuery GroupSack::new_query() {
    Pool * pool = comps.p_impl->get_pool();
    GroupQuery query = GroupQuery(this);
    query.debug_string = "temp value";
    std::map<std::string, std::vector<Id>> group_map;
    Id solvable_id;
    std::string groupid;

    // loop over all solvables
    FOR_POOL_SOLVABLES(solvable_id) {
        groupid = pool_lookup_str(pool, solvable_id, SOLVABLE_NAME);
        if (!group_map.count(groupid))  {
            std::vector<Id> solvable_ids;
            group_map.emplace(std::pair<std::string, std::vector<Id>>(groupid, solvable_ids));
        }
        group_map[groupid].push_back(solvable_id);
    }

    std::map<std::string, std::vector<Id>>::iterator it;
    for (it = group_map.begin(); it != group_map.end(); it++) {
        Group group = Group(& query);
        for (Id solvable_id : it->second) {
            add_solvable_id(group, solvable_id);
        }
        query.add(group);
    }
    return GroupQuery(query);
}


/*
void GroupSack::add_group(std::unique_ptr<Group> && group) {
    add_item(std::move(group));
}
*/

}  // namespace libdnf::comps
