import React from 'react';

import chroma, {distance, Scale} from 'chroma-js';
import {noop} from 'lodash';
import {BehaviorSubject} from 'rxjs';

import {PaletteType, Theme} from '@material-ui/core';
import {
    createMount,
    createShallow,
    getClasses
} from '@material-ui/core/test-utils';

import Area from './area';
import Path from './path';
import Point from './point';
import {CanvasRect} from './provider';
import TransferFunction, {POINT_SIZE} from './transfer-function';
import {ControlPoint, PointCoords} from './types';
import {getColor} from './utils';

import {
    createPoint,
    createPointCoords,
    createRect,
    createRectSubject
} from './testing';


const RECT_SIZE = 100;
const canvas = {
    width: 68,
    height: 68,
    left: 24,
    top: 8
};


it('renders without crashing', () => {
    const shallow = createShallow();
    const component = shallow(
        <TransferFunction
            data={[]}
            colormap={[]}
            range={[]}
        />
    );
    component.unmount();
});

it('creates a color interpolator from the {colormap} prop', () => {
    const shallow = createShallow({dive: true});

    const data = [
        createPointCoords(0, 0),
        createPointCoords(1, 1)
    ];
    const colormap = ['#fff', '#222'];

    const component = shallow(
        <TransferFunction
            data={data}
            colormap={colormap}
            range={[]}
        />
    );

    const interpolator = component.dive()
        .state<Scale>('interpolate');

    expect(typeof interpolator).toBe('function');

    expect(interpolator(0)
        .hex()).toBe('#ffffff');
    expect(interpolator(1)
        .hex()).toBe('#222222');

    component.unmount();
});

it('maps each of the PointCoords in {data} prop to a ControlPoint', () => {
    const shallow = createShallow({dive: true});

    const data = [
        createPointCoords(0, 0),
        createPointCoords(1, 1)
    ];
    const colormap = ['#fff', '#222'];

    const component = shallow(
        <TransferFunction
            data={data}
            colormap={colormap}
            range={[]}
        />
    );

    const points = component.dive()
        .state<ControlPoint[]>('points');

    expect(points).toHaveLength(data.length);

    const [a] = points;
    expect(a.x).toBe(0);
    expect(a.y).toBe(1);
    expect(compareColor(a.color, colormap[0])).toBe(true);
    expect(a.id).toBeDefined();

    const [, b] = points;
    expect(b.x).toBe(1);
    expect(b.y).toBe(0);
    expect(compareColor(b.color, colormap[1])).toBe(true);
    expect(b.id).toBeDefined();

    component.unmount();
});

it('updates the color interpolator when the {colormap} prop changes', () => {
    const shallow = createShallow({dive: true});

    const data = [
        createPointCoords(0, 0),
        createPointCoords(1, 1)
    ];
    const colormap = ['#fff', '#222'];

    const component = shallow(
        <TransferFunction
            data={data}
            colormap={colormap}
            range={[]}
        />
    );

    component.setProps({
        colormap: ['#fff', '#000']
    });

    const interpolator = component.dive()
        .state<Scale>('interpolate');

    expect(typeof interpolator).toBe('function');

    expect(interpolator(0)
        .hex()).toBe('#ffffff');
    expect(interpolator(1)
        .hex()).toBe('#000000');

    component.unmount();
});

it('updates the control points when the {data} prop changes', () => {
    const shallow = createShallow({dive: true});

    const data = [
        createPointCoords(0, 0),
        createPointCoords(1, 1)
    ];
    const colormap = ['#fff', '#222'];

    const component = shallow(
        <TransferFunction
            data={data}
            colormap={colormap}
            range={[]}
        />
    );

    component.setProps({
        data: [
            createPointCoords(0, 0),
            createPointCoords(0.5, 0.5),
            createPointCoords(1, 1)
        ]
    });

    const points = component.dive()
        .state<ControlPoint[]>('points');

    expect(points).toHaveLength(3);

    const [a] = points;
    expect(a.x).toBe(0);
    expect(a.y).toBe(1);
    expect(compareColor(a.color, colormap[0])).toBe(true);
    expect(a.id).toBeDefined();

    const [, b] = points;
    expect(b.x).toBe(0.5);
    expect(b.y).toBe(0.5);
    const scale = chroma.scale(colormap);
    expect(compareColor(b.color, scale(0.5).hex())).toBe(true);
    expect(b.id).toBeDefined();

    const [, , c] = points;
    expect(c.x).toBe(1);
    expect(c.y).toBe(0);
    expect(compareColor(c.color, colormap[1])).toBe(true);
    expect(c.id).toBeDefined();

    component.unmount();
});

it('does not compute a new state if {data, colormap} props are set to the same value', () => {
    const shallow = createShallow({dive: true});

    const data = [
        createPointCoords(0, 0),
        createPointCoords(1, 1)
    ];
    const colormap = ['#fff', '#222'];

    const component = shallow(
        <TransferFunction
            data={data}
            colormap={colormap}
            range={[]}
        />
    );

    const tf = component.dive();

    const points = tf.state('points');

    tf.setProps({
        data: [...data],
        colormap: [...colormap],
        range: []
    });

    expect(tf.state('points')).toEqual(points);

    component.unmount();
});

it('renders an <svg> element', () => {
    const mount = createMount();

    const rect = createRectSubject(RECT_SIZE, RECT_SIZE);
    mockSVGRect(rect);

    const data = [
        createPointCoords(0, 0),
        createPointCoords(1, 1)
    ];
    const colormap = ['#fff', '#222'];
    const component = mount(
        <TransferFunction
            data={data}
            colormap={colormap}
            range={[]}
        />
    );

    const svg = component.find('svg');
    expect(svg).toHaveLength(1);

    expect(svg.prop('width')).toBe(RECT_SIZE);
    expect(svg.prop('height')).toBe(200);

    component.unmount();
    mount.cleanUp();
});

it('computes the paint area from the <svg> bounding box', () => {
    const mount = createMount();
    const range = [0, 1];

    const rect = createRectSubject(RECT_SIZE, RECT_SIZE);
    mockSVGRect(rect);

    const component = mount(
        <TransferFunction
            data={[]}
            colormap={[]}
            range={range}
        />
    );

    const instance: any = component.childAt(0)
        .childAt(0)
        .instance();

    expect(instance.canvas).toEqual(canvas);

    component.unmount();
    mount.cleanUp();
});

it('renders an <Area>', () => {
    const mount = createMount();

    const data = [
        createPointCoords(0, 0),
        createPointCoords(1, 1)
    ];
    const colormap = ['#fff', '#222'];
    const component = mount(
        <TransferFunction
            data={data}
            colormap={colormap}
            range={[]}
        />
    );

    const area = component.find(Area);
    expect(area).toHaveLength(1);

    const points = area.prop('points');
    expect(points).toHaveLength(data.length);

    component.unmount();
    mount.cleanUp();
});

it('renders a <Path>', () => {
    const mount = createMount();

    const data = [
        createPointCoords(0, 0),
        createPointCoords(1, 1)
    ];
    const colormap = ['#fff', '#222'];
    const component = mount(
        <TransferFunction
            data={data}
            colormap={colormap}
            range={[]}
        />
    );

    const path = component.find(Path);
    expect(path).toHaveLength(1);

    const points = path.prop('points');
    expect(points).toHaveLength(data.length);

    component.unmount();
    mount.cleanUp();
});

it('renders a <Point> for each point provided through the {data} prop', () => {
    const mount = createMount();

    const data: PointCoords[] = [
        createPointCoords(0, 0),
        createPointCoords(0.5, 0.5),
        createPointCoords(1, 1)
    ];
    const colormap = ['#fff', '#222'];
    const scale = chroma.scale(colormap);

    const rect = createRectSubject(RECT_SIZE, RECT_SIZE);
    mockSVGRect(rect);

    const component = mount(
        <TransferFunction
            data={data}
            colormap={colormap}
            range={[]}
        />
    );

    const points = component.find(Point);
    expect(points).toHaveLength(3);

    points.forEach(point => {
        expect(point.prop('r')).toBe(POINT_SIZE);
        expect(point.prop('onChange')).toBeDefined();
        expect(point.prop('onRemove')).toBeDefined();
    });

    const a = points.at(0);
    const [d0] = data;
    expect(a.prop('cx')).toBe(scaleX(d0.x));
    expect(a.prop('cy')).toBe(scaleY(d0.y));
    expect(a.prop('color')).toBe(scale(d0.x)
        .hex());

    const b = points.at(1);
    const [, d1] = data;
    expect(b.prop('cx')).toBe(scaleX(d1.x));
    expect(b.prop('cy')).toBe(scaleY(d1.y));
    expect(b.prop('color')).toBe(scale(d1.x)
        .hex());

    const c = points.at(2);
    const [, , d2] = data;
    expect(c.prop('cx')).toBe(scaleX(d2.x));
    expect(c.prop('cy')).toBe(scaleY(d2.y));
    expect(c.prop('color')).toBe(scale(d2.x)
        .hex());

    component.unmount();
    mount.cleanUp();
});

it('renders d3 left axis', () => {
    const mount = createMount();

    const node = (
        <TransferFunction
            data={[]}
            colormap={[]}
            range={[]}
        />
    );
    const classes = getClasses(node);
    const component = mount(node);

    const theme: Theme = component.childAt(0)
        .prop('theme');
    const {typography} = theme;

    const axis = component.render()
        .find(`.${classes.axisLeft}`);

    expect(axis.length).toBe(1);
    expect(axis.prop('font-family')).toBe(typography.fontFamily);
    expect(axis.prop('color')).toBe(getColor(theme));

    const ticks = axis.find('.tick');
    expect(ticks).toHaveLength(11); // [0, 1] range

    component.unmount();
    mount.cleanUp();
});

it('updates d3 left axis when <svg> rect {height} changes', async () => {
    const mount = createMount();

    const rect = createRectSubject(RECT_SIZE, RECT_SIZE);
    mockSVGRect(rect);

    const component = mount(
        <TransferFunction
            data={[]}
            colormap={[]}
            range={[]}
        />
    );

    const instance: any = component.childAt(0)
        .childAt(0)
        .instance();

    const renderAxisSpy = spyOn(instance, 'renderAxisLeft').and.callThrough();

    rect.next(createRect(100, 120));
    await tick(50);
    rect.next(createRect(10, 120));
    await tick(50);
    rect.next(createRect(10, 100));
    await tick(50);
    rect.next(createRect(10, 120));
    await tick(50);

    expect(renderAxisSpy).toHaveBeenCalledTimes(3);

    component.unmount();
    mount.cleanUp();
});

it('updates d3 left axis when {theme} prop changes', () => {
    const mount = createMount();

    const component = mount(
        <TransferFunction
            data={[]}
            colormap={[]}
            range={[]}
        />
    );

    const theme: Theme = component.childAt(0)
        .prop('theme');

    const instance: any = component.childAt(0)
        .childAt(0)
        .instance();

    const renderAxisSpy = spyOn(instance, 'renderAxisLeft').and.callThrough();

    component.setProps(changeTheme(theme, 'dark'));
    component.setProps(changeTheme(theme, 'dark'));
    component.setProps(changeTheme(theme, 'light'));
    component.setProps(changeTheme(theme, 'dark'));

    expect(renderAxisSpy).toHaveBeenCalledTimes(3);

    component.unmount();
    mount.cleanUp();
});

it('renders d3 bottom axis', () => {
    const mount = createMount();
    const range = [0, 1];

    const node = (
        <TransferFunction
            data={[]}
            colormap={[]}
            range={range}
        />
    );
    const classes = getClasses(node);
    const component = mount(node);

    const theme: Theme = component.childAt(0)
        .prop('theme');
    const {typography} = theme;

    const axis = component.render()
        .find(`.${classes.axisBottom}`);

    expect(axis.length).toBe(1);
    expect(axis.prop('font-family')).toBe(typography.fontFamily);
    expect(axis.prop('color')).toBe(getColor(theme));

    const ticks = axis.find('.tick');
    expect(ticks).toHaveLength(11); // [0, 1] range

    component.unmount();
    mount.cleanUp();
});

it('updates d3 bottom axis when {theme} prop changes', () => {
    const mount = createMount();

    const range = [0, 1];

    const component = mount(
        <TransferFunction
            data={[]}
            colormap={[]}
            range={range}
        />
    );

    const theme: Theme = component.childAt(0)
        .prop('theme');

    const instance: any = component.childAt(0)
        .childAt(0)
        .instance();

    const renderAxisSpy = spyOn(instance, 'renderAxisBottom').and.callThrough();

    component.setProps(changeTheme(theme, 'dark'));
    component.setProps(changeTheme(theme, 'dark'));
    component.setProps(changeTheme(theme, 'light'));
    component.setProps(changeTheme(theme, 'dark'));

    expect(renderAxisSpy).toHaveBeenCalledTimes(3);

    component.unmount();
    mount.cleanUp();
});

it('updates d3 bottom axis when {range} prop changes', () => {
    const mount = createMount();

    const range = [0, 1];

    const component = mount(
        <TransferFunction
            data={[]}
            colormap={[]}
            range={range}
        />
    );

    const instance: any = component.childAt(0)
        .childAt(0)
        .instance();

    const renderAxisSpy = spyOn(instance, 'renderAxisBottom').and.callThrough();

    component.setProps({range: [-10, 10]});
    component.setProps({range: [-10, 10]});

    expect(renderAxisSpy).toHaveBeenCalledTimes(1);

    component.unmount();
    mount.cleanUp();
});

it('updates d3 bottom axis when <svg> rect {width} changes', async () => {
    const mount = createMount();

    const rect = createRectSubject(RECT_SIZE, RECT_SIZE);
    mockSVGRect(rect);

    const component = mount(
        <TransferFunction
            data={[]}
            colormap={[]}
            range={[]}
        />
    );

    const instance: any = component.childAt(0)
        .childAt(0)
        .instance();

    const renderAxisSpy = spyOn(instance, 'renderAxisBottom').and.callThrough();

    rect.next(createRect(120, 100));
    await tick(50);
    rect.next(createRect(120, 10));
    await tick(50);
    rect.next(createRect(100, 100));
    await tick(50);
    rect.next(createRect(120, 100));
    await tick(50);

    expect(renderAxisSpy).toHaveBeenCalledTimes(3);

    component.unmount();
    mount.cleanUp();
});

it('caches point update fn', () => {
    const shallow = createShallow({dive: true});
    const component = shallow(
        <TransferFunction
            data={[]}
            colormap={[]}
            range={[]}
        />
    );

    const {createPointUpdater}: any = component.dive()
        .instance();

    const a = createPoint(0, 0);
    const b = createPoint(0, 0);
    const updatePoint = createPointUpdater(a);

    expect(createPointUpdater(a) === updatePoint).toBe(true);
    expect(createPointUpdater(a) === createPointUpdater(b)).toBe(false);

    component.unmount();
});

it('caches point remove fn', () => {
    const shallow = createShallow({dive: true});
    const component = shallow(
        <TransferFunction
            data={[]}
            colormap={[]}
            range={[]}
        />
    );

    const {createPointRemover}: any = component.dive()
        .instance();

    const a = createPoint(0, 0);
    const b = createPoint(0, 0);
    const updatePoint = createPointRemover(a);

    expect(createPointRemover(a) === updatePoint).toBe(true);
    expect(createPointRemover(a) === createPointRemover(b)).toBe(false);

    component.unmount();
});

it('emits changes debounced', async () => {
    const shallow = createShallow({dive: true});

    const data: PointCoords[] = [
        createPointCoords(0, 0),
        createPointCoords(1, 1)
    ];

    const colormap = ['#fff', '#222'];

    const onChangeSpy = jest.fn();
    const component = shallow(
        <TransferFunction
            data={data}
            colormap={colormap}
            range={[]}
            onChange={onChangeSpy}
        />
    );

    const tf = component.dive();
    const points: ControlPoint[] = tf.state('points');
    const {emitChanges} = tf.instance() as any;

    emitChanges();

    // TODO: Test that this also sets the state, then it emits the change

    await tick(150);
    expect(onChangeSpy).not.toHaveBeenCalled();
    await tick(100);
    expect(onChangeSpy).toHaveBeenCalled();

    const [changes] = onChangeSpy.mock.calls[0];
    expect(changes).toEqual(points.map(point => ({
        x: point.x,
        y: 1 - point.y
    })));

    component.unmount();
});

it('does not emit any changes if nothing changed', async () => {
    const shallow = createShallow({dive: true});

    const data: PointCoords[] = [
        createPointCoords(0, 0),
        createPointCoords(1, 1)
    ];

    const colormap = ['#fff', '#222'];

    const onChangeSpy = jest.fn();
    const component = shallow(
        <TransferFunction
            data={data}
            colormap={colormap}
            range={[]}
            onChange={onChangeSpy}
        />
    );

    const tf = component.dive();
    const {emitChanges} = tf.instance() as any;

    emitChanges();
    await tick(250);
    emitChanges();
    await tick(250);

    expect(onChangeSpy).toHaveBeenCalledTimes(1);

    component.unmount();
});

it('destroys rxjs subscribers on unmount', async () => {
    const shallow = createShallow({dive: true});

    const data: PointCoords[] = [
        createPointCoords(0, 0),
        createPointCoords(1, 1)
    ];
    const colormap = ['#fff', '#222'];

    const onChangeSpy = jest.fn();
    const component = shallow(
        <TransferFunction
            data={data}
            colormap={colormap}
            range={[]}
            onChange={onChangeSpy}
        />
    );

    const tf = component.dive();

    const instance: any = tf.instance();

    instance.componentWillUnmount();
    instance.emitChanges();

    await tick(250);

    expect(onChangeSpy).not.toHaveBeenCalled();

    component.unmount();
});

it('adds a new control point when mousedown occurs on the canvas', () => {
    const shallow = createShallow({dive: true});

    const data: PointCoords[] = [
        createPointCoords(0, 0),
        createPointCoords(1, 1)
    ];

    const colormap = ['#fff', '#222'];
    const scale = chroma.scale(colormap);

    const component = shallow(
        <TransferFunction
            data={data}
            colormap={colormap}
            range={[]}
        />
    );

    const tf = component.dive();

    Object.defineProperty(tf.instance(), 'canvas', {
        value: createRect(100, 100)
    });

    const initialPoints: ControlPoint[] = tf.state('points');
    expect(initialPoints).toHaveLength(data.length);

    const persistSpy = jest.fn();
    tf.find('svg').simulate('mousedown', {
        persist: persistSpy,
        preventDefault: noop,
        pageX: 50,
        pageY: 50
    });

    expect(persistSpy).toHaveBeenCalled();

    const points: ControlPoint[] = tf.state('points');
    expect(initialPoints).toHaveLength(data.length);
    expect(points).toHaveLength(3);

    const [a, b, c] = points;
    const [a0, b0] = initialPoints;

    expect(a).toEqual(a0);
    expect(c).toEqual(b0);

    expect(b.x).toBe(0.5);
    expect(b.y).toBe(0.5);
    expect(compareColor(b.color, scale(0.5).hex())).toBe(true);
    expect(b.id).toBeDefined();

    component.unmount();
});

it('does not add a new control point when mousedown occurs outside the canvas (on the axis or padding area)', () => {
    const shallow = createShallow({dive: true});

    const data: PointCoords[] = [
        createPointCoords(0, 0),
        createPointCoords(1, 1)
    ];
    const colormap = ['#fff', '#222'];

    const component = shallow(
        <TransferFunction
            data={data}
            colormap={colormap}
            range={[]}
        />
    );

    const tf = component.dive();

    Object.defineProperty(tf.instance(), 'canvas', {
        value: createRect(100, 100, 20, 10)
    });

    const initialPoints: ControlPoint[] = tf.state('points');
    expect(initialPoints).toHaveLength(data.length);

    const svg = tf.find('svg');
    svg.simulate('mousedown', createMouseDown(5, 50));
    svg.simulate('mousedown', createMouseDown(50, 10));
    svg.simulate('mousedown', createMouseDown(5, 10));
    svg.simulate('mousedown', createMouseDown(150, 50));
    svg.simulate('mousedown', createMouseDown(50, 150));
    svg.simulate('mousedown', createMouseDown(150, 150));

    const points: ControlPoint[] = tf.state('points');
    expect(initialPoints).toHaveLength(data.length);
    expect(points).toEqual(initialPoints);

    component.unmount();
});

it('does not add a new control point when mousedown occurs on the same axis of an existing point', () => {
    const shallow = createShallow({dive: true});

    const data: PointCoords[] = [
        createPointCoords(0, 0),
        createPointCoords(0.5, 0.5),
        createPointCoords(1, 1)
    ];
    const colormap = ['#fff', '#222'];

    const component = shallow(
        <TransferFunction
            data={data}
            colormap={colormap}
            range={[]}
        />
    );

    const tf = component.dive();

    Object.defineProperty(tf.instance(), 'canvas', {
        value: createRect(100, 100)
    });

    const initialPoints: ControlPoint[] = tf.state('points');
    expect(initialPoints).toHaveLength(data.length);

    // Account for point size and hit area epsilon
    for (let i = -5; i <= 5; i++) {
        tf.find('svg')
            .simulate('mousedown', createMouseDown(50 + i, 50));
    }

    const points: ControlPoint[] = tf.state('points');
    expect(initialPoints).toHaveLength(data.length);
    expect(points).toEqual(initialPoints);

    component.unmount();
});

it('emits the changes after a new point is added', async () => {
    const shallow = createShallow({dive: true});

    const data: PointCoords[] = [
        createPointCoords(0, 0),
        createPointCoords(1, 1)
    ];

    const colormap = ['#fff', '#222'];

    const onChangeSpy = jest.fn();
    const component = shallow(
        <TransferFunction
            data={data}
            colormap={colormap}
            range={[]}
            onChange={onChangeSpy}
        />
    );

    const tf = component.dive();

    Object.defineProperty(tf.instance(), 'canvas', {
        value: createRect(100, 100)
    });

    tf.find('svg')
        .simulate('mousedown', createMouseDown(50, 50));

    const points: ControlPoint[] = tf.state('points');

    await tick(250);
    expect(onChangeSpy).toHaveBeenCalled();

    const [changes] = onChangeSpy.mock.calls[0];
    expect(changes).toEqual(points.map(point => ({
        x: point.x,
        y: 1 - point.y
    })));

    component.unmount();
});

it('removes a control point when <Point> invokes {onRemove} cb', () => {
    const shallow = createShallow({dive: true});

    const data: PointCoords[] = [
        createPointCoords(0, 0),
        createPointCoords(0.5, 0.5),
        createPointCoords(1, 1)
    ];

    const colormap = ['#fff', '#222'];

    const component = shallow(
        <TransferFunction
            data={data}
            colormap={colormap}
            range={[]}
        />
    );

    const tf = component.dive();

    const initialPoints: ControlPoint[] = tf.state('points');
    expect(initialPoints).toHaveLength(data.length);

    const pointElements = tf.find(Point);
    pointElements.at(1)
        .props()
        .onRemove();

    const points: ControlPoint[] = tf.state('points');

    expect(initialPoints).toHaveLength(data.length);
    expect(points).toHaveLength(2);

    const [a, b] = points;
    const [a0, , c0] = initialPoints;

    expect(a).toEqual(a0);
    expect(b).toEqual(c0);

    component.unmount();
});

it('cannot remove the first/last control point', () => {
    const shallow = createShallow({dive: true});

    const data: PointCoords[] = [
        createPointCoords(0, 0),
        createPointCoords(0.5, 0.5),
        createPointCoords(1, 1)
    ];

    const colormap = ['#fff', '#222'];

    const component = shallow(
        <TransferFunction
            data={data}
            colormap={colormap}
            range={[]}
        />
    );

    const tf = component.dive();

    const initialPoints: ControlPoint[] = tf.state('points');
    expect(initialPoints).toHaveLength(data.length);

    const pointElements = tf.find(Point);

    pointElements.at(0)
        .props()
        .onRemove();

    pointElements.at(2)
        .props()
        .onRemove();

    const points: ControlPoint[] = tf.state('points');

    expect(initialPoints).toHaveLength(data.length);
    expect(points).toHaveLength(data.length);
    expect(points).toEqual(initialPoints);

    component.unmount();
});

it('emits the changes after a point is removed', async () => {
    const shallow = createShallow({dive: true});

    const data: PointCoords[] = [
        createPointCoords(0, 0),
        createPointCoords(0.5, 0.5),
        createPointCoords(1, 1)
    ];

    const colormap = ['#fff', '#222'];

    const onChangeSpy = jest.fn();
    const component = shallow(
        <TransferFunction
            data={data}
            colormap={colormap}
            range={[]}
            onChange={onChangeSpy}
        />
    );

    const tf = component.dive();

    const pointElements = tf.find(Point);
    pointElements.at(1)
        .props()
        .onRemove();

    const points: ControlPoint[] = tf.state('points');

    await tick(250);
    expect(onChangeSpy).toHaveBeenCalled();

    const [changes] = onChangeSpy.mock.calls[0];
    expect(changes).toEqual(points.map(point => ({
        x: point.x,
        y: 1 - point.y
    })));

    component.unmount();
});

it('updates control point when <Point> invokes {onChange} cb', () => {
    const shallow = createShallow({dive: true});

    const data: PointCoords[] = [
        createPointCoords(0, 0),
        createPointCoords(0.5, 0.5),
        createPointCoords(1, 1)
    ];

    const colormap = ['#fff', '#222'];
    const scale = chroma.scale(colormap);

    const component = shallow(
        <TransferFunction
            data={data}
            colormap={colormap}
            range={[]}
        />
    );

    const tf = component.dive();

    Object.defineProperty(tf.instance(), 'canvas', {
        value: createRect(100, 100)
    });

    const initialPoints: ControlPoint[] = tf.state('points');
    expect(initialPoints).toHaveLength(data.length);

    const pointElements = tf.find(Point);
    pointElements.at(1)
        .props()
        .onChange({
            pageX: 70,
            pageY: 65
        });

    const points: ControlPoint[] = tf.state('points');

    expect(initialPoints).toHaveLength(data.length);
    expect(points).toHaveLength(data.length);

    const [a, b, c] = points;
    const [a0, b0, c0] = initialPoints;

    expect(a).toEqual(a0);
    expect(b).not.toEqual(b0);
    expect(c).toEqual(c0);

    expect(b.id).toBe(b0.id);
    expect(b.x).toBe(0.7);
    expect(b.y).toBe(0.65);
    expect(compareColor(b.color, scale(0.7).hex())).toBe(true);

    component.unmount();
});

it('updates of <Point> are bounded to neighbouring right point', () => {
    const shallow = createShallow({dive: true});

    const data: PointCoords[] = [
        createPointCoords(0, 0),
        createPointCoords(0.25, 0.25),
        createPointCoords(0.5, 0.5),
        createPointCoords(0.75, 0.75),
        createPointCoords(1, 1)
    ];

    const colormap = ['#fff', '#222'];
    const scale = chroma.scale(colormap);

    const component = shallow(
        <TransferFunction
            data={data}
            colormap={colormap}
            range={[]}
        />
    );

    const tf = component.dive();

    Object.defineProperty(tf.instance(), 'canvas', {
        value: createRect(100, 100)
    });

    const initialPoints: ControlPoint[] = tf.state('points');
    expect(initialPoints).toHaveLength(data.length);

    const pointElements = tf.find(Point);
    pointElements.at(2)
        .props()
        .onChange({
            pageX: 85,
            pageY: 75
        });

    const points: ControlPoint[] = tf.state('points');

    expect(initialPoints).toHaveLength(data.length);
    expect(points).toHaveLength(data.length);

    const [a, b, c, d, e] = points;
    const [a0, b0, c0, d0, e0] = initialPoints;

    expect(a).toEqual(a0);
    expect(b).toEqual(b0);
    expect(c).not.toEqual(c0);
    expect(d).toEqual(d0);
    expect(e).toEqual(e0);

    // Point to right has 0.75
    const cx = 0.75 - POINT_SIZE / 100 * 2;
    const color = scale(cx)
        .hex();
    expect(c.x).toBe(cx);
    expect(c.y).toBe(0.75);
    expect(compareColor(c.color, color)).toBe(true);
    expect(c.id).toBe(c0.id);

    component.unmount();
});

it('updates of <Point> are bounded to neighbouring left point', () => {
    const shallow = createShallow({dive: true});

    const data: PointCoords[] = [
        createPointCoords(0, 0),
        createPointCoords(0.25, 0.25),
        createPointCoords(0.5, 0.5),
        createPointCoords(0.75, 0.75),
        createPointCoords(1, 1)
    ];

    const colormap = ['#fff', '#222'];
    const scale = chroma.scale(colormap);

    const component = shallow(
        <TransferFunction
            data={data}
            colormap={colormap}
            range={[]}
        />
    );

    const tf = component.dive();

    Object.defineProperty(tf.instance(), 'canvas', {
        value: createRect(100, 100)
    });

    const initialPoints: ControlPoint[] = tf.state('points');
    expect(initialPoints).toHaveLength(data.length);

    const pointElements = tf.find(Point);
    pointElements.at(2)
        .props()
        .onChange({
            pageX: 15,
            pageY: 75
        });

    const points: ControlPoint[] = tf.state('points');

    expect(initialPoints).toHaveLength(data.length);
    expect(points).toHaveLength(data.length);

    const [a, b, c, d, e] = points;
    const [a0, b0, c0, d0, e0] = initialPoints;

    expect(a).toEqual(a0);
    expect(b).toEqual(b0);
    expect(c).not.toEqual(c0);
    expect(d).toEqual(d0);
    expect(e).toEqual(e0);

    // Point to left has 0.25
    const cx = 0.25 + POINT_SIZE / 100 * 2;
    const color = scale(cx)
        .hex();
    expect(c.x).toBe(cx);
    expect(c.y).toBe(0.75);
    expect(compareColor(c.color, color)).toBe(true);
    expect(c.id).toBe(c0.id);

    component.unmount();
});

it('updates of the first/last <Point> are bounded to the Y axis', () => {
    const shallow = createShallow({dive: true});

    const data: PointCoords[] = [
        createPointCoords(0, 0),
        createPointCoords(0.5, 0.5),
        createPointCoords(1, 1)
    ];

    const colormap = ['#fff', '#222'];

    const component = shallow(
        <TransferFunction
            data={data}
            colormap={colormap}
            range={[]}
        />
    );

    const tf = component.dive();

    Object.defineProperty(tf.instance(), 'canvas', {
        value: createRect(100, 100)
    });

    const initialPoints: ControlPoint[] = tf.state('points');
    expect(initialPoints).toHaveLength(data.length);

    const pointElements = tf.find(Point);

    pointElements.at(0)
        .props()
        .onChange({
            pageX: 15,
            pageY: 75
        });

    pointElements.at(2)
        .props()
        .onChange({
            pageX: 75,
            pageY: 25
        });

    const points: ControlPoint[] = tf.state('points');

    expect(initialPoints).toHaveLength(data.length);
    expect(points).toHaveLength(data.length);

    const [a, b, c] = points;
    const [a0, b0, c0] = initialPoints;

    expect(a).not.toEqual(a0);
    expect(b).toEqual(b0);
    expect(c).not.toEqual(c0);

    expect(a.x).toBe(0);
    expect(a.y).toBe(0.75);
    expect(a.color).toBe(a0.color);
    expect(a.id).toBe(a0.id);

    expect(c.x).toBe(1);
    expect(c.y).toBe(0.25);
    expect(c.color).toBe(c0.color);
    expect(c.id).toBe(c0.id);

    component.unmount();
});

it('emits the changes after a point is updated', async () => {
    const shallow = createShallow({dive: true});

    const data: PointCoords[] = [
        createPointCoords(0, 0),
        createPointCoords(0.5, 0.5),
        createPointCoords(1, 1)
    ];

    const colormap = ['#fff', '#222'];

    const onChangeSpy = jest.fn();
    const component = shallow(
        <TransferFunction
            data={data}
            colormap={colormap}
            range={[]}
            onChange={onChangeSpy}
        />
    );

    const tf = component.dive();

    const pointElements = tf.find(Point);
    pointElements.at(1)
        .props()
        .onChange({
            pageX: 70,
            pageY: 50
        });

    const points: ControlPoint[] = tf.state('points');

    await tick(250);
    expect(onChangeSpy).toHaveBeenCalled();

    const [changes] = onChangeSpy.mock.calls[0];
    expect(changes).toEqual(points.map(point => ({
        x: point.x,
        y: 1 - point.y
    })));

    component.unmount();
});


function tick(timeout: number = 100) {
    return new Promise<void>(resolve => {
        setTimeout(() => {
            resolve();
        }, timeout);
    });
}

function mockSVGRect(subject: BehaviorSubject<CanvasRect>) {
    const spy = spyOn(HTMLDivElement.prototype, 'getBoundingClientRect').and.callFake(() => {
        return subject.value;
    });
    return spy;
}

function compareColor(a: string, b: string) {
    const a0 = chroma(a);
    const b0 = chroma(b);
    const d: number = distance(a0, b0) as any;
    return d === 0;
}

function changeTheme(theme: Theme, type: PaletteType) {
    return {
        theme: {
            ...theme,
            palette: {
                ...theme.palette,
                type
            }
        }
    };
}

function scaleX(x: number) {
    return x * canvas.width;
}

function scaleY(y: number) {
    return canvas.height - (y * canvas.height);
}

function createMouseDown(pageX: number, pageY: number) {
    return {
        persist: noop,
        preventDefault: noop,
        pageX,
        pageY
    };
}
