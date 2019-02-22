import uid from 'crypto-uid';
import {isString} from 'lodash';
import {JsonRpcError} from 'rockets-client';
import {
    addEventObserver,
    dispatchEvent
} from './pubsub';


// TODO: Use context to manage this
export const NOTIFICATION = 'notification';

export function onNotification() {
    return addEventObserver<Notification>(NOTIFICATION);
}

export function dispatchNotification(params: NotificationParams) {
    dispatchEvent(NOTIFICATION, {
        id: uid(),
        message: isString(params)
            ? params
            : params.message
    });
}


export type NotificationParams = string | JsonRpcError;

export interface Notification {
    id: string;
    message: string;
}
