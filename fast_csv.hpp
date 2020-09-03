/****************************************************************************
 Copyright (c) 2020 Simdsoft Limited.

 https://simdsoft.com

 Apache License 2.0

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#ifndef SIMDSOFT_FAST_CSV_HPP
#define SIMDSOFT_FAST_CSV_HPP
#include <map>
#include <set>
#include <unordered_map>
#include <vector>

namespace fastl {
    class fast_csv
    {
    public:
        /*
         * op prototype: op(const char* v_start, const char* v_end)
         */
        template <typename _Fty> inline static const char* parse_line(const char* s, _Fty op)
        {
            // FSM
            enum
            {
                normal, // new field
                quote_field,
                quote_field_try_end,
                quote_field_end,
            } state;

            state = normal;

            const char* _Start = s; // the start of every string
            const char* _Ptr = s; // source string iterator
            const char* _Quote = nullptr;
            int skipCRLF = 1;

        _L_loop:
            {
                switch (*_Ptr)
                {
                case ',':
                    switch (state)
                    {
                    case normal:
                        if (_Start <= _Ptr)
                            op(_Start, _Ptr);
                        _Start = _Ptr + 1;
                        break;
                    case quote_field_try_end:
                        if (_Start < _Ptr)
                            op(_Start, _Ptr - 1);
                    case quote_field_end:
                        state = normal;
                        _Start = _Ptr + 1; // skip the field end '\"'
                        break;
                    default:; // quote_field, do nothing
                    }

                    break;
                case '\"':
                    switch (state)
                    {
                    case normal:
                        state = quote_field;
                        ++_Start; // skip left '\"'
                        break;
                    case quote_field: // field end by right '\"'
                        _Quote = _Ptr;
                        state = quote_field_try_end; // delay 1 loop to detect whether is a
                                                      // inner quote, not end for field
                        break;
                    case quote_field_try_end:
                        if (_Ptr - _Quote > 1)
                        {
                            state = quote_field_end;
                            op(_Start, _Ptr);
                        }
                        else
                        {
                            state = quote_field;
                        }
                        break;
                    default:;
                    }
                    break;
                case '\r':
                    skipCRLF = 2;
                case '\n':
                    if (quote_field == state)
                    {
                        skipCRLF = 0;
                        break;
                    }
                case '\0':
                    switch (state)
                    {
                    case normal:
                        if (_Start <= _Ptr)
                            op(_Start, _Ptr);
                        break;
                    case quote_field_try_end:
                        if (_Start < _Ptr)
                            op(_Start, _Ptr - 1);
                        break;
                    default:; // other state, do nothing, quote_field, invalid csv format
                    }
                    goto _L_end;
                }
                ++_Ptr;

                goto _L_loop;
            }

        _L_end:
            return _Ptr + skipCRLF; // pointer to next line or after of
                                    // null-termainted-charactor
        }
    };
}

#endif
