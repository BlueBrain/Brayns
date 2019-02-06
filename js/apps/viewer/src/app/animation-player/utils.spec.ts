import {frameToTimeStr} from './utils';

describe('frameToTimeStr()', () => {
    it('should return the time with 2 decimals for the frame as a string', () => {
        const time = frameToTimeStr(25, 0.1);
        expect(time).toBe('2.50');
    });

    it('should return an empty string if the params are not numbers', () => {
        const time = frameToTimeStr(undefined, undefined);
        expect(time).toBe('');
    });
});
