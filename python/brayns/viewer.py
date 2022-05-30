import io
import json
import logging
import sys

import brayns
import PySimpleGUI as sg
from PIL import Image

IMAGE = '-IMAGE-'
METHOD = '-METHOD-'
PARAMS = '-PARAMS-'
SEND = '-SEND-'
RESULT = '-RESULT-'

POSITION_X = '-POSITION_X-'
POSITION_Y = '-POSITION_Y-'
POSITION_Z = '-POSITION_Z-'

TARGET_X = '-TARGET_X-'
TARGET_Y = '-TARGET_Y-'
TARGET_Z = '-TARGET_Z-'

UP_X = '-UP_X-'
UP_Y = '-UP_Y-'
UP_Z = '-UP_Z-'


def create_image_column() -> sg.Column:
    layout = [
        [sg.Image(key=IMAGE, size=(600, 400))]
    ]
    return sg.Column(layout)

def create_control_column() -> sg.Column:
    layout = [
        [sg.Text('Method')],
        [sg.Input(key=METHOD)],
        [sg.Text('Params')],
        [sg.Multiline(key=PARAMS, size=(None, 20))],
        [sg.Button('Send', key=SEND)],
        [sg.Text('Result')],
        [sg.Multiline(key=RESULT, size=(None, 20), disabled=True)]
    ]
    return sg.Column(layout, vertical_alignment='top')

def create_window() -> sg.Window:
    layout = [
        [create_image_column(), create_control_column()]
    ]
    return sg.Window('Brayns Viewer', layout, resizable=True)

def convert_to_png(jpeg: bytes) -> bytes:
    jpeg_io = io.BytesIO(jpeg)
    image = Image.open(jpeg_io)
    png_io = io.BytesIO()
    image.save(png_io, 'png')
    return png_io.getvalue()

def on_binary(window: sg.Window, data: bytes) -> None:
    data = convert_to_png(data)
    image: sg.Image = window[IMAGE]
    image.update(data=data)

def get_params(values: dict) -> str:
    data: str = values[PARAMS]
    if not data or data.isspace():
        return None
    return json.loads(data)

def send_request(instance: brayns.Instance, window: sg.Window, values: dict) -> None:
    method: str = values[METHOD]
    params = get_params(values)
    result = instance.request(method, params)
    text = json.dumps(result, indent=4)
    element: sg.Text = window[RESULT]
    element.update(value=text)

def try_send_request(instance: brayns.Instance, window: sg.Window, values: dict) -> None:
    try:
        send_request(instance, window, values)
    except Exception as e:
        sg.PopupError(str(e), title='Invalid request')

def process_events(instance: brayns.Instance, window: sg.Window) -> None:
    while True:
        event, values = window.read(timeout=20)
        if event == sg.WIN_CLOSED:
            return
        if event == sg.TIMEOUT_EVENT:
            continue
        print(event, values)
        if event == SEND:
            try_send_request(instance, window, values)

def run(uri: str):
    window = create_window()
    with brayns.connect(
        uri=uri,
        on_binary=lambda data: on_binary(window, data),
        log_level=logging.DEBUG
    ) as instance:
        process_events(instance, window)
    window.close()

if __name__ == '__main__':
    argv = sys.argv
    uri = '128.178.97.151:5000'
    if len(argv) > 1:
        uri = argv[1]
    run(uri)
