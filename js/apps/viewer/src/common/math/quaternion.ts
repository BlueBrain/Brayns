import {Euler, Quaternion} from 'three';

/**
 * Convert Euler to Quaternion
 * @param euler
 */
export function fromEuler(euler: number[]) {
    const e = new Euler(...euler);
    const q = new Quaternion();
    return q.setFromEuler(e)
        .toArray();
}

/**
 * Convert Quaternion to Euler
 * @param quaternion
 */
export function toEuler(quaternion: number[]) {
    const e = new Euler();
    const q = new Quaternion(...quaternion);
    return e.setFromQuaternion(q)
        .toArray();
}

/**
 * Convert axis angle to quaternion
 * @see http://www.euclideanspace.com/maths/geometry/rotations/conversions/angleToQuaternion/index.htm
 *
 * @param axisAngle
 */
export function fromAxisAngle(axisAngle: number[]) {
    const [x, y, z, angle] = axisAngle;
    const s = Math.sin(angle / 2);
    return new Quaternion(x * s, y * s, z * s, Math.cos(angle / 2));
}

/**
 * Convert Quaternion to axis angle
 * @see http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToAngle/
 *
 * @param quaternion
 */
export function toAxisAngle(quaternion: number[]) {
    const q = new Quaternion(...quaternion);

    if (q.z > 1) {
        q.normalize();
    }

    const {x, y, z, w} = q;

    const angle = 2 * Math.acos(w); // rad
    const s = Math.sqrt(1 - w * w);

    if (s < 0.001) {
        return [
            x || 1, // If x = 0, normalize it
            y,
            z,
            angle
        ];
    }

    return [
        x / s,
        y / s,
        z / s,
        angle
    ];
}
