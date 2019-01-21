// tslint:disable: no-empty
import {isFunction, noop} from 'lodash';

/**
 * Check if current browser supports {passive} event listener options
 * https://github.com/WICG/EventListenerOptions/blob/gh-pages/explainer.md#feature-detection
 * https://developers.google.com/web/tools/lighthouse/audits/passive-event-listeners
 */
export function supportsPassive(): boolean {
    if (isFunction(window.addEventListener)) {
        let passive = false;
        const options = Object.defineProperty({}, 'passive', {
            get() {
                passive = true;
            }
        });

        try {
            window.addEventListener('testPassive', noop, options);
            window.removeEventListener('testPassive', noop, options);
        } catch (e) {}

        return passive;
    }
    return false;
}
