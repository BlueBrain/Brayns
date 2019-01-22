import {Observable, Subject} from 'rxjs';
import {
    CAMERA_CHANGE,
    CAMERA_SETTINGS_CHANGE,
    CameraChange,
    dispatchCamera,
    dispatchCameraSettings,
    onCameraChange,
    onCameraSettingsChange
} from './camera';


const mockAddEventObserver = jest.fn();
const mockDispatchEvent = jest.fn();
jest.mock('./pubsub', () => ({
    addEventObserver(name: string) {
        return mockAddEventObserver(name);
    },
    dispatchEvent(name: string, data: any) {
        mockDispatchEvent(name, data);
    }
}));


describe('dispatchCamera()', () => {
    it('dispatches an camera change event', async () => {
        const camera: CameraChange = {
            position: [0, 1, 0],
            orientation: [0, 0, 0, 1],
            target: [0, 0, 0]
        };
        dispatchCamera(camera);
        expect(mockDispatchEvent).toHaveBeenCalledWith(CAMERA_CHANGE, camera);
        mockDispatchEvent.mockClear();
    });
});

describe('onCameraChange()', () => {
    beforeEach(() => {
        mockAddEventObserver.mockClear();
    });

    it('returns an Observable', () => {
        const subject = new Subject();
        mockAddEventObserver.mockReturnValueOnce(subject);
        expect(onCameraChange()).toBeInstanceOf(Observable);
    });

    it('should emit camera changes', done => {
        const subject = new Subject();
        mockAddEventObserver.mockReturnValueOnce(subject);

        const data = {position: [0, 1, 0]};

        onCameraChange()
            .subscribe(camera => {
                expect(mockAddEventObserver).toHaveBeenCalledWith(CAMERA_CHANGE);
                expect(camera).toEqual(data);
                done();
            });

        subject.next(data);
    });
});


describe('dispatchCameraSettings()', () => {
    it('dispatches an camera change event', async () => {
        const settings = {
            sensitivity: 0.1
        };
        dispatchCameraSettings(settings);
        expect(mockDispatchEvent).toHaveBeenCalledWith(CAMERA_SETTINGS_CHANGE, settings);
        mockDispatchEvent.mockClear();
    });
});

describe('onCameraSettingsChange()', () => {
    beforeEach(() => {
        mockAddEventObserver.mockClear();
    });

    it('returns an Observable', () => {
        const subject = new Subject();
        mockAddEventObserver.mockReturnValueOnce(subject);
        expect(onCameraSettingsChange()).toBeInstanceOf(Observable);
    });

    it('should emit camera changes', done => {
        const subject = new Subject();
        mockAddEventObserver.mockReturnValueOnce(subject);

        const data = {sensitivity: 1};

        onCameraSettingsChange()
            .subscribe(settings => {
                expect(mockAddEventObserver).toHaveBeenCalledWith(CAMERA_SETTINGS_CHANGE);
                expect(settings).toEqual(data);
                done();
            });

        subject.next(data);
    });
});
