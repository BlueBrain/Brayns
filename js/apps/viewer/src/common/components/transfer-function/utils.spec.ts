import {Theme} from '@material-ui/core';
import {createPoint, createRect} from './testing';
import {
    createToRectScaler,
    getColor,
    getPointCoords,
    isPointOutsideCanvas
} from './utils';


describe('createToRectScaler()', () => {
    it('returns a fn', () => {
        const rect = createRect();
        expect(typeof createToRectScaler(rect)).toBe('function');
    });

    it('scales a point coords to rect size', () => {
        const rect = createRect(100, 100);
        const point = createPoint(0.2, 0.4);

        const toRectScale = createToRectScaler(rect);

        expect(toRectScale(point)).toEqual({
            ...point,
            x: 20,
            y: 40
        });
    });

    it('does nothing if rect is not defined', () => {
        const point = createPoint(0.2, 0.4);
        const toRectScale = createToRectScaler();
        expect(toRectScale(point)).toEqual(point);
    });
});

describe('getColor()', () => {
    it('returns grey A100 for the dark theme', () => {
        const theme: Theme = {
            palette: {
                grey: {
                    A100: 'test'
                },
                type: 'dark'
            }
        } as any;
        expect(getColor(theme)).toBe('test');
    });

    it('returns grey A400 for the light theme', () => {
        const theme: Theme = {
            palette: {
                grey: {
                    A400: 'test'
                },
                type: 'light'
            }
        } as any;
        expect(getColor(theme)).toBe('test');
    });
});

describe('getPointCoords()', () => {
    it('should return {x, y} computed from rect size and mouse evt position', () => {
        const rect = createRect(100, 100);
        const evt = createMouseEvt('mousedown', {
            pageX: 50,
            pageY: 50
        });

        const {x, y} = getPointCoords(evt, rect);

        expect(x).toBe(0.5);
        expect(y).toBe(0.5);
    });

    it('should account for rect position', () => {
        const rect = createRect(100, 100, 20, 20);
        const evt = createMouseEvt('mousedown', {
            pageX: 50,
            pageY: 50
        });

        const {x, y} = getPointCoords(evt, rect);

        expect(x).toBe(0.3);
        expect(y).toBe(0.3);
    });

    it('should account for window scroll offset', () => {
        const rect = createRect(100, 100);
        const restoreScroll = mockScrollOffset(20, 20);
        const evt = createMouseEvt('mousedown', {
            pageX: 50,
            pageY: 50
        });

        const {x, y} = getPointCoords(evt, rect);

        expect(x).toBe(0.3);
        expect(y).toBe(0.3);

        restoreScroll();
    });

    it('should be bounded on min {top, left}', () => {
        const rect = createRect(100, 100, 20, 20);
        const evt = createMouseEvt('mousedown', {
            pageX: 10,
            pageY: 10
        });

        const {x, y} = getPointCoords(evt, rect);

        expect(x).toBe(0);
        expect(y).toBe(0);
    });

    it('should be bounded on max {top, left}', () => {
        const rect = createRect(100, 100, 20, 20);
        const evt = createMouseEvt('mousedown', {
            pageX: 120,
            pageY: 120
        });

        const {x, y} = getPointCoords(evt, rect);

        expect(x).toBe(1);
        expect(y).toBe(1);
    });
});

describe('isPointOutsideCanvas()', () => {
    it('should return false if point is within min/max bounds', () => {
        const rect = createRect(100, 100);
        const evt = createMouseEvt('mousedown', {
            pageX: 50,
            pageY: 50
        });

        const res = isPointOutsideCanvas(evt, rect);
        expect(res).toBe(false);
    });

    it('should account for rect position', () => {
        const rect = createRect(100, 100, 20, 20);
        const evt = createMouseEvt('mousedown', {
            pageX: 50,
            pageY: 50
        });

        const res = isPointOutsideCanvas(evt, rect);
        expect(res).toBe(false);
    });

    it('should account for window scroll offset', () => {
        const rect = createRect(100, 100);
        const restoreScroll = mockScrollOffset(20, 20);
        const evt = createMouseEvt('mousedown', {
            pageX: 50,
            pageY: 50
        });

        const res = isPointOutsideCanvas(evt, rect);
        expect(res).toBe(false);

        restoreScroll();
    });

    it('should return false if evt {x} is outside rect {left}', () => {
        const rect = createRect(100, 100, 20, 20);
        const evt = createMouseEvt('mousedown', {
            pageX: 10,
            pageY: 50
        });

        const res = isPointOutsideCanvas(evt, rect);
        expect(res).toBe(true);
    });

    it('should return false if evt {x} is outside rect {left} + {width}', () => {
        const rect = createRect(100, 100, 20, 20);
        const evt = createMouseEvt('mousedown', {
            pageX: 150,
            pageY: 50
        });

        const res = isPointOutsideCanvas(evt, rect);
        expect(res).toBe(true);
    });

    it('should return false if evt {y} is outside rect {top}', () => {
        const rect = createRect(100, 100, 20, 20);
        const evt = createMouseEvt('mousedown', {
            pageX: 50,
            pageY: 10
        });

        const res = isPointOutsideCanvas(evt, rect);
        expect(res).toBe(true);
    });

    it('should return false if evt {y} is outside rect {top} + {height}', () => {
        const rect = createRect(100, 100);
        const evt = createMouseEvt('mousedown', {
            pageX: 50,
            pageY: 150
        });

        const res = isPointOutsideCanvas(evt, rect);
        expect(res).toBe(true);
    });

    it('should return false if evt {x, y} is outside rect {left, top}', () => {
        const rect = createRect(100, 100, 20, 20);
        const evt = createMouseEvt('mousedown', {
            pageX: 10,
            pageY: 10
        });

        const res = isPointOutsideCanvas(evt, rect);
        expect(res).toBe(true);
    });

    it('should return false if evt {x, y} is outside rect {left, top} + {width, height}', () => {
        const rect = createRect(100, 100, 20, 20);
        const evt = createMouseEvt('mousedown', {
            pageX: 150,
            pageY: 150
        });

        const res = isPointOutsideCanvas(evt, rect);
        expect(res).toBe(true);
    });
});


function createMouseEvt(name: string, props?: Pick<MouseEvent, 'pageX' | 'pageY'>) {
    const evt = new MouseEvent(name, {});
    if (props) {
        Object.assign(evt, props);
    }
    return evt;
}

function mockScrollOffset(x: number, y: number) {
    const offset = {
        pageXOffset: window.pageXOffset,
        pageYOffset: window.pageYOffset
    };

    Object.defineProperty(window, 'pageXOffset', {value: x});
    Object.defineProperty(window, 'pageYOffset', {value: y});

    return () => {
        Object.defineProperty(window, 'pageXOffset', {
            value: offset.pageXOffset
        });
        Object.defineProperty(window, 'pageYOffset', {
            value: offset.pageYOffset
        });
    };
}
