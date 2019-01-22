import {Observable, Subject} from 'rxjs';

const observers = new Map<string, Subject<any>>();

export function dispatchEvent<T>(type: string, data?: T) {
    if (observers.has(type)) {
        const subject = observers.get(type) as Subject<T>;
        subject.next(data);
    }
}

export function addEventObserver<T>(type: string): Observable<T> {
    if (observers.has(type)) {
        const subject = observers.get(type) as Subject<T>;
        return subject.asObservable();
    }
    const subject = new Subject<T>();
    observers.set(type, subject);
    return subject.asObservable();
}
