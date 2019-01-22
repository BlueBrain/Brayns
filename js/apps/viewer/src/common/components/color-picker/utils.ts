import {Color} from 'chroma-js';

export function compareColors(current: number[], next: number[]) {
    return current[0] === next[0]
        && current[1] === next[1]
        && current[2] === next[2];
}

// For white, black and grey,
// h is NaN
export function toHsv(color: Color) {
    const [h, s, v] = color.hsv();
    return [
        Number.isNaN(h) ? 0 : h,
        s,
        v
    ];
}
