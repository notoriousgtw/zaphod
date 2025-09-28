#!/bin/bash
# Filter script to remove sections between DOXYGEN_EXCLUDE comments
sed '/<!-- DOXYGEN_EXCLUDE_BEGIN -->/,/<!-- DOXYGEN_EXCLUDE_END -->/d' "$1"