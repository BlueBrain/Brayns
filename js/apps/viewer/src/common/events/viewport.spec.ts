import {Observable, Subject} from 'rxjs';
import {
    dispatchViewport,
    onViewportChange,
    VIEWPORT_CHANGE
} from './viewport';


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


describe('dispatchViewport()', () => {
    it('dispatches a viewport change event', async () => {
        const viewport = [10, 10];
        dispatchViewport(viewport);
        expect(mockDispatchEvent).toHaveBeenCalledWith(VIEWPORT_CHANGE, viewport);
        mockDispatchEvent.mockClear();
    });
});

describe('onViewportChange()', () => {
    beforeEach(() => {
        mockAddEventObserver.mockClear();
    });

    it('returns an Observable', () => {
        const subject = new Subject();
        mockAddEventObserver.mockReturnValueOnce(subject);
        expect(onViewportChange()).toBeInstanceOf(Observable);
    });

    it('should emit viewport changes', done => {
        const subject = new Subject();
        mockAddEventObserver.mockReturnValueOnce(subject);

        const data = [10, 10];

        onViewportChange()
            .subscribe(viewport => {
                expect(mockAddEventObserver).toHaveBeenCalledWith(VIEWPORT_CHANGE);
                expect(viewport).toEqual(data);
                done();
            });

        subject.next(data);
    });
});
