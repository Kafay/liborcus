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

#ifndef __ORCUS_SPREADSHEET_ODSTABLE_HPP__
#define __ORCUS_SPREADSHEET_ODSTABLE_HPP__

#include "orcus/spreadsheet/import_interface.hpp"
#include "orcus/spreadsheet/export_interface.hpp"
#include "orcus/env.hpp"

#include <ostream>

namespace orcus {

class pstring;

namespace spreadsheet {

class document;
struct sheet_impl;

/**
 * This class represents a single sheet instance in the internal document
 * model.
 */
class ORCUS_DLLPUBLIC sheet : public iface::import_sheet, public iface::export_sheet
{
    friend struct sheet_impl;

    static const row_t max_row_limit;
    static const col_t max_col_limit;

public:
    sheet(document& doc, sheet_t sheet_index, row_t row_size, col_t col_size);
    virtual ~sheet();

    // Import methods

    virtual iface::import_sheet_properties* get_sheet_properties();

    virtual void set_auto(row_t row, col_t col, const char* p, size_t n);
    virtual void set_string(row_t row, col_t col, size_t sindex);
    virtual void set_value(row_t row, col_t col, double value);
    virtual void set_bool(row_t row, col_t col, bool value);
    virtual void set_date_time(row_t row, col_t col, int year, int month, int day, int hour, int minute, double second);
    virtual void set_format(row_t row, col_t col, size_t index);
    virtual void set_formula(row_t row, col_t col, formula_grammar_t grammar, const char* p, size_t n);
    virtual void set_shared_formula(
        row_t row, col_t col, formula_grammar_t grammar, size_t sindex,
        const char* p_formula, size_t n_formula, const char* p_range, size_t n_range);
    virtual void set_shared_formula(
        row_t row, col_t col, formula_grammar_t grammar, size_t sindex,
        const char* p_formula, size_t n_formula);
    virtual void set_shared_formula(row_t row, col_t col, size_t sindex);
    virtual void set_formula_result(row_t row, col_t col, const char* p, size_t n);
    virtual void set_array_formula(row_t, col_t, formula_grammar_t,
            const char*, size_t, row_t, col_t);
    virtual void set_array_formula(row_t, col_t, formula_grammar_t,
            const char*, size_t, const char*, size_t);

    // Export methods

    virtual void write_string(std::ostream& os, row_t row, col_t col) const;

    void set_col_width(col_t col, col_width_t width);
    col_width_t get_col_width(col_t col, col_t* col_start, col_t* col_end) const;

    void set_col_hidden(col_t col, bool hidden);

    void set_row_height(row_t row, row_height_t height);
    row_height_t get_row_height(row_t row, row_t* row_start, row_t* row_end) const;

    void set_row_hidden(row_t row, bool hidden);

    size_t get_string_identifier(row_t row, col_t col) const;

    row_t row_size() const;
    col_t col_size() const;

    void finalize();

    void dump() const;
    void dump_check(std::ostream& os, const pstring& sheet_name) const;
    void dump_html(const ::std::string& filepath) const;

    /**
     * Get the cell format ID of specified cell.
     */
    size_t get_cell_format(row_t row, col_t col) const;

private:
    sheet_impl* mp_impl;
};

}}

#endif
