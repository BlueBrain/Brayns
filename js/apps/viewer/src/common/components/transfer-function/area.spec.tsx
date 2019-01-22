import React from 'react';

import {createMount} from '@material-ui/core/test-utils';
import Area, {createLineIntercept} from './area';
import {Provider} from './provider';
import {
    comparePathD,
    createPoint,
    createRect
} from './testing';
import {PointCoords} from './types';


it('renders without crashing', () => {
    const mount = createMount();
    const canvas = createRect();
    const component = mount(
        <svg>
            <Provider value={{canvas}}>
                <Area colormap={[]} points={[]} />
            </Provider>
        </svg>
    );
    component.unmount();
    mount.cleanUp();
});

it('renders a <path> node', () => {
    const mount = createMount();
    const canvas = createRect();
    const component = mount(
        <svg>
            <Provider value={{canvas}}>
                <Area colormap={[]} points={[]} />
            </Provider>
        </svg>
    );

    const path = component.find('path');
    expect(path).toHaveLength(1);

    component.unmount();
    mount.cleanUp();
});

it('renders a <path> with {d} which M starts at x = 0, y = canvas rect height and ends at x = canvas width, y = canvas height', () => {
    const mount = createMount();
    const canvas = createRect(100, 100);

    const points = [createPoint(0, 0)];
    const component = mount(
        <svg>
            <Provider value={{canvas}}>
                <Area colormap={[]} points={points} />
            </Provider>
        </svg>
    );

    const path = component.find('path');
    expect(comparePathD(path.prop('d'), 'M 0,100 L 0,0 L 100,100 z')).toBe(true);

    component.unmount();
    mount.cleanUp();
});

it('renders a <path> based on provided points', () => {
    const mount = createMount();
    const canvas = createRect(100, 100);

    const points = [
        createPoint(0, 0),
        createPoint(0.5, 0.5)
    ];
    const component = mount(
        <svg>
            <Provider value={{canvas}}>
                <Area colormap={[]} points={points} />
            </Provider>
        </svg>
    );

    const path = component.find('path');
    expect(comparePathD(path.prop('d'), 'M 0,100 L 0,0 L 50,50 L 100,100 z')).toBe(true);

    component.unmount();
    mount.cleanUp();
});

it('renders a <linearGradient> node', () => {
    const mount = createMount();
    const canvas = createRect();
    const component = mount(
        <svg>
            <Provider value={{canvas}}>
                <Area colormap={[]} points={[]} />
            </Provider>
        </svg>
    );

    const gradient = component.find('linearGradient');
    expect(gradient).toHaveLength(1);

    component.unmount();
    mount.cleanUp();
});

it('uses the <linearGradient> as fill for the <path>', () => {
    const mount = createMount();
    const canvas = createRect();
    const component = mount(
        <svg>
            <Provider value={{canvas}}>
                <Area colormap={[]} points={[]} />
            </Provider>
        </svg>
    );

    const path = component.find('path');
    const gradient = component.find('linearGradient');

    expect(path.prop('fill')).toEqual(`url(#${gradient.prop('id')})`);

    component.unmount();
    mount.cleanUp();
});

it('renders a <linearGradient> with <stop> for each colormap value', () => {
    const mount = createMount();
    const canvas = createRect();

    const colormap = ['#000', '#222', '#fefefe', '#fff'];
    const points = [
        createPoint(0, 0),
        createPoint(0.25, 0.25),
        createPoint(0.75, 0.75),
        createPoint(1, 1)
    ];

    const component = mount(
        <svg>
            <Provider value={{canvas}}>
                <Area colormap={colormap} points={points} />
            </Provider>
        </svg>
    );

    const stops = component.find('stop');
    expect(stops).toHaveLength(colormap.length);

    const children = [
        stops.at(0),
        stops.at(1),
        stops.at(2),
        stops.at(3)
    ];

    for (const [index, stop] of children.entries()) {
        const color = colormap[index];
        const offset = index / (colormap.length - 1);
        expect(stop.prop('offset')).toBe(offset);
        expect(stop.prop('stopColor')).toBe(color);
        expect(stop.prop('stopOpacity')).toBe(1 - offset);
    }

    component.unmount();
    mount.cleanUp();
});

describe('createLineIntercept()', () => {
    it('returns a function', () => {
        const a = {x: 0, y: 0};
        const b = {x: 1, y: 1};
        expect(createLineIntercept(a, b)).toBeInstanceOf(Function);
    });

    it('returns the same value for {y} as the given "x" if the line is parallel with the X axis', () => {
        const a = {x: 0, y: 0.25};
        const b = {x: 0.5, y: 0.25};

        const intersect = createLineIntercept(a, b);

        const target = 0.25;
        const {x, y} = intersect(target);

        verify(target, a, {x, y});

        expect(x).toBe(target);
        expect(y).toBe(0.25);
    });

    it('computes {x, y} for given x', () => {
        const a = {x: 0.25, y: 0.45};
        const b = {x: 0.85, y: 0.9};

        const intersect = createLineIntercept(a, b);

        const target = 0.60;
        const {x, y} = intersect(target);

        verify(target, a, {x, y});

        expect(x).toBeCloseTo(target);
        expect(y).toBeCloseTo(0.7125);
    });

    it('returns same value for {y} as the given "x" if the line forms a 45˚ angle', () => {
        const a = {x: 0.25, y: 0.25};
        const b = {x: 1, y: 1};

        const intersect = createLineIntercept(a, b);

        const target = 0.5;
        const {x, y} = intersect(target);

        verify(target, a, {x, y});

        expect(x).toBe(target);
        expect(y).toBe(0.5);
    });

    it('returns {x: NaN, y: NaN} when target is outside the coords', () => {
        const a = {x: 0.5, y: 0.5};
        const b = {x: 0.85, y: 0.75};

        const intersect = createLineIntercept(a, b);

        const target = 0.25;
        const {x, y} = intersect(target);

        expect(x).toBeNaN();
        expect(y).toBeNaN();
    });

    it('returns the origin when the target is at the origin', () => {
        const a = {x: 0.5, y: 0.5};
        const b = {x: 1, y: 1};

        const intersect = createLineIntercept(a, b);

        const target = 0.5;
        const {x, y} = intersect(target);

        expect(x).toBe(a.x);
        expect(y).toBe(a.y);
    });

    it('returns the destination when the target is at the destination', () => {
        const a = {x: 0.5, y: 0.5};
        const b = {x: 1, y: 1};

        const intersect = createLineIntercept(a, b);

        const target = 1;
        const {x, y} = intersect(target);

        expect(x).toBe(b.x);
        expect(y).toBe(b.y);
    });
});


/**
 * Verify that a point's intersection coords are correct using the Pythagorean theorem
 * @see https://en.wikipedia.org/wiki/Pythagorean_theorem
 */
function verify(target: number, origin: PointCoords, {x, y}: PointCoords) {
    const a = distance({x: origin.x, y: origin.y}, {x: target, y: origin.y});
    const b = distance({x: target, y: origin.y}, {x, y});
    const c = distance({x: origin.x, y: origin.y}, {x, y});

    const p = (a ** 2) + (b ** 2);
    const q = (c ** 2);

    expect(p).toBeCloseTo(q);
}

function distance(a: PointCoords, b: PointCoords) {
    const p = (b.x - a.x) ** 2;
    const q = (b.y - a.y) ** 2;
    return Math.sqrt(p + q);
}
