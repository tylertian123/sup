import base64
import sys
import json
import gzip
import io

from google.oauth2 import service_account
from google.auth.transport.requests import AuthorizedSession

USAGE = """
ota_helper.py <version> [firmware] [options]

Upload a firmware to Firebase for OTA.

Options:
    -h, --help: Show this message
    --auth=<service_account_key_file>: Specify service account key file for authentication
    --valid=<true|false>: Set the 'valid' key for the firmware 
"""

args = []
opts = {}
for arg in sys.argv:
    if arg.startswith("-"):
        opt = arg[2:] if arg.startswith("--") else arg[1:]
        if "=" in opt:
            i = opt.find("=")
            opts[opt[:i]] = opt[i + 1:]
        else:
            opts[opt] = None
    else:
        args.append(arg)

if "h" in opts or "help" in opts:
    print(USAGE)
    sys.exit(0)

# Define the required scopes
scopes = [
  "https://www.googleapis.com/auth/userinfo.email",
  "https://www.googleapis.com/auth/firebase.database"
]

DB_URL = "https://sreelas-unnamed-project-default-rtdb.firebaseio.com/"


# Authenticate a credential with the service account
credentials = service_account.Credentials.from_service_account_file(opts.get("auth") or "service_account_key.json", scopes=scopes)

session = AuthorizedSession(credentials)

if len(args) < 2:
    print("Error: Must give a version!")
    print(USAGE)
    sys.exit(1)

version = args[1]
print(f"Firmware version {version}")

if len(args) > 2:
    FIRMWARE_PATH = args[2]
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

if "valid" in opts:
    if opts["valid"].lower() == "true":
        data["valid"] = True
    elif opts["valid"].lower() == "false":
        data["valid"] = False
    else:
        print("Error: Invalid option!")
        print(USAGE)
        sys.exit(1)

print(f"INFO: Total {len(chunks)} chunks ({CHUNK_SIZE} per chunk)")

session.patch(DB_URL + "ota.json", data=json.dumps(data))

print("Done.")
