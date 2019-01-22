import uid from 'crypto-uid';
import {BehaviorSubject} from 'rxjs';
import {CanvasRect} from './provider';
import {
    ControlPoint,
    PointCoords
} from './types';

export function createPoint(x: number, y: number, color?: string): ControlPoint {
    return {
        x, y,
        color,
        id: uid(4)
    } as any;
}

export function createPointCoords(x: number, y: number): PointCoords {
    return {
        x, y
    };
}

export function createRect(
    width: number = 0,
    height: number = 0,
    top: number = 0,
    left: number = 0
) {
    return {
        width,
        height,
        top,
        left
    };
}

export function createRectSubject(
    width: number = 0,
    height: number = 0,
    top: number = 0,
    left: number = 0
) {
    const subject = new BehaviorSubject<CanvasRect>({
        width,
        height,
        left,
        top
    });
    return subject;
}

export function comparePathD(a?: string, b?: string) {
    if ((!a && b) || (a && !b)) {
        return false;
    }
    const a0 = a!.replace(/\s/g, '');
    const b0 = b!.replace(/\s/g, '');
    return a0 === b0;
}
