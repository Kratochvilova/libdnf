#include <libdnf/comps/group/group.hpp>
#include <libdnf/comps/group/query.hpp>
#include <libdnf/comps/group/sack.hpp>
#include <libdnf/comps/comps.hpp>
#include <libdnf/comps/comps_impl.hpp>

extern "C" {
#include <solv/knownid.h>
#include <solv/pool.h>
#include <solv/solvable.h>
}

#include <string>
#include <iostream>


namespace libdnf::comps {


Group::~Group() {}


Group::Group(GroupQuery * query) : query(query->get_weak_ptr()) {}


void add_solvable_id(Group & group, Id solvable_id) {
    group.add_group_id(GroupId(solvable_id));
}


Group & Group::operator+=(const Group & rhs) {
    this->group_ids.insert(this->group_ids.begin(), rhs.group_ids.begin(), rhs.group_ids.end());
    return *this;
}


std::string lookup_str(Pool * pool, std::vector<GroupId> group_ids, Id key) {
    for (GroupId group_id: group_ids) {
        if (pool_lookup_str(pool, group_id.id, key)) {
            return pool_lookup_str(pool, group_id.id, key);
        }
    }
    return "";
}


std::string Group::foo() const {
    std::cout << query->debug_string << std::endl;
    Pool * pool = query->sack->comps.p_impl->get_pool();
    return pool_lookup_str(pool, 2, SOLVABLE_NAME);
}


std::string Group::get_groupid() const {
    Pool * pool = query->sack->comps.p_impl->get_pool();
    return pool_lookup_str(pool, 2, SOLVABLE_NAME);
    /*
    std::string solvable_name(
        lookup_str(query->sack->comps.p_impl->get_pool(), group_ids, SOLVABLE_NAME));
    if (solvable_name.find(":") == std::string::npos) {
        return "";
    }
    return solvable_name.substr(solvable_name.find(":") + 1);*/
}


std::string Group::get_name() const {
    return lookup_str(query->sack->comps.p_impl->get_pool(), group_ids, SOLVABLE_SUMMARY);
}


std::string Group::get_description() const {
    return lookup_str(query->sack->comps.p_impl->get_pool(), group_ids, SOLVABLE_DESCRIPTION);
}


std::string Group::get_translated_name(const char * lang) const {
    std::string translation;
    Pool * pool = query->sack->comps.p_impl->get_pool();
    for (GroupId group_id: group_ids) {
        Solvable * solvable = pool->solvables + group_id.id;
        if (solvable_lookup_str_lang(solvable, SOLVABLE_SUMMARY, lang, 1)) {
            translation = solvable_lookup_str_lang(solvable, SOLVABLE_SUMMARY, lang, 1);
            // Return translation only if it's different from the untranslated string.
            if (translation != solvable_lookup_str(solvable, SOLVABLE_SUMMARY)) {
                return translation;
            }
        }
    }
    return this->get_name();
}


std::string Group::get_translated_description(const char * lang) const {
    std::string translation;
    Pool * pool = query->sack->comps.p_impl->get_pool();
    for (GroupId group_id: group_ids) {
        Solvable * solvable = pool->solvables + group_id.id;
        if (solvable_lookup_str_lang(solvable, SOLVABLE_DESCRIPTION, lang, 1)) {
            translation = solvable_lookup_str_lang(solvable, SOLVABLE_DESCRIPTION, lang, 1);
            // Return translation only if it's different from the untranslated string.
            if (translation != solvable_lookup_str(solvable, SOLVABLE_DESCRIPTION)) {
                return translation;
            }
        }
    }
    return this->get_description();
}


std::string Group::get_order() const {
    return lookup_str(query->sack->comps.p_impl->get_pool(), group_ids, SOLVABLE_ORDER);
}


std::string Group::get_langonly() const {
    return lookup_str(query->sack->comps.p_impl->get_pool(), group_ids, SOLVABLE_LANGONLY);
}


bool Group::get_uservisible() const {
    Pool * pool = query->sack->comps.p_impl->get_pool();
    return pool_lookup_void(pool, group_ids[0].id, SOLVABLE_ISVISIBLE);
}


bool Group::get_default() const {
    Pool * pool = query->sack->comps.p_impl->get_pool();
    return pool_lookup_void(pool, group_ids[0].id, SOLVABLE_ISDEFAULT);
}


bool Group::get_installed() const {
    return get_repos().find("@System") != get_repos().end();
}


std::set<std::string> Group::get_repos() const {
    std::set<std::string> result;
    return result;
}


}  // namespace libdnf::comps
