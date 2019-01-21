import {
    camelCase,
    isObject,
    snakeCase
} from 'lodash';

import {interval, Observable} from 'rxjs';
import {take} from 'rxjs/operators';


export function getFileDesc(file: File): FileDescription {
    const type = getFileExt(file);
    const path = file.name;
    const name = path.replace(`.${type}`, '');
    return {name, path, type, size: file.size};
}

export function getFileExt(file: File): string {
    const {name} = file;
    const index = name.lastIndexOf('.');
    if (index !== -1) {
        const extension = name.substr(index + 1);
        return extension;
    }
    return '';
}


export function timeout(delay: number): Observable<number> {
    return interval(delay)
        .pipe(take(1));
}


const mapToCamelCase = createObjectMapper(camelCase, toCamelCase);
const mapToSnakeCase = createObjectMapper(snakeCase, toSnakeCase);

export function toCamelCase(obj: any): any {
    if (Array.isArray(obj)) {
        return obj.map(toCamelCase);
    } else if (isObject(obj)) {
        return mapToCamelCase(obj);
    }
    return obj;
}

export function toSnakeCase(obj: any): any {
    if (Array.isArray(obj)) {
        return obj.map(toSnakeCase);
    } else if (isObject(obj)) {
        return mapToSnakeCase(obj);
    }
    return obj;
}

function createObjectMapper(keyFn: (key: string) => string, valueFn: (value: any) => any) {
    return (obj: AbstractObj) => {
        const result: AbstractObj = {};
        const keys = Object.keys(obj);

        for (const k of keys) {
            const key = keyFn(k);
            const value = obj[k];
            result[key] = valueFn(value);
        }

        return result;
    };
}

export interface FileDescription {
    name: string;
    path: string;
    type: string;
    size: number;
}

interface AbstractObj {
    [key: string]: any;
}
