/*************************************************************************
 *
 * Copyright (c) 2012 Kohei Yoshida
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

#include "xml_map_tree.hpp"

#define ORCUS_DEBUG_XML 1

#if ORCUS_DEBUG_XML
#include <iostream>
#endif

using namespace std;

namespace orcus {

xml_map_tree::xpath_error::xpath_error(const string& msg) : general_error(msg) {}

xml_map_tree::cell_reference::cell_reference() :
    row(-1), col(-1) {}

xml_map_tree::cell_reference::cell_reference(const pstring& _sheet, model::row_t _row, model::col_t _col) :
    sheet(_sheet), row(_row), col(_col) {}

xml_map_tree::cell_reference::cell_reference(const cell_reference& r) :
    sheet(r.sheet), row(r.row), col(r.col) {}

xml_map_tree::element::element(const pstring& _name, element_type _type) :
    name(_name), type(_type)
{
    switch (type)
    {
        case element_cell_ref:
            cell_ref = new cell_reference;
        break;
        case element_non_leaf:
            child_elements = new element_list_type;
        break;
        case element_range_field_ref:
            field_ref = new field_in_range;
        break;
        default:
            throw general_error("unexpected element type in the constructor.");
    }
}

xml_map_tree::element::~element()
{
    switch (type)
    {
        case element_cell_ref:
            delete cell_ref;
        break;
        case element_non_leaf:
            delete child_elements;
        break;
        case element_range_field_ref:
            delete field_ref;
        break;
        default:
            throw general_error("unexpected element type in the destructor.");
    }
}

xml_map_tree::xml_map_tree() : m_root(NULL) {}
xml_map_tree::~xml_map_tree()
{
    delete m_root;
}

void xml_map_tree::set_cell_link(const pstring& xpath, const cell_reference& ref)
{
    if (xpath.empty())
        return;

    cout << "cell link: " << xpath << " (ref=" << ref << ")" << endl;
    element* p = get_element(xpath, element_cell_ref);
    assert(p && p->cell_ref);
    *p->cell_ref = ref;
}

void xml_map_tree::set_range_field_link(
   const pstring& xpath, const cell_reference& ref, int column_pos)
{
    if (xpath.empty())
        return;

    cout << "range field link: " << xpath << " (ref=" << ref << "; column=" << column_pos << ")" << endl;
    element* p = get_element(xpath, element_range_field_ref);
    assert(p && p->field_ref);
    p->field_ref->ref = ref;
    p->field_ref->column_pos = column_pos;
}

namespace {

class find_by_name : std::unary_function<xml_map_tree::element, bool>
{
    pstring m_name;
public:
    find_by_name(const pstring& name) : m_name(name) {}
    bool operator() (const xml_map_tree::element& e) const
    {
        return m_name == e.name;
    }
};

class xpath_parser
{
    const char* mp_char;
    const char* mp_end;
    size_t m_size;
public:
    xpath_parser(const char* p, size_t n) : mp_char(p), mp_end(p+n), m_size(n)
    {
        if (!n)
            xml_map_tree::xpath_error("empty path");

        if (*p != '/')
            xml_map_tree::xpath_error("first character must be '/'.");

        ++mp_char;
    }

    pstring next()
    {
        if (mp_char == mp_end)
            return pstring();

        const char* p0 = mp_char;
        size_t len = 0;
        for (;mp_char != mp_end; ++mp_char, ++len)
        {
            if (*mp_char != '/')
                continue;

            // '/' encountered.
            ++mp_char; // skip the '/'.
            return pstring(p0, len);
        }

        // '/' has never been encountered.  It must be the last name in the path.
        return pstring(p0, len);
    }
};

}

const xml_map_tree::element* xml_map_tree::get_link(const pstring& xpath) const
{
    if (!m_root)
        return NULL;

    if (xpath.empty())
        return NULL;

    const element* cur_element = m_root;

    xpath_parser parser(xpath.get(), xpath.size());

    // Check the root element first.
    pstring name = parser.next();
    if (cur_element->name != name)
        return NULL;

    for (name = parser.next();!name.empty(); name = parser.next())
    {
        // See if an element of this name exists below the current element.
        if (cur_element->type != element_non_leaf)
            return NULL;

        if (!cur_element->child_elements)
            return NULL;

        element_list_type::const_iterator it =
            std::find_if(cur_element->child_elements->begin(), cur_element->child_elements->end(), find_by_name(name));
        if (it == cur_element->child_elements->end())
            // No such child element exists.
            return NULL;

        cur_element = &(*it);
    }

    if (cur_element->type == element_non_leaf)
        // Non-leaf elements are not links.
        return NULL;

    return cur_element;
}

xml_map_tree::element* xml_map_tree::get_element(const pstring& xpath, element_type type)
{
    assert(!xpath.empty());
    xpath_parser parser(xpath.get(), xpath.size());

    // Get the root element first.
    pstring name = parser.next();
    if (m_root)
    {
        // Make sure the root element's names are the same.
        if (m_root->name != name)
            xpath_error("path begins with inconsistent root level name.");
    }
    else
    {
        // First time the root element is encountered.
        m_root = new element(m_names.intern(name.get(), name.size()), element_non_leaf);
    }

    element* cur_element = m_root;

    assert(cur_element);
    assert(cur_element->child_elements);

    name = parser.next();
    for (pstring name_next = parser.next();!name_next.empty(); name_next = parser.next())
    {
        // Check if the current element contains a chile element of the same name.
        element_list_type& children = *cur_element->child_elements;
        element_list_type::iterator it = std::find_if(children.begin(), children.end(), find_by_name(name));
        if (it == children.end())
        {
            // Insert a new element of this name.
            children.push_back(new element(m_names.intern(name.get(), name.size()), element_non_leaf));
            cur_element = &children.back();
        }
        else
            cur_element = &(*it);

        name = name_next;
    }

    assert(cur_element);

    // Insert a leaf node.

    // Check if an element of the same name already exists.
    element_list_type& children = *cur_element->child_elements;
    element_list_type::iterator it = std::find_if(children.begin(), children.end(), find_by_name(name));
    if (it != children.end())
        throw xpath_error("This path is already linked.  You can't link the same path twice.");

    children.push_back(new element(m_names.intern(name.get(), name.size()), type));
    return &children.back();
}

std::ostream& operator<< (std::ostream& os, const xml_map_tree::cell_reference& ref)
{
    os << "[sheet='" << ref.sheet << "' row=" << ref.row << " column=" << ref.col << "]";
    return os;
}

}

