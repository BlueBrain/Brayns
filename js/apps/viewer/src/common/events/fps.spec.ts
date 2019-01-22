import {Observable, Subject} from 'rxjs';
import {
    dispatchFps,
    FPS_CHANGE,
    onFpsChange
} from './fps';


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


describe('dispatchFps()', () => {
    it('dispatches an fps change event', async () => {
        const fps = 10;
        dispatchFps(fps);
        expect(mockDispatchEvent).toHaveBeenCalledWith(FPS_CHANGE, fps);
        mockDispatchEvent.mockClear();
    });
});

describe('onFpsChange()', () => {
    beforeEach(() => {
        mockAddEventObserver.mockClear();
    });

    it('returns an Observable', () => {
        const subject = new Subject();
        mockAddEventObserver.mockReturnValueOnce(subject);
        expect(onFpsChange()).toBeInstanceOf(Observable);
    });

    it('should emit fps changes', done => {
        const subject = new Subject();
        mockAddEventObserver.mockReturnValueOnce(subject);

        const data = 10;

        onFpsChange()
            .subscribe(fps => {
                expect(mockAddEventObserver).toHaveBeenCalledWith(FPS_CHANGE);
                expect(fps).toEqual(data);
                done();
            });

        subject.next(data);
    });
});
