import requests

SHA = "0172874b4e337b68ceab9f4d8e7b889dcc167e78"
URL = f"https://api.github.com/repos/BlueBrain/Brayns/statuses/{SHA}"
TOKEN = "toto"
HEADERS = {
    "Accept": "application/vnd.github+json",
    "Authorization": f"Bearer {TOKEN}",
    "X-GitHub-Api-Version": "2022-11-28",
}
BODY = {
    "state": "pending",
    "context": "test",
    "description": "This is a test",
}

response = requests.post(URL, headers=HEADERS, json=BODY)

print(response.status_code)

print(response.content)
