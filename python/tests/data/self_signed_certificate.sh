openssl req -x509 -newkey rsa:4096 -passout pass:'test' -keyout key.pem -out certificate.pem -sha256 -days 3650 -subj "/C=SW/ST=Geneva/L=Geneva/O=EPFL/OU=BBP/CN=localhost"
