import {Client} from 'brayns';
import {from} from 'rxjs';
import {filter, map, take} from 'rxjs/operators';
import {getHostFromUrl} from './url';

const host = getHostFromUrl();
const brayns = new Client(host);

export {brayns as default};

export function onReady() {
    return brayns.ready.pipe(filter(ready => ready));
}

export function ifReady<T>(result: T) {
    return from(isReady())
        .pipe(
            filter(ready => ready),
            map(() => result));
}

export async function isReady() {
    const ready = await brayns.ready.pipe(take(1))
        .toPromise();
    return ready;
}
