JSONRPCAPI=source/jsonrpcapi
PYTHONAPI=source/pythonapi
HTML=html

rm -rf ${JSONRPCAPI}
python ../scripts/build_json_rpc_doc.py ${JSONRPCAPI}

rm -rf ${PYTHONAPI}
sphinx-apidoc -o ${PYTHONAPI} ../brayns

rm -rf ${HTML}
sphinx-build -b html source ${HTML}
