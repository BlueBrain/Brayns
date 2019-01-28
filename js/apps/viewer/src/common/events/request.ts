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
export const REQUEST_DONE = 'requestdone';
export const REQUEST_CANCEL = 'requestcancel';


export function onRequestStart() {
    return addEventObserver<Request>(REQUEST_START);
}

export function onRequestProgress() {
    return addEventObserver<ProgressEvent>(REQUEST_PROGRESS);
}

export function onRequestDone() {
    return addEventObserver<RequestDoneEvent>(REQUEST_DONE);
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
    return (error: boolean = false) => {
        dispatchEvent(REQUEST_DONE, {
            request,
            error
        });
    };
}

export interface ProgressEvent extends Progress {
    id: string | number;
}

export interface RequestDoneEvent {
    error?: any;
    request: Request;
}
