import {
    addEventObserver,
    dispatchEvent
} from './pubsub';

export const VIEWPORT_CHANGE = 'viewportchange';

export function onViewportChange() {
    return addEventObserver<number[]>(VIEWPORT_CHANGE);
}

export function dispatchViewport(viewport: number[]) {
    dispatchEvent(VIEWPORT_CHANGE, viewport);
}
