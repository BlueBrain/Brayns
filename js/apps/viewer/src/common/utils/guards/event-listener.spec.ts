import {supportsPassive} from './event-listener';

describe('supportsPassive()', () => {
    it('should return true if {passive} is supported', () => {
        spyOn(window, 'addEventListener').and.callFake((_: string, __: any, opts: any) => {
            // Access the passive options
            if (typeof opts === 'object') {
                opts.passive; // tslint:disable-line: no-unused-expression
            }
        });
        expect(supportsPassive()).toBe(true);
    });

    it('should return false if {passive} is not supported', () => {
        spyOn(window, 'addEventListener').and.callFake(() => {}); // tslint:disable-line: no-empty
        expect(supportsPassive()).toBe(false);
    });
});
