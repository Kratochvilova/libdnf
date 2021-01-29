#if defined(SWIGPYTHON)
%module(package="libdnf") comps
#elif defined(SWIGPERL)
%module "libdnf::comps"
#elif defined(SWIGRUBY)
%module "libdnf/comps"
#endif

%include <exception.i>
%include <std_string.i>
%include <std_vector.i>

%import "common.i"

%{
    #include "libdnf/comps/group/package.hpp"
    #include "libdnf/comps/group/group.hpp"
    #include "libdnf/comps/group/query.hpp"
    #include "libdnf/comps/group/sack.hpp"
    #include "libdnf/comps/comps.hpp"
%}

#define CV __perl_CV

// %template(SackQueryGroup) libdnf::sack::Query<libdnf::comps::Group>;

// %template(SackQueryGroupWeakPtr) libdnf::WeakPtr<GroupQuery, false>;
// %template(SackQueryGroupWeakPtrSet) libdnf::Set<libdnf::comps::GroupQueryWeakPtr>;

// %template(SackGroupGroupQuery) libdnf::sack::Sack<libdnf::comps::Group, libdnf::comps::GroupQuery>;

// %template(SackGroupGroupQueryWeakPtr) libdnf::WeakPtr<GroupSack, false>;
// %template(SackGroupGroupQueryWeakPtrSet) libdnf::Set<libdnf::comps::GroupSackWeakPtr>;

%include "libdnf/comps/group/package.hpp"
%include "libdnf/comps/group/group.hpp"
%include "libdnf/comps/group/query.hpp"
%include "libdnf/comps/group/sack.hpp"
%include "libdnf/comps/comps.hpp"
