import {ApplicationParameters} from 'brayns';
import {
    addEventObserver,
    dispatchEvent
} from './pubsub';


export const APP_PARAMS_CHANGE = 'appparamschange';
export type AppParamsChange = Partial<ApplicationParameters>;


export function onAppParamsChange() {
    return addEventObserver<AppParamsChange>(APP_PARAMS_CHANGE);
}

export function dispatchAppParams(camera: AppParamsChange) {
    dispatchEvent(APP_PARAMS_CHANGE, camera);
}
