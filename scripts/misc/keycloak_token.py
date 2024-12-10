from getpass import getpass
import requests

# SBO
HOST = "sboauth.epfl.ch"
REALM = "SBO"
CLIENT_ID = "sbo-brayns"
USERNAME = "sbo-viz"
PASSWORD = "623bxDBbS2MSDJ0OLmGRTb7osn4"

# BBP
HOST = "bbpauth.epfl.ch"
REALM = "BBP"
CLIENT_ID = "bbp-braynscircuitstudio"
USERNAME = "acfleury"
PASSWORD = getpass()

URL = f"https://{HOST}/auth/realms/{REALM}/protocol/openid-connect"

response = requests.post(
    f"{URL}/token",
    headers={"content-type": "application/x-www-form-urlencoded"},
    data={
        "grant_type": "password",
        "client_id": CLIENT_ID,
        "username": USERNAME,
        "password": PASSWORD,
    },
)

print(response.status_code)
print(response.reason)

data = response.json()

token = data["access_token"]

print(token)

response = requests.get(
    f"{URL}/userinfo",
    headers={
        "Host": HOST,
        "Authorization": f"Bearer {token}",
    },
)

print(response.status_code)
print(response.reason)

print(response.content)
