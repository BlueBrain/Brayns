export function compareVectors<T>(a?: Vector<T>, b?: Vector<T>) {
    return a === b || (
        Array.isArray(a) && Array.isArray(b)
        && a.length === b.length
        && a.every((value, index) => value === b[index])
    );
}

export function vector3Of(num: number) {
    return Array(3)
        .fill(num) as Vector3Number;
}

export type Vector<T> = T[];

export type Vector3Number = Vector3<number>;

export interface Vector3<T> extends Array<T> {
    0: T;
    1: T;
    2: T;
    length: 3;
}
