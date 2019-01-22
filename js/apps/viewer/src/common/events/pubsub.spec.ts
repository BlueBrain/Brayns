import {Observable} from 'rxjs';
import {addEventObserver, dispatchEvent} from './pubsub';

describe('addEventObserver()', () => {
    it('should return an observable', () => {
        const observer = addEventObserver('test');
        expect(observer).toBeInstanceOf(Observable);
    });

    it('should react on event dispatch', done => {
        const evt = 'test';
        addEventObserver(evt)
            .subscribe(() => {
                done();
            });

        dispatchEvent(evt);
    });
});

describe('dispatchEvent()', () => {
    it('should emit events on the observers created by addEventObserver()', done => {
        const evt = 'test';
        const data = {ping: true};

        addEventObserver(evt)
            .subscribe(d => {
                expect(d).toEqual(data);
                done();
            });

        dispatchEvent(evt, data);
    });
});
