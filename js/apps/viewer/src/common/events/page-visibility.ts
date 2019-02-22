import {empty, fromEvent} from 'rxjs';
import {map} from 'rxjs/operators';

// TODO: Use context to manage this
/**
 * Observe page visibility
 * @see https://developer.mozilla.org/en-US/docs/Web/API/Page_Visibility_API
 */
export function onPageVisibilityChange() {
    const events = getPageVisibilityEventNames();
    if (events) {
        return fromEvent(document, events.visibilityChange)
            .pipe(map(() => (document as any)[events.hidden]));
    }
    return empty();
}

export function getPageVisibilityEventNames() {
    if (typeof document.hidden !== 'undefined') { // Opera 12.10 and Firefox 18 and later support
        return {
            hidden: 'hidden',
            visibilityChange: 'visibilitychange'
        };
    } else if (typeof (document as any).msHidden !== 'undefined') {
        return {
            hidden: 'msHidden',
            visibilityChange: 'msvisibilitychange'
        };
    } else if (typeof (document as any).webkitHidden !== 'undefined') {
        return {
            hidden: 'webkitHidden',
            visibilityChange: 'webkitvisibilitychange'
        };
    }
    return;
}
