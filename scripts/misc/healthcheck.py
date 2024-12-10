import requests

reply = requests.get("https://sbo-brayns.shapes-registry.org:8000/healthz")

print(reply.status_code, reply.content)
