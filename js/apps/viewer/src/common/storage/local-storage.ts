import {isString} from 'lodash';

export default class LocalStorage {
    static create(ns: string) {
        return new LocalStorage(ns);
    }

    constructor(readonly ns: string) {}

    set(item: string, data: any): this {
        const key = this.key(item);
        const json = JSON.stringify(data);
        localStorage.setItem(key, json);
        return this;
    }

    get<T = any>(item: string): T | null {
        const key = this.key(item);
        const json = localStorage.getItem(key);
        if (isString(json)) {
            try {
                const data = JSON.parse(json);
                return data;
            } catch {} // tslint:disable-line: no-empty
        }
        return null;
    }

    remove(item: string): this {
        const key = this.key(item);
        localStorage.removeItem(key);
        return this;
    }

    clear(): this {
        for (const key in localStorage) {
            if (key.startsWith(this.ns)) {
                localStorage.removeItem(key);
            }
        }
        return this;
    }

    private key(item: string): string {
        return `${this.ns}:${item}`;
    }
}
