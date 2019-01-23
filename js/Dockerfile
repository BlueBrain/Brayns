FROM node:8.15.0-jessie as builder
LABEL maintainer="bbp-svc-viz@groupes.epfl.ch"

ARG DIST_PATH=/app/dist

WORKDIR /app
ADD . /app

RUN cd /app && \
    yarn install --pure-lockfile --non-interactive && \
    yarn build && \
    mkdir ${DIST_PATH} && \
    mv ./apps/viewer/build/* ${DIST_PATH}


FROM nginx:stable-alpine
ARG DIST_PATH=/app/dist

COPY --from=builder ${DIST_PATH} ${DIST_PATH}

# IMPORTANT: Check https://github.com/nginxinc/openshift-nginx/blob/master/Dockerfile for info on how to ensure OpenShift works with nginx.
ADD ./nginx/default.conf /etc/nginx/conf.d
ADD ./scripts/setup_nginx.sh /tmp
RUN /tmp/setup_nginx.sh && \
    rm -rf /var/cache/apk/* \
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
