import {
    addEventObserver,
    dispatchEvent
} from './pubsub';

// TODO: Use context to manage this
export const KEYBOARD_LOCK_CHANGE = 'keyboardlockchange';

export function onKeyboardLockChange() {
    return addEventObserver<boolean>(KEYBOARD_LOCK_CHANGE);
}

export function dispatchKeyboardLock(lock: boolean) {
    dispatchEvent(KEYBOARD_LOCK_CHANGE, lock);
}
