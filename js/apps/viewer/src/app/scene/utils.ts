import {MouseEvent} from 'react';

export function composeEvtHandler(fn: (evt: MouseEvent) => void) {
    return (evt: MouseEvent) => {
        evt.preventDefault();
        evt.stopPropagation();
        fn(evt);
    };
}
