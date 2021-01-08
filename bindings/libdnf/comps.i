%module comps

%include <exception.i>
%include <std_string.i>

#if defined(SWIGPYTHON)
%import(module="libdnf.common") "common.i"
#elif defined(SWIGRUBY)
%import(module="libdnf/common") "common.i"
#elif defined(SWIGPERL)
%include "std_vector.i"
%import(module="libdnf::common") "common.i"
#endif

%{
    #include "libdnf/comps/comps.hpp"
    #include "libdnf/comps/group/group.hpp"
    #include "libdnf/comps/group/query.hpp"
    #include "libdnf/comps/group/sack.hpp"
    using namespace libdnf::comps;
%}

#define CV __perl_CV

%template(SackQueryGroup) libdnf::sack::Query<libdnf::comps::Group>;

%template(SackQueryGroupWeakPtr) libdnf::WeakPtr<GroupQuery, false>;
%template(SackQueryGroupWeakPtrSet) libdnf::Set<libdnf::comps::GroupQueryWeakPtr>;

%template(SackGroupGroupQuery) libdnf::sack::Sack<libdnf::comps::Group, libdnf::comps::GroupQuery>;

%template(SackGroupGroupQueryWeakPtr) libdnf::WeakPtr<GroupSack, false>;
%template(SackGroupGroupQueryWeakPtrSet) libdnf::Set<libdnf::comps::GroupSackWeakPtr>;

%include "libdnf/comps/comps.hpp"
%include "libdnf/comps/group/group.hpp"
%include "libdnf/comps/group/query.hpp"
%include "libdnf/comps/group/sack.hpp"
