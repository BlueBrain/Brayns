import {Request, RequestTask} from 'rockets-client';
import {Observable, Subject} from 'rxjs';
import {
    dispatchRequest,
    dispatchRequestCancel,
    onRequestCancel,
    onRequestDone,
    onRequestProgress,
    onRequestStart,
    REQUEST_CANCEL,
    REQUEST_DONE,
    REQUEST_PROGRESS,
    REQUEST_START
} from './request';


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


describe('dispatchRequest()', () => {
    beforeEach(() => {
        mockDispatchEvent.mockClear();
    });

    it('dispatches a request start event with the request', () => {
        const source = new Subject();
        const request = new Request('test');
        const task = createTask(source, request);
        dispatchRequest(task);
        expect(mockDispatchEvent).toHaveBeenCalledWith(REQUEST_START, request);
    });

    it('dispatches request progress update events with the request id and progress when the request emits state changes', () => {
        const source = new Subject();
        const request = new Request('test');

        const task = createTask(source, request);
        dispatchRequest(task);

        source.next({amount: 0.5});
        source.next({amount: 1});

        // First call is for request start
        expect(mockDispatchEvent).toHaveBeenCalledTimes(3);
        const [, callOne, callTwo] = mockDispatchEvent.mock.calls;

        expect(callOne).toEqual([
            REQUEST_PROGRESS,
            {id: request.id, amount: 0.5}
        ]);

        expect(callTwo).toEqual([
            REQUEST_PROGRESS,
            {id: request.id, amount: 1}
        ]);
    });

    it('dispatches a request complete event with the request when the request finishes', () => {
        const source = new Subject();
        const request = new Request('test');

        const task = createTask(source, request);
        dispatchRequest(task);

        source.complete();

        expect(mockDispatchEvent).toHaveBeenCalledWith(REQUEST_DONE, {
            request,
            error: false
        });
    });

    it('dispatches a request complete event with the request when the request throws', () => {
        const source = new Subject();
        const request = new Request('test');
        const error = new Error('Oops :(');

        const task = createTask(source, request);
        dispatchRequest(task);

        source.error(error);

        expect(mockDispatchEvent).toHaveBeenCalledWith(REQUEST_DONE, {
            request,
            error
        });
    });
});

describe('dispatchRequestCancel()', () => {
    it('dispatches a request start event with the request', () => {
        const request = new Request('test');
        dispatchRequestCancel(request);
        expect(mockDispatchEvent).toHaveBeenCalledWith(REQUEST_CANCEL, request);

        mockDispatchEvent.mockClear();
    });
});

describe('onRequestStart()', () => {
    beforeEach(() => {
        mockAddEventObserver.mockClear();
    });

    it('returns an Observable', () => {
        const subject = new Subject();
        mockAddEventObserver.mockReturnValueOnce(subject);
        expect(onRequestStart()).toBeInstanceOf(Observable);
    });

    it('should emit request start', done => {
        const subject = new Subject();
        mockAddEventObserver.mockReturnValueOnce(subject);

        const data = {ping: true};

        onRequestStart()
            .subscribe(request => {
                expect(mockAddEventObserver).toHaveBeenCalledWith(REQUEST_START);
                expect(request).toEqual(data);
                done();
            });

        subject.next(data);
    });
});

describe('onRequestProgress()', () => {
    beforeEach(() => {
        mockAddEventObserver.mockClear();
    });

    it('returns an Observable', () => {
        const subject = new Subject();
        mockAddEventObserver.mockReturnValueOnce(subject);
        expect(onRequestProgress()).toBeInstanceOf(Observable);
    });

    it('should emit request progress', done => {
        const subject = new Subject();
        mockAddEventObserver.mockReturnValueOnce(subject);

        const data = {ping: true};

        onRequestProgress()
            .subscribe(progress => {
                expect(mockAddEventObserver).toHaveBeenCalledWith(REQUEST_PROGRESS);
                expect(progress).toEqual(data);
                done();
            });

        subject.next(data);
    });
});

describe('onRequestDone()', () => {
    beforeEach(() => {
        mockAddEventObserver.mockClear();
    });

    it('returns an Observable', () => {
        const subject = new Subject();
        mockAddEventObserver.mockReturnValueOnce(subject);
        expect(onRequestDone()).toBeInstanceOf(Observable);
    });

    it('should emit request complete', done => {
        const subject = new Subject();
        mockAddEventObserver.mockReturnValueOnce(subject);

        const data = {ping: true};

        onRequestDone()
            .subscribe(request => {
                expect(mockAddEventObserver).toHaveBeenCalledWith(REQUEST_DONE);
                expect(request).toEqual(data);
                done();
            });

        subject.next(data);
    });
});

describe('onRequestCancel()', () => {
    beforeEach(() => {
        mockAddEventObserver.mockClear();
    });

    it('returns an Observable', () => {
        const subject = new Subject();
        const source = new Subject();
        const request = new Request('test');

        mockAddEventObserver.mockReturnValueOnce(subject);

        const task = createTask(source, request);
        expect(onRequestCancel(task)).toBeInstanceOf(Observable);
    });

    it('should emit request cancel', done => {
        const subject = new Subject();
        const source = new Subject();
        const request = new Request('test');

        mockAddEventObserver.mockReturnValueOnce(subject);

        const task = createTask(source, request);

        onRequestCancel(task)
            .subscribe(r => {
                expect(mockAddEventObserver).toHaveBeenCalledWith(REQUEST_CANCEL);
                expect(r).toEqual(request);
                done();
            });

        subject.next(request);
    });
});


function createTask(source: Subject<any>, request: Request): RequestTask<any, any> {
    return {
        request,
        on() {
            return source;
        }
    } as any;
}
