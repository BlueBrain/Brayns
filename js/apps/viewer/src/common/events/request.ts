import {
    Progress,
    Request,
    RequestTask
} from 'rockets-client';
import {filter} from 'rxjs/operators';
import {
    addEventObserver,
    dispatchEvent
} from './pubsub';


export const REQUEST_START = 'requeststart';
export const REQUEST_PROGRESS = 'requestprogress';
export const REQUEST_COMPLETE = 'requestcomplete';
export const REQUEST_CANCEL = 'requestcancel';


export function onRequestStart() {
    return addEventObserver<Request>(REQUEST_START);
}

export function onRequestProgress() {
    return addEventObserver<ProgressEvent>(REQUEST_PROGRESS);
}

export function onRequestComplete() {
    return addEventObserver<Request>(REQUEST_COMPLETE);
}

export function onRequestCancel<P = any, R = any>(task: RequestTask<P, R>) {
    return addEventObserver<Request>(REQUEST_CANCEL)
        .pipe(filter(r => r.id === task.request.id));
}


export function dispatchRequestCancel(request: Request) {
    dispatchEvent(REQUEST_CANCEL, request);
}


export function dispatchRequest<P, R>(task: RequestTask<P, R>) {
    const {request} = task;
    const done = createDone(request);

    dispatchEvent(REQUEST_START, request);

    task.on('progress')
        .subscribe({
            next: progress => {
                dispatchEvent(REQUEST_PROGRESS, {
                    id: request.id,
                    ...progress
                });
            },
            complete: done,
            error: done
        });
}

function createDone(request: Request) {
    return () => {
        dispatchEvent(REQUEST_COMPLETE, request);
    };
}

export interface ProgressEvent extends Progress {
    id: string | number;
}
