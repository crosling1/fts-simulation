#!/usr/bin/env bash

set -o pipefail

clang-tidy "$@" 2>&1 | grep -Ev \
    -e '^[0-9]+ warnings generated\.$' \
    -e '^Suppressed [0-9]+ warnings \([0-9]+ in non-user code\)\.$' \
    -e '^Use -header-filter=.* to display errors from all non-system headers\. Use -system-headers to display errors from system headers as well\.$'

exit "${PIPESTATUS[0]}"
