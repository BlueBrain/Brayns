import {Camera} from 'brayns';
import {
    addEventObserver,
    dispatchEvent
} from './pubsub';

export const CAMERA_SETTINGS_CHANGE = 'camerasettingschange';

export const CAMERA_CHANGE = 'camerachange';
export type CameraChange = Pick<Camera, 'position' | 'orientation' | 'target'>;

export function onCameraSettingsChange() {
    return addEventObserver<CameraSettings>(CAMERA_SETTINGS_CHANGE);
}

export function dispatchCameraSettings(settings: CameraSettings) {
    dispatchEvent(CAMERA_SETTINGS_CHANGE, settings);
}

export function onCameraChange() {
    return addEventObserver<CameraChange>(CAMERA_CHANGE);
}

export function dispatchCamera(camera: CameraChange) {
    dispatchEvent(CAMERA_CHANGE, camera);
}

interface CameraSettings {
    sensitivity?: number;
}
