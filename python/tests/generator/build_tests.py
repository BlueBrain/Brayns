import test_builder
import pathlib

DIRECTORY = pathlib.Path(__file__).parent
SAMPLES = DIRECTORY / 'samples.py'
URI = 'localhost:5000'
FOLDER = DIRECTORY.parent / 'requests'

if __name__ == '__main__':
    test_builder.create_mock_requests(
        samples_path=SAMPLES,
        brayns_uri=URI,
        output_folder=FOLDER
    )
