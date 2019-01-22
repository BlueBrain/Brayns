import {
    compareAnimationParams,
    currentFrame,
    framesCount,
    frameToTimeStr,
    hasAnimation
} from './utils';

describe('compareAnimationParams()', () => {
    it('should return true if two frame objects are the same', () => {
        expect(compareAnimationParams({}, {})).toBe(true);
        expect(compareAnimationParams({current: 0}, {current: 0})).toBe(true);
        expect(compareAnimationParams({delta: 1}, {delta: 1})).toBe(true);
        expect(compareAnimationParams({unit: undefined}, {unit: undefined})).toBe(true);
        expect(compareAnimationParams({dt: 0.1}, {dt: 0.1})).toBe(true);
        expect(compareAnimationParams({end: 100, start: 0}, {end: 100, start: 0})).toBe(true);
    });

    it('should return false otherwise', () => {
        expect(compareAnimationParams(undefined, undefined)).toBe(false);
        expect(compareAnimationParams({current: undefined}, {current: 0})).toBe(false);
        expect(compareAnimationParams({delta: 1}, {delta: 10})).toBe(false);
        expect(compareAnimationParams({unit: 'ms'}, {unit: undefined})).toBe(false);
        expect(compareAnimationParams({dt: 0.1}, {dt: 1})).toBe(false);
        expect(compareAnimationParams({end: 100, start: 0}, {end: 10, start: 0})).toBe(false);
    });
});

describe('currentFrame()', () => {
    it('should return the current frame', () => {
        const frame = currentFrame({current: 100});
        expect(frame).toBe(100);
    });

    it('should return 0 otherwise', () => {
        const frame = currentFrame({});
        expect(frame).toBe(0);
    });
});

describe('framesCount()', () => {
    it('should return the frames count', () => {
        const count = framesCount({start: 0, end: 100});
        expect(count).toBe(100);
    });

    it('should return 0 otherwise', () => {
        expect(framesCount({end: 100})).toBe(0);
        expect(framesCount({start: 100})).toBe(0);
        expect(framesCount({})).toBe(0);
    });
});

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

describe('hasAnimation()', () => {
    it('should return true if there are frames or false otherwise', () => {
        expect(hasAnimation({start: 0, end: 100})).toBe(true);
        expect(hasAnimation({start: 10, end: 1})).toBe(false);
        expect(hasAnimation({start: 0, end: 0})).toBe(false);
        expect(hasAnimation({start: 0})).toBe(false);
        expect(hasAnimation({end: 0})).toBe(false);
        expect(hasAnimation({})).toBe(false);
        expect(hasAnimation(undefined)).toBe(false);
    });
});
