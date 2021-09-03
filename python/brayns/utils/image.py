import base64
import io
from PIL import Image


def base64decode(data: str) -> bytes:
    """Add the missing padding and decode base64 data."""
    missing_padding = len(data) % 4
    if missing_padding != 0:
        data += b'=' * (4 - missing_padding)
    return base64.b64decode(data)


def convert_snapshot_response_to_PIL(data: str) -> Image:
    """Convert the snapshot base64 data received from Brayns to a PIL image."""
    return Image.open(io.BytesIO(base64decode(data)))
