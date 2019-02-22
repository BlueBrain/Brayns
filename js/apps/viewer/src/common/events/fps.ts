import {
    addEventObserver,
    dispatchEvent
} from './pubsub';

// TODO: Use context to manage this
export const FPS_CHANGE = 'renderingfpschange';

export function onFpsChange() {
    return addEventObserver<number>(FPS_CHANGE);
}

export function dispatchFps(fps: number) {
    dispatchEvent(FPS_CHANGE, fps);
}
