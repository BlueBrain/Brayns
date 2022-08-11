JSONRPCAPI=source/jsonrpcapi
PYTHONAPI=source/pythonapi
HTML=html

rm -rf ${JSONRPCAPI}
python ../scripts/build_api_doc.py

rm -rf ${PYTHONAPI}
sphinx-apidoc -o ${PYTHONAPI} ../brayns

rm -rf ${HTML}
sphinx-build -b html source ${HTML}
