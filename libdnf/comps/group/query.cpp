#include "libdnf/comps/group/query.hpp"
#include "libdnf/comps/group/query_impl.hpp"
#include "libdnf/comps/group/sack.hpp"

extern "C" {
#include <solv/pool.h>
#include <libdnf/comps/comps.hpp>
#include <libdnf/comps/comps_impl.hpp>
}

#include <iostream>

namespace libdnf::comps {


GroupQueryWeakPtr GroupQuery::get_weak_ptr() {
    return GroupQueryWeakPtr(this, &p_impl->data_guard);
}

GroupQuery::GroupQuery(GroupSack * sack)
    : Query()
    , sack(sack->get_weak_ptr())
    , p_impl{new Impl()}
    , debug_string("constructor")
{
}
    
    /*
    Id solvable_id;
    // loop over all solvables
    FOR_POOL_SOLVABLES(solvable_id) {
        auto group = std::make_unique<Group>();

        load_group_from_solvable(*group, solvable_id, pool);

        // query sack for available (not installed) groups with given id
        auto q = get_group_sack().new_query();
        q.ifilter_installed(false);
        q.ifilter_id(libdnf::sack::QueryCmp::EQ, group->get_id());

        if (q.empty()) {
            // move the newly created group to the sack
            get_group_sack().add_group(std::move(group));
        } else {
            // update an existing group
            auto existing_group = q.get();
            *existing_group.get() += *group;
        }
    }*/


GroupQuery::GroupQuery(const Set<Group> & src_set, GroupSack * sack)
    : Query(src_set)
    , sack(sack->get_weak_ptr())
    , p_impl{new Impl()}
    , debug_string("constructor")
{
    for (auto group : src_set.get_data()) {
        group.query = this->get_weak_ptr();
    }
}


GroupQuery::GroupQuery(Set<Group> && src_set, GroupSack * sack)
    : Query(src_set)
    , sack(sack->get_weak_ptr())
    , p_impl{new Impl()}
    , debug_string("constructor")
{
    for (auto group : src_set.get_data()) {
        group.query = this->get_weak_ptr();
    }
}


GroupQuery::GroupQuery(const GroupQuery & query)
    : Query(query)
    , sack(query.sack)
    , p_impl{new Impl()}
    , debug_string("copy constructor")
{
    for (auto group : query.get_data()) {
        std::cout << group.query->debug_string << std::endl;
        group.query = this->get_weak_ptr();
        std::cout << group.query->debug_string << std::endl;
    }
    for (auto group : get_data()) {
        std::cout << group.query->debug_string << std::endl;
        group.query = this->get_weak_ptr();
        std::cout << group.query->debug_string << std::endl;
    }
    for (auto group : get_data()) {
        std::cout << group.query->debug_string << std::endl;
        group.query = this->get_weak_ptr();
        std::cout << group.query->debug_string << std::endl;
    }
}


void GroupQuery::foo() {
    std::cout << debug_string << std::endl;
}


GroupQuery::~GroupQuery() {}


}  // namespace libdnf::comps

