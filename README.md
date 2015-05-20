# evelopers_cleaner

EOL after comment is not removed. (e.g. "//abc\n" => "\n", "//abc\n\r" => "\n\r"
Comments inside string literals are not ignored.
Unfinished comments in the end of line are removed.
