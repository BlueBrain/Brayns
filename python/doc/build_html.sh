#!/bin/bash

# Requires python venv activated with brayns latest version installed and
# a running instance of braynsService on localhost:5000.

DOC=$(dirname $0)
SOURCE=${DOC}/source
JSONRPCAPI=${SOURCE}/jsonrpcapi
PYTHONAPI=${SOURCE}/pythonapi
HTML=${DOC}/html

rm -rf ${JSONRPCAPI}
python ${DOC}/../scripts/build_json_rpc_doc.py ${JSONRPCAPI}

rm -rf ${PYTHONAPI}
python ${DOC}/../scripts/build_python_doc.py ${PYTHONAPI}

rm -rf ${HTML}
sphinx-build -b html ${SOURCE} ${HTML}
