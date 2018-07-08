# Brayns with Google Cloud Platform

> Use our [Docker](https://docs.docker.com) image to run Brayns as a service on [Google Cloud](https://cloud.google.com/)'s [Compute Engine](https://cloud.google.com/compute) infrastructure.


### Prerequisites
-----------------
Must have an account at [Google Cloud](https://cloud.google.com/). If not, go ahead and create one.


### Setup
---------
Install the [Cloud SDK](https://cloud.google.com/sdk) tool on your machine.

Create a new project and enable billing for it (see [creating-managing-projects](https://cloud.google.com/resource-manager/docs/creating-managing-projects)).

Create a VM instance based on our Docker image:
```bash
gcloud --project=PROJECT_NAME beta compute instances create-with-container brayns \
    --container-image docker.io/bluebrain/brayns \
    --tags brayns-server
```

We use port `:8200` by default, so you'll need to [publish](https://cloud.google.com/compute/docs/containers/configuring-options-to-run-containers#publishing_container_ports) that port:
```bash
gcloud --project=PROJECT_NAME compute firewall-rules create allow-brayns \
    --allow tcp:8200 \
    --target-tags brayns-server
```

Check if all went well:
```bash
# Should return a JSON string if it's running
curl http://EXTERNAL_IP:8200/registry
```
