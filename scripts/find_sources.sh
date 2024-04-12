find \
    apps src tests \
    \( -name "*.h" -or -name "*.cpp" \) \
    -not -path "src/brayns/core/deps/*.h" \
    -not -path "tests/deps/*.h"
