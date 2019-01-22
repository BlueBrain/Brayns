import {degToRad} from './utils';

describe('degToRad()', () => {
    it('converts degrees to radians', () => {
        expect(degToRad(0)).toBe(0);
        expect(degToRad(45)).toBe(Math.PI / 4);
        expect(degToRad(90)).toBe(Math.PI / 2);
        expect(degToRad(180)).toBe(Math.PI);
        expect(degToRad(270)).toBe(Math.PI * 3 / 2);
        expect(degToRad(360)).toBe(Math.PI * 2);
    });
});
