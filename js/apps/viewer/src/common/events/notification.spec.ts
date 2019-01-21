import {JsonRpcError} from 'rockets-client';
import {Observable, Subject} from 'rxjs';
import {
    dispatchNotification,
    NOTIFICATION,
    onNotification
} from './notification';


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

const mockUid = jest.fn();
jest.mock('crypto-uid', () => () => {
    return mockUid();
});


describe('dispatchNotification()', () => {
    beforeEach(() => {
        mockUid.mockClear();
        mockDispatchEvent.mockClear();
    });

    it('dispatches a message as notification with {id, message}', async () => {
        const uid = 'test';
        const message = 'Hey';
        mockUid.mockReturnValueOnce(uid);
        dispatchNotification(message);
        expect(mockDispatchEvent).toHaveBeenCalledWith(NOTIFICATION, {
            message,
            id: uid
        });
    });

    it('dispatches an error as notification with {id, message}', async () => {
        const uid = 'test';
        const error = new JsonRpcError({
            code: -1,
            message: 'Hey'
        });
        mockUid.mockReturnValueOnce(uid);
        dispatchNotification(error);
        expect(mockDispatchEvent).toHaveBeenCalledWith(NOTIFICATION, {
            id: uid,
            message: error.message
        });
    });
});

describe('onNotification()', () => {
    beforeEach(() => {
        mockAddEventObserver.mockClear();
    });

    it('returns an Observable', () => {
        const subject = new Subject();
        mockAddEventObserver.mockReturnValueOnce(subject);
        expect(onNotification()).toBeInstanceOf(Observable);
    });

    it('should emit notifications', done => {
        const subject = new Subject();
        mockAddEventObserver.mockReturnValueOnce(subject);

        const data = {ping: true};

        onNotification()
            .subscribe(n => {
                expect(mockAddEventObserver).toHaveBeenCalledWith(NOTIFICATION);
                expect(n).toEqual(data);
                done();
            });

        subject.next(data);
    });
});
