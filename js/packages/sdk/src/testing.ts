import {pick} from 'lodash';
import {JsonRpcRequest} from 'rockets-client';

export function toResponseJson(request: JsonRpcRequest, result: any) {
    return toJson({
        ...pick(request, ['jsonrpc', 'id']),
        result
    });
}

export function toResponseErrorJson(request: JsonRpcRequest, error: any) {
    return toJson({
        ...pick(request, ['jsonrpc', 'id']),
        error
    });
}

export function toJson<T = any>(data: T): string {
    return JSON.stringify(data);
}

export function fromJson<T>(json: string): T {
    const data = JSON.parse(json);
    return data;
}
