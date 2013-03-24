#!/bin/sh

#where "*.py" may be replaced:
[[ `find . -maxdepth 1 -name "*.py"` != "" ]] && ls -l `find . -maxdepth 1 -name "*.py"`

#where ".*abc...*" my be replaced with any pattern:
find . -regextype posix-egrep -regex ".*abc...*"

curl -s -G http://www.uml.edu/Sciences/computer-science/alumni/Alumni-Directory.aspx | grep -Eon "[-%.&'*+/=?^_\`{|}~a-zA-Z0-9]+@[-.a-zA-Z0-9]+"
