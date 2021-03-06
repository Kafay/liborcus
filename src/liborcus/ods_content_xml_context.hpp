/*************************************************************************
 *
 * Copyright (c) 2010, 2011 Kohei Yoshida
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

#ifndef __ORCUS_ODSCONTEXT_HPP__
#define __ORCUS_ODSCONTEXT_HPP__

#include "xml_context_base.hpp"
#include "odf_para_context.hpp"
#include "odf_styles.hpp"

#include <vector>
#include <boost/scoped_ptr.hpp>
#include <boost/unordered_map.hpp>

namespace orcus {

namespace spreadsheet { namespace iface {

class import_factory;
class import_sheet;

}}

class ods_content_xml_context : public xml_context_base
{
    typedef boost::unordered_map<pstring, size_t, pstring::hash> name2id_type;

public:
    struct row_attr
    {
        int number_rows_repeated;
        row_attr();
    };

    enum cell_value_type { vt_unknown, vt_float, vt_string, vt_date };

    struct cell_attr
    {
        int number_columns_repeated;
        cell_value_type type;
        double value;
        pstring date_value;
        pstring style_name;

        cell_attr();
    };

    ods_content_xml_context(session_context& session_cxt, const tokens& tokens, spreadsheet::iface::import_factory* factory);
    virtual ~ods_content_xml_context();

    virtual bool can_handle_element(xmlns_id_t ns, xml_token_t name) const;
    virtual xml_context_base* create_child_context(xmlns_id_t ns, xml_token_t name);
    virtual void end_child_context(xmlns_id_t ns, xml_token_t name, xml_context_base* child);

    virtual void start_element(xmlns_id_t ns, xml_token_t name, const xml_attrs_t& attrs);
    virtual bool end_element(xmlns_id_t ns, xml_token_t name);
    virtual void characters(const pstring& str, bool transient);

private:
    void start_null_date(const xml_attrs_t& attrs);

    void start_table(const xml_attrs_t& attrs);
    void end_table();

    void start_column(const xml_attrs_t& attrs);
    void end_column();

    void start_row(const xml_attrs_t& attrs);
    void end_row();

    void start_cell(const xml_attrs_t& attrs);
    void end_cell();

    void push_cell_value();

private:
    spreadsheet::iface::import_factory* mp_factory;
    std::vector<spreadsheet::iface::import_sheet*> m_tables;

    boost::scoped_ptr<xml_context_base> mp_child;

    row_attr    m_row_attr;
    cell_attr   m_cell_attr;

    int m_row;
    int m_col;
    size_t m_para_index;
    bool m_has_content;

    odf_styles_map_type m_styles; /// map storing all automatic styles by their names.
    name2id_type m_cell_format_map; /// map of style names to cell format (xf) IDs.

    text_para_context m_child_para;
};

}

#endif
