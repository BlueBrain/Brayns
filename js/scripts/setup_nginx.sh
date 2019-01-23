#!/bin/sh
set -e

NGINX_CONF=/etc/nginx/nginx.conf
NGINX_LOG_DIR=/var/log/nginx

chmod -R 777 /var/cache/nginx
sed -i -e '/user/!b' -e '/nginx/!b' -e '/nginx/d' ${NGINX_CONF}
sed -i 's!/var/run/nginx.pid!/var/cache/nginx/nginx.pid!g' ${NGINX_CONF}
ln -sf /dev/stdout ${NGINX_LOG_DIR}/access.log
ln -sf /dev/stderr ${NGINX_LOG_DIR}/error.log

