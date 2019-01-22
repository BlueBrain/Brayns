import {BoundingBox} from 'brayns';
import {Vector3} from 'three';


/**
 * Get center of bounding box
 */
// TODO: Add tests
export function getBoundingBoxCenter(box: BoundingBox) {
    const min = new Vector3(...box.min);
    const max = new Vector3(...box.max);
    const center = getPointAtDistance(min, max, 0.5);

    const diff = max.sub(min);
    const z = Math.max(...diff.toArray());

    const target = center.clone()
        .setZ(z);

    return {
        center,
        target
    };
}


/**
 * Convert degree to radian
 */
export function degToRad(deg: number) {
    return deg * Math.PI / 180;
}


/**
 * Get the coords of a point on a line at some given percentage of the distance
 * f(t) = A + t * N;
 *
 * @see https://stackoverflow.com/a/27430674/1092007
 *
 * @param a
 * @param b
 * @param percentage
 */
// TODO: Add tests
function getPointAtDistance(a: Vector3, b: Vector3, percentage: number) {
    const dir = b.clone()
        .sub(a);
    const length = dir.length();
    dir.normalize()
        .multiplyScalar(length * percentage);
    return a.clone()
        .add(dir);
}
