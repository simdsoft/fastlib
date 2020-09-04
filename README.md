# fastlib
* **fast_csv.hpp**: Only one API ```fast_csv_parse_line``` to help parse csv easy with FSM, no any memory alloc duraing parsing
  ```cpp
  std::string buffer = read_file("xxx.csv");
  const char* cursor = buffer.c_str();
  while(*cursor) {
     cursor = fastl::fast_csv_parse_line(cursor, [&](const char* v_start, const char* v_end) {
         // do somethings, store or parse the column value
         // std::string value(v_start, v_end);
     });
  ```
* **fast_split.hpp**: Yeah, idea same with ```fast_csv.hpp```
