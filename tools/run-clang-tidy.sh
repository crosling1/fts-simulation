#!/usr/bin/env bash

set -o pipefail

CLANG_TIDY="${CLANG_TIDY:-clang-tidy}"
STRICT_CLANG_TIDY_CHECKS="${STRICT_CLANG_TIDY_CHECKS:-0}"
REQUIRED_CHECKS=(
    bugprone-unchecked-optional-access
)

if ! command -v "${CLANG_TIDY}" >/dev/null 2>&1; then
    echo "error: ${CLANG_TIDY} was not found. Install clang-tidy or set CLANG_TIDY=/path/to/clang-tidy." >&2
    exit 127
fi

for check in "${REQUIRED_CHECKS[@]}"; do
    if ! "${CLANG_TIDY}" --list-checks -checks="${check}" | grep -q "${check}"; then
        if [[ "${STRICT_CLANG_TIDY_CHECKS}" == "1" ]]; then
            echo "error: ${CLANG_TIDY} does not support ${check}." >&2
            echo "Install a newer clang-tidy or run with CLANG_TIDY=/path/to/newer/clang-tidy." >&2
            exit 2
        fi

        echo "warning: ${CLANG_TIDY} does not support ${check}; local lint may miss CI warnings." >&2
    fi
done

"${CLANG_TIDY}" "$@" 2>&1 | grep -Ev \
    -e '^[0-9]+ warnings generated\.$' \
    -e '^Suppressed [0-9]+ warnings \([0-9]+ in non-user code\)\.$' \
    -e '^Use -header-filter=.* to display errors from all non-system headers\. Use -system-headers to display errors from system headers as well\.$'

exit "${PIPESTATUS[0]}"
