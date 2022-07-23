import base64
import sys
import json
import gzip
import io

from google.oauth2 import service_account
from google.auth.transport.requests import AuthorizedSession

# Define the required scopes
scopes = [
  "https://www.googleapis.com/auth/userinfo.email",
  "https://www.googleapis.com/auth/firebase.database"
]

DB_URL = "https://sreelas-unnamed-project-default-rtdb.firebaseio.com/"


# Authenticate a credential with the service account
credentials = service_account.Credentials.from_service_account_file("service_account_key.json", scopes=scopes)

session = AuthorizedSession(credentials)

if len(sys.argv) < 2:
    print("Error: Must give a version!")
    sys.exit(1)
version = sys.argv[1]
print(f"Firmware version {version}")

if len(sys.argv) > 2:
    FIRMWARE_PATH = sys.argv[2]
else:
    FIRMWARE_PATH = ".pio/build/nodemcuv2/firmware.bin"

CHUNK_SIZE = 1024
chunks = []

with open(FIRMWARE_PATH, "rb") as f:
    zipped = gzip.compress(f.read())

print(f"INFO: Firmware is {len(zipped)} bytes compressed")

firmware = io.BytesIO(zipped)
# Break data into chunks to store at separate keys
while True:
    chunk = firmware.read(CHUNK_SIZE)
    if not chunk:
        break
    chunks.append("blob,base64," + base64.b64encode(chunk).decode("ascii"))

data = {
    "firmware": {
        "chunkCount": len(chunks),
        "bin": {str(i): chunks[i] for i in range(len(chunks))}
    },
    "version": version,
}

print(f"INFO: Total {len(chunks)} chunks ({CHUNK_SIZE} per chunk)")

session.patch(DB_URL + "ota.json", data=json.dumps(data))

print("Done")
