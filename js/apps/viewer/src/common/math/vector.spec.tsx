import {
    compareVectors,
    vector3Of
} from './vector';

describe('compareVectors()', () => {
    it('should compare two arrays for equality', () => {
        const a = [0, 1, 2];
        const b = [0, 1, 2];
        expect(compareVectors(undefined, undefined)).toBe(true);
        expect(compareVectors(a, a)).toBe(true);
        expect(compareVectors(a, b)).toBe(true);
        expect(compareVectors(a, undefined)).toBe(false);
        expect(compareVectors(undefined, a)).toBe(false);
        expect(compareVectors(a, [])).toBe(false);
    });
});

describe('vector3Of()', () => {
    it('generates an array of 3 numbers', () => {
        expect(vector3Of(1)).toEqual([1, 1, 1]);
    });
});
