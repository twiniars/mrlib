#ifndef BOOST_ARCHIVE_BASIC_BINARY_OARCHIVE_HPP
#define BOOST_ARCHIVE_BASIC_BINARY_OARCHIVE_HPP

// MS compatible compilers support #pragma once
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// basic_binary_oarchive.hpp

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

// archives stored as native binary - this should be the fastest way
// to archive the state of a group of obects.  It makes no attempt to
// convert to any canonical form.

// IN GENERAL, ARCHIVES CREATED WITH THIS CLASS WILL NOT BE READABLE
// ON PLATFORM APART FROM THE ONE THEY ARE CREATE ON

#include <cassert>
#include <boost/integer.hpp>
#include <boost/integer_traits.hpp>

#include <boost/config.hpp>
#include <boost/serialization/pfto.hpp>

#include <boost/detail/workaround.hpp>
#include <boost/archive/detail/common_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/collection_size_type.hpp>

#include <boost/archive/detail/abi_prefix.hpp> // must be the last header

#ifdef BOOST_MSVC
#  pragma warning(push)
#  pragma warning(disable : 4511 4512)
#endif

namespace boost {
namespace archive {

//////////////////////////////////////////////////////////////////////
// class basic_binary_oarchive - write serialized objects to a binary output stream
// note: this archive has no pretensions to portability.  Archive format
// may vary across machine architectures and compilers.  About the only
// guarentee is that an archive created with this code will be readable
// by a program built with the same tools for the same machne.  This class
// does have the virtue of buiding the smalles archive in the minimum amount
// of time.  So under some circumstances it may be he right choice.
template<class Archive>
class basic_binary_oarchive : 
    public archive::detail::common_oarchive<Archive>
{
protected:
#if BOOST_WORKAROUND(BOOST_MSVC, <= 1300)
public:
#elif defined(BOOST_MSVC)
    // for some inexplicable reason insertion of "class" generates compile erro
    // on msvc 7.1
    friend detail::interface_oarchive<Archive>;
#else
    friend class detail::interface_oarchive<Archive>;
#endif
    // any datatype not specifed below will be handled by base class
    typedef detail::common_oarchive<Archive> detail_common_oarchive;
    template<class T>
    void save_override(const T & t, BOOST_PFTO int version){
      this->detail_common_oarchive::save_override(t, static_cast<int>(version));
    }

    // binary files don't include the optional information 
    void save_override(const class_id_optional_type & /* t */, int){}

    void save_override(const version_type & t, int){
        // upto 255 versions
        // note:t.t resolves borland ambguity
        assert(t.t <= boost::integer_traits<unsigned char>::const_max);
        const unsigned char x = static_cast<const unsigned char>(t.t);
        * this->This() << x;
    }
    void save_override(const class_id_type & t, int){
        // upto 32K classes
        assert(t.t <= boost::integer_traits<boost::int_least16_t>::const_max);
        const boost::int_least16_t x = static_cast<const boost::int_least16_t>(t.t); 
        * this->This() << x;
    }
    void save_override(const class_id_reference_type & t, int){
        // upto 32K classes
        assert(t.t <= boost::integer_traits<boost::int_least16_t>::const_max);
        const boost::int_least16_t x = t.t;
        * this->This() << x;
    }
    void save_override(const object_id_type & t, int){
        // upto 2G objects
        assert(t.t <= boost::integer_traits<boost::uint_least32_t>::const_max);
        const boost::uint_least32_t x = t.t;
        * this->This() << x;
    }
    void save_override(const object_reference_type & t, int){
        // upto 2G objects
        assert(t.t <= boost::integer_traits<boost::uint_least32_t>::const_max);
        const boost::uint_least32_t x = t.t;
        * this->This() << x;
    }
    void save_override(const tracking_type & t, int){
        const char x = t.t;
        * this->This() << x;
    }

    // explicitly convert to char * to avoid compile ambiguities
    void save_override(const class_name_type & t, int){
        const std::string s(t);
        * this->This() << s;
    }

    void save_override(const serialization::collection_size_type & t, int){
    // for backward compatibility, 64 bit integer or variable length integer would be preferred
        std::size_t x = t.t;
        * this->This() << x;
   }

    BOOST_ARCHIVE_OR_WARCHIVE_DECL(void)
    init();

    basic_binary_oarchive(unsigned int flags) :
        detail::common_oarchive<Archive>(flags)
    {}
};

} // namespace archive
} // namespace boost

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

#include <boost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

#endif // BOOST_ARCHIVE_BASIC_BINARY_OARCHIVE_HPP
