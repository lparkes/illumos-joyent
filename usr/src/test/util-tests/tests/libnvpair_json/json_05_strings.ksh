#!/bin/ksh

DIR=$(dirname $(whence $0))
. ${DIR}/json_common

#
# This test checks UTF-8 parsing behaviour
#
export LC_ALL="en_US.UTF-8"
export LANG="${LANG}"

BASELINE="$(cat <<EOF
{\
"blank":"",\
"":"blank key",\
" ":"whitespace key",\
"\ttab\t":"tab key",\
"escapes":"escape \u001b newline \n cr \r backslash \\\\ quote \"",\
"escape array":[\
"escape \u001b",\
"alarm \u0007",\
"backspace \b",\
"formfeed \f",\
"newline \n",\
"return \r",\
"tab \t",\
"vertical tab \u000b",\
"black circle (UTF-8) \u25cf"\
]\
}
EOF)"

OUTPUT="$(${DIR}/../../bin/print_json <<'EOF'
add_string "blank" "";
add_string "" "blank key";
add_string " " "whitespace key";
add_string "	tab	" "tab key";
add_string "escapes" "escape \x1b newline \n cr \r backslash \\ quote \"";
add_string_array "escape array"
    "escape \x1b"
    "alarm \a"
    "backspace \b"
    "formfeed \f"
    "newline \n"
    "return \r"
    "tab \t"
    "vertical tab \v"
    "black circle (UTF-8) \xe2\x97\x8f";
EOF)"

complete
