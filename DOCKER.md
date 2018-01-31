# Brayns with Docker

> Use [Docker](https://docs.docker.com) to run Brayns as a service and avoid painful tooling setup.

**NOTE**: This is work in progress.

### Prerequisites
-----------------
Head over to [Docker](https://docs.docker.com/engine/installation/#supported-platforms) and install Docker for your own platform.


### Setup
---------
First build the image (*it's necessary to do this step if you want to run Brayns*):
```bash
docker build . -t brayns
```


### Usage
---------
By default, the entrypoint when running the image is `braynsService`, but if you want to ssh into the container use:
```bash
# `-p 8200:8200` is used only to provide some port bindings (host:container) if you want to run and access Brayns from your host while in the container
docker run -ti --rm --entrypoint bash -p 8200:8200 brayns
```

If you want to run Brayns use:
```bash
# Runs Brayns as a service with the HTTP interface binded on port 8200
docker run -ti --rm -p 8200:8200 brayns
```

**NOTE** If you are having trouble exiting the process after you run the container (with the above command), use `docker stop <container-id>` to stop the container.
`docker ps` will give you the current running process.

Run Brayns with the HTTP interface binded to a different port:
```bash
docker run -ti --rm -p 8300:8300 brayns --http-server :8300
```

Provide other flags (or env vars) to `braynsService`:
```bash
docker run -ti --rm -p 8300:8300 -e DEFLECT_HOST=<your deflect host> brayns \
    --http-server :8300 \
    --enable-benchmark true \
    --synchronous-mode true \
    --camera [100, 100]
```
