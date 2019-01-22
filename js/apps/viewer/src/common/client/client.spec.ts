import {
    BehaviorSubject,
    Observable,
    Subject
} from 'rxjs';
import {ifReady, isReady} from './client';


const mockReady = jest.fn();
jest.mock('brayns', () => {
   class Client {
       get ready() {
           return mockReady();
       }
   }
   return {Client};
});


describe('ifReady()', () => {
    it('returns an Observable', () => {
        const data = {ping: true};
        const subject = new BehaviorSubject(true);
        mockReady.mockImplementationOnce(() => subject);
        expect(ifReady(data)).toBeInstanceOf(Observable);
    });

    it('returns arg if there is a connection', done => {
        const data = {ping: true};
        const subject = new BehaviorSubject(true);
        mockReady.mockImplementationOnce(() => subject);

        ifReady(data)
            .subscribe(res => {
                expect(res).toEqual(data);
                done();
            });
    });

    it('never emits if there is no connection', done => {
        const data = {ping: true};
        const subject = new Subject();
        mockReady.mockImplementationOnce(() => subject);

        ifReady(data)
            .subscribe(() => {
                done.fail();
            });

        setTimeout(() => {
            done();
        }, 50);
    });
});

describe('isReady()', () => {
    it('resolves if there is a connection', async () => {
        const subject = new BehaviorSubject(true);
        mockReady.mockImplementationOnce(() => subject);

        await isReady();
    });

    it('never resolves if there is no connection', done => {
        const subject = new Subject();
        mockReady.mockImplementationOnce(() => subject);

        isReady()
            .then(() => {
                done.fail();
            });

        setTimeout(() => {
            done();
        }, 50);
    });
});
