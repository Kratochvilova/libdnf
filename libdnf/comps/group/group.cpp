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


namespace libdnf::comps {


Group::~Group() {}


Group::Group(GroupQuery * query) : query(query->get_weak_ptr()) {}


// TODO(pkratoch): Store also packagelist
void load_group_from_solvable(Group & group, Id solvable_id, Pool * pool) {
    group.add_group_id(GroupId(solvable_id));
}


Group & Group::operator+=(const Group & rhs) {
    this->group_ids.insert(this->group_ids.begin(), rhs.group_ids.begin(), rhs.group_ids.end());
    return *this;
}


std::string solvables_lookup_str(std::vector<Solvable *> solvables, Id key) {
    for (auto solvable: solvables) {
        if (solvable_lookup_str(solvable, key)) {
            return solvable_lookup_str(solvable, key);
        }
    }
    return "";
}


std::string Group::get_groupid() const {
    return "";
    /*
    Pool * pool = query->sack->comps.p_impl->get_pool();
    

    Id solvable_id;
    FOR_POOL_SOLVABLES(solvable_id) {
        std::string solvable_name(pool_lookup_str(pool, SOLVABLE_NAME));
        return solvable_name.substr(solvable_name.find(":") + 1);
        
        
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
    }
    
    
    std::string solvable_name(solvables_lookup_str(solvables, SOLVABLE_NAME));
    return solvable_name.substr(solvable_name.find(":") + 1);*/
}


std::string Group::get_name() const {
    return "";
    /*
    return solvables_lookup_str(p_impl->get_solvables(), SOLVABLE_SUMMARY);
    */
}


std::string Group::get_description() const {
    return "";
    /*
    return solvables_lookup_str(p_impl->get_solvables(), SOLVABLE_DESCRIPTION);
    */
}


std::string Group::get_translated_name(const char * lang) const {
    return "";
    /*
    std::string translation;
    for (auto solvable: p_impl->get_solvables()) {
        if (solvable_lookup_str_lang(solvable, SOLVABLE_SUMMARY, lang, 1)) {
            translation = solvable_lookup_str_lang(solvable, SOLVABLE_SUMMARY, lang, 1);
            // Return translation only if it's different from the untranslated string.
            if (translation != solvable_lookup_str(solvable, SOLVABLE_SUMMARY)) {
                return translation;
            }
        }
    }
    // Return the untranslated name when no translation was found
    return this->get_name();
    */
}


std::string Group::get_translated_description(const char * lang) const {
    return "";
    /*
    std::string translation;
    for (auto solvable: p_impl->get_solvables()) {
        if (solvable_lookup_str_lang(solvable, SOLVABLE_DESCRIPTION, lang, 1)) {
            translation = solvable_lookup_str_lang(solvable, SOLVABLE_DESCRIPTION, lang, 1);
            // Return translation only if it's different from the untranslated string.
            if (translation != solvable_lookup_str(solvable, SOLVABLE_DESCRIPTION)) {
                return translation;
            }
        }
    }
    // Return the untranslated description when no translation was found
    return this->get_description();
    */
}


std::string Group::get_order() const {
    return "";
    /*
    return solvables_lookup_str(p_impl->get_solvables(), SOLVABLE_ORDER);
    */
}


std::string Group::get_langonly() const {
    return "";
    /*
    return solvables_lookup_str(p_impl->get_solvables(), SOLVABLE_LANGONLY);
    */
}


bool Group::get_uservisible() const {
    return true;
    /*
    return solvable_lookup_void(p_impl->get_solvables()[0], SOLVABLE_ISVISIBLE);
    */
}


bool Group::get_default() const {
    return true;
    /*
    return solvable_lookup_void(p_impl->get_solvables()[0], SOLVABLE_ISDEFAULT);
    */
}


bool Group::get_installed() const {
    return get_repos().find("@System") != get_repos().end();
}


std::set<std::string> Group::get_repos() const {
    std::set<std::string> result;
    return result;
}


}  // namespace libdnf::comps
