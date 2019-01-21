import {BoundingBox, Camera} from 'brayns';
import {isNumber} from 'lodash';
import {Quaternion, Vector3} from 'three';

import {
    fromAxisAngle,
    toAxisAngle as asAxisAngle
} from './quaternion';
import {
    degToRad,
    getBoundingBoxCenter
} from './utils';


const FORWARD = new Vector3(0, 0, -1);

// Enum
const Axis = {
    X: [1, 0, 0],
    Y: [0, 1, 0],
    Z: [0, 0, 1]
};

const rotations = new Map<Face, Quaternion>([
    ['front', fromAxisAngle([...Axis.X, 0])],
    ['back', fromAxisAngle([...Axis.Y, Math.PI])],
    ['left', fromAxisAngle([...Axis.Y, Math.PI / -2])],
    ['right', fromAxisAngle([...Axis.Y, Math.PI / 2])],
    ['top', fromAxisAngle([...Axis.X, Math.PI / -2])],
    ['bottom', fromAxisAngle([...Axis.X, Math.PI / 2])]
]);

const axes = new Map([
    ['x', Axis.X],
    ['y', Axis.Y],
    ['z', Axis.Z]
]);


/**
 * Rotate camera to the center of a bounding box
 * @param bb
 */
// TODO: Add tests
export function rotateToBoundingBox(camera: CameraProps, bb: BoundingBox): CameraProps {
    const orientation = new Quaternion(0, 0, 0, 1);
    const {center, target} = getBoundingBoxCenter(bb);

    const position = getPositionForRotation({
        ...camera,
        target: target.toArray(),
        position: FORWARD.toArray()
    }, orientation);

    return {
        position: position.toArray(),
        orientation: orientation.toArray(),
        target: center.toArray()
    };
}


/**
 * Rotate camera to face a specific target
 * @param camera
 * @param target
 */
// TODO: Add tests
export function rotateToTarget(camera: CameraProps, target: Face): CameraProps {
    const orientation = rotations.get(target)!
        .toArray();
    return setCameraRotation(camera, orientation);
}


/**
 * Set camera rotation
 * @param camera
 * @param quaternion
 */
// TODO: Add tests
export function setCameraRotation(camera: CameraProps, quaternion: number[]): CameraProps {
    const orientation = new Quaternion(...quaternion);
    const position = getPositionForRotation(camera, orientation);

    return {
        position: position.toArray(),
        orientation: orientation.toArray(),
        target: camera.target
    };
}


/**
 * Rotate camera on {x, y, z} with n degrees
 * @param camera
 * @param rotation
 */
// TODO: Add tests
export function rotateCamera(camera: CameraProps, rotation: Rotation): CameraProps {
    const orientation =  new Quaternion(...camera.orientation);

    const rotations = Object.entries(rotation)
        .filter(([, value]) => isNumber(value));

    for (const [axis, value] of rotations) {
        const angle = degToRad(value);
        const rotation = fromAxisAngle([...axes.get(axis), angle]);
        orientation.multiply(rotation);
    }

    const position = getPositionForRotation(camera, orientation);

    return {
        position: position.toArray(),
        orientation: orientation.toArray(),
        target: camera.target
    };
}


// TODO: Add tests
/**
 * Get camera rotation from current camera coordinates as axis angle
 * @see https://developer.mozilla.org/en-US/docs/Web/CSS/transform-function/rotate3d
 */
export function toAxisAngle(camera: CameraProps) {
    const angle = asAxisAngle(camera.orientation);
    return angle;
}


function getPositionForRotation(camera: CameraProps, orientation: Quaternion) {
    const position = new Vector3(...camera.position);
    const center = camera.target
        ? new Vector3(...camera.target)
        : new Vector3(0, 0, 0);

    const dir = FORWARD.clone();
    dir.applyQuaternion(orientation);

    const radius = position.clone()
        .sub(center)
        .length();

    return dir.negate()
        .multiplyScalar(radius)
        .add(center);
}


export type CameraProps = Pick<Camera, 'position' | 'orientation' | 'target'>;

export interface Rotation {
    x?: number;
    y?: number;
    z?: number;
}

export type Face = 'front'
    | 'back'
    | 'right'
    | 'left'
    | 'top'
    | 'bottom';
