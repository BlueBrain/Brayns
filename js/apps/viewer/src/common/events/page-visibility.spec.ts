import {Observable} from 'rxjs';
import {take} from 'rxjs/operators';
import {
    getPageVisibilityEventNames,
    onPageVisibilityChange
} from './page-visibility';


describe('onPageVisibilityChange()', () => {
    it('returns an Observable', () => {
        expect(onPageVisibilityChange()).toBeInstanceOf(Observable);
    });

    it('emits when page visibility changes', done => {
        const restore = mockHidden('hidden', true);

        onPageVisibilityChange()
            .pipe(take(1))
            .subscribe(hidden => {
                expect(hidden).toBe(true);
                restore();
                done();
            });

        const event = new Event('visibilitychange');
        document.dispatchEvent(event);
    });

    it('never emits if page visibility API is not supported', done => {
        const restore = mockHidden();

        onPageVisibilityChange()
            .subscribe(() => {
                done.fail();
            });

        const event = new Event('visibilitychange');
        document.dispatchEvent(event);

        setTimeout(() => {
            restore();
            done();
        }, 50);
    });
});

describe('getPageVisibilityEventNames()', () => {
    it('finds page visibility event names if document has {hidden}', () => {
        const restore = mockHidden('hidden', false);
        const events = getPageVisibilityEventNames();

        expect(events).toEqual({
            hidden: 'hidden',
            visibilityChange: 'visibilitychange'
        });

        restore();
    });

    it('finds page visibility event names if document has {msHidden}', () => {
        const restore = mockHidden('msHidden', false);
        const events = getPageVisibilityEventNames();

        expect(events).toEqual({
            hidden: 'msHidden',
            visibilityChange: 'msvisibilitychange'
        });

        restore();
    });

    it('finds page visibility event names if document has {webkitHidden}', () => {
        const restore = mockHidden('webkitHidden', false);
        const events = getPageVisibilityEventNames();

        expect(events).toEqual({
            hidden: 'webkitHidden',
            visibilityChange: 'webkitvisibilitychange'
        });

        restore();
    });

    it('returns undefined otherwise', () => {
        const restore = mockHidden();
        const events = getPageVisibilityEventNames();
        expect(events).toBeUndefined();
        restore();
    });
});


function mockHidden(key: 'hidden' | 'msHidden' | 'webkitHidden' = 'hidden', value?: boolean) {
    const doc: any = (global as any).document;

    const restore = removeHidden();

    const hidden = doc[key];
    Object.defineProperty(doc, key, {
        value,
        configurable: true,
        writable: true
    });

    return () => {
        Object.defineProperty(doc, key, {
            value: hidden
        });
        restore();
    };
}

function removeHidden() {
    const doc: any = (global as any).document;
    const hidden = doc.hidden;
    Object.defineProperty(doc, 'hidden', {
        value: undefined,
        configurable: true,
        writable: true
    });
    return () => {
        Object.defineProperty(doc, 'hidden', {
            value: hidden
        });
    };
}
