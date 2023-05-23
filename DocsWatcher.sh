#!/bin/bash
while inotifywait -r -e modify codes docs/doxygen/pages docs/doxygen/main_page.doxy; do
    doxygen Doxyfile
done