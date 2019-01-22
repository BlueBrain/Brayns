import {Observable, Subject} from 'rxjs';
import {
    dispatchKeyboardLock,
    KEYBOARD_LOCK_CHANGE,
    onKeyboardLockChange
} from './keyboard';


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


describe('dispatchKeyboardLock()', () => {
    it('dispatches an keyboard lock change event', async () => {
        const lock = true;
        dispatchKeyboardLock(lock);
        expect(mockDispatchEvent).toHaveBeenCalledWith(KEYBOARD_LOCK_CHANGE, lock);
        mockDispatchEvent.mockClear();
    });
});

describe('onKeyboardLockChange()', () => {
    beforeEach(() => {
        mockAddEventObserver.mockClear();
    });

    it('returns an Observable', () => {
        const subject = new Subject();
        mockAddEventObserver.mockReturnValueOnce(subject);
        expect(onKeyboardLockChange()).toBeInstanceOf(Observable);
    });

    it('should emit keyboard lock changes', done => {
        const subject = new Subject();
        mockAddEventObserver.mockReturnValueOnce(subject);

        const data = true;

        onKeyboardLockChange()
            .subscribe(lock => {
                expect(mockAddEventObserver).toHaveBeenCalledWith(KEYBOARD_LOCK_CHANGE);
                expect(lock).toEqual(data);
                done();
            });

        subject.next(data);
    });
});
