import {AnimationParameters} from 'brayns';
import {isNumber, isObject} from 'lodash';

/**
 * Compare two AnimationParameters onjects
 * @param prev
 * @param current
 */
export function compareAnimationParams(prev?: Partial<AnimationParameters>, current?: Partial<AnimationParameters>): boolean {
    return isObject(prev)
        && isObject(current)
        && compareAnimationParamsValues(prev!, current!);
}

function compareAnimationParamsValues(prev: Partial<AnimationParameters>, current: Partial<AnimationParameters>) {
    return prev.current === current.current
        && prev.delta === current.delta
        && prev.end === current.end
        && prev.start === current.start
        && prev.dt === current.dt
        && prev.unit === current.unit;
}

/**
 * Get the total count of frames
 * @param params
 */
export function framesCount(params: Partial<AnimationParameters>): number {
    if (isNumber(params.start) && isNumber(params.end)) {
        return params.end - params.start;
    }
    return 0;
}

export function hasAnimation(params?: Partial<AnimationParameters>) {
    if (params) {
        const count = framesCount(params);
        return count > 0;
    }
    return false;
}

/**
 * Get the current frame
 * @param params
 */
export function currentFrame(params: Partial<AnimationParameters>): number {
    if (isNumber(params.current)) {
        return params.current;
    }
    return 0;
}

/**
 * Convert a frame to time str based on the delta
 * @param frame
 * @param dt
 */
export function frameToTimeStr(frame?: number, dt?: number): string {
    if (isNumber(frame) && isNumber(dt)) {
        return (frame * dt).toFixed(2);
    }
    return '';
}
