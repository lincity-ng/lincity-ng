#!/bin/sh
# The original tiny script is use to put linux code in corformance with
# the Linux Coding Style. You should read it :-)
#indent -npro -kr -i8 -ts8 -sob -l80 -ss -ncs -cp1 "$@"

# However for Lincity-ng, due to
# - C++ code which tends to have longs lines
# - NG is graphic so run on rather good hardware => we can afford longer line (120) than linux kernel (80)
# - indent 8 is too much for existing code => indent is 4
# - tabs causes trouble to some we use spaces intead of tabs.
indent -npro -kr -i4 --no-tabs -sob -l120 -ss -ncs -cp1 "$@"
