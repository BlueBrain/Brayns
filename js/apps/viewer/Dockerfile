FROM nginx:stable-alpine

LABEL maintainer="roland.groza@epfl.ch"

# Set working dir
WORKDIR /app
# Add source to working dir
ADD . /app

# Prepare app
# IMPORTANT: Check https://github.com/nginxinc/openshift-nginx/blob/master/Dockerfile for info on how to ensure OpenShift works with nginx.
# 1. Navigate to workdir
# 2. Fix nginx
# 3. Cleanup
RUN cd /app && \
    ./scripts/setup_nginx.sh && \
    mv ./build/* . && \
    rm -rf build \
        nginx \
        scripts \
        Dockerfile \
        /var/cache/apk/* \
        /usr/share/man \
        /tmp

## Ports
EXPOSE 8080

STOPSIGNAL SIGTERM

## Entrypoint
# Run nginx in foreground
# See https://stackoverflow.com/a/40828098/1092007
# for more details
CMD ["nginx", "-g", "daemon off;"]
