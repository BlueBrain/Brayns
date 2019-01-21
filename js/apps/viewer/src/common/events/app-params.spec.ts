import {Observable, Subject} from 'rxjs';
import {
    APP_PARAMS_CHANGE,
    dispatchAppParams,
    onAppParamsChange
} from './app-params';


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


describe('dispatchAppParams()', () => {
    it('dispatches an app params change event', async () => {
        const params = {imageStreamFps: 10};
        dispatchAppParams(params);
        expect(mockDispatchEvent).toHaveBeenCalledWith(APP_PARAMS_CHANGE, params);
        mockDispatchEvent.mockClear();
    });
});

describe('onAppParamsChange()', () => {
    beforeEach(() => {
        mockAddEventObserver.mockClear();
    });

    it('returns an Observable', () => {
        const subject = new Subject();
        mockAddEventObserver.mockReturnValueOnce(subject);
        expect(onAppParamsChange()).toBeInstanceOf(Observable);
    });

    it('should emit app params changes', done => {
        const subject = new Subject();
        mockAddEventObserver.mockReturnValueOnce(subject);

        const data = {imageStreamFps: 10};

        onAppParamsChange()
            .subscribe(params => {
                expect(mockAddEventObserver).toHaveBeenCalledWith(APP_PARAMS_CHANGE);
                expect(params).toEqual(data);
                done();
            });

        subject.next(data);
    });
});
