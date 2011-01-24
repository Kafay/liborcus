/*************************************************************************
 *
 * Copyright (c) 2011 Kohei Yoshida
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 ************************************************************************/

#ifndef __ORCUS_MODEL_SHARED_STRINGS_HPP__
#define __ORCUS_MODEL_SHARED_STRINGS_HPP__

#include <cstdlib>
#include <vector>

namespace orcus {

class pstring;

}

namespace orcus { namespace model {

/**
 * Interface class designed to be derived by the implementor.
 * 
 */
class shared_strings_base
{
public:
    virtual ~shared_strings_base() = 0;

    /**
     * Append new string to the string list.  Order of insertion is important 
     * since that determines the numerical ID values of inserted strings.
     *  
     * @param s pointer to the first character of the string array.  The 
     *          string array doesn't necessary have to be null-terminated.
     * @param n length of the string.
     */
    virtual void append(const char* s, size_t n) = 0;
};

/**
 * This class handles global pool of string instances.
 */
class shared_strings : public shared_strings_base
{
public:
    shared_strings();
    virtual ~shared_strings();

    virtual void append(const char* s, size_t n);

    bool has(size_t index) const;
    const pstring& get(size_t index) const;
private:
    ::std::vector<pstring> m_strings;
};

}}

#endif
