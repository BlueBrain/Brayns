#!/bin/bash
set -e

REMOTE_VERSION=`npm show brayns version 2> /dev/null` || true
R_VER=${REMOTE_VERSION// /}
# https://gist.github.com/DarrenN/8c6a5b969481725a4413
LOCAL_VERSION=$(node -p "require('./packages/sdk/package.json').version")
L_VER=${LOCAL_VERSION// /}

echo "Remote version: ${R_VER}"
echo "Local version: ${L_VER}"

if [ "${R_VER}" != "${L_VER}" ]; then
    export BRAYNS_CLIENT_VERSION=${L_VER}
    echo "Set version: ${L_VER}"
fi
