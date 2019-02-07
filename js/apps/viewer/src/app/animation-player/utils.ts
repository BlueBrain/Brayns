import {isNumber} from 'lodash';

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
