import {ReactWrapper} from 'enzyme';
import React from 'react';
import ReactDOM from 'react-dom';

import {createMount, getClasses} from '@material-ui/core/test-utils';

import Saturation, {calcSaturationAndBrightness} from './saturation';


it('renders without crashing', () => {
    const div = document.createElement('div');
    ReactDOM.render((
        <Saturation value={[0, 0, 0.26]} />
    ), div);
});

it('sets a HSL {backgroundColor} with the hue from the {value} prop', () => {
    const mount = createMount();

    const hue = 60;
    const hsv = [hue, 1, 1]; // #ffff00
    const saturation = mount(<Saturation value={hsv} />);

    const style = saturation.find('div')
        .at(0)
        .prop('style');

    expect(style).toEqual({
        backgroundColor: `hsl(${hue}, 100%, 50%)`
    });

    saturation.unmount();
    mount.cleanUp();
});

it('renders a point with its position calculated from the {value} prop', () => {
    const mount = createMount();

    const hue = 340;
    const s = 1;
    const v = 0.96;
    const hsv = [hue, s, v];

    const saturation = (<Saturation value={hsv} />);
    const classes = getClasses(saturation);

    const component = mount(saturation);

    const pointer = component.find(`.${classes.pointer}`);

    const style = pointer.prop('style');

    // Positions are scaled from [0, 1] to [0, 100]
    // Brightness is inverted,
    // hence the +100
    expect(style).toEqual({
        top: '4%', // -v * 100 + 100
        left: '100%' // s * 100
    });

    component.unmount();
    mount.cleanUp();
});

it('should run {onChange} with a new HSV value computed from current HSV and mouse position when clicked', () => {
    const mount = createMount();

    const hue = 340;
    const s = 1;
    const v = 0.96;

    const onChangeSpy = jest.fn();

    const saturation = mount(
        <Saturation
            value={[hue, s, v]}
            onChange={onChangeSpy}
        />
    );

    mockRect(saturation, {
        width: 100,
        height: 100,
        top: 0,
        left: 0
    });

    saturation.simulate('mousedown', {
        pageX: 50,
        pageY: 50
    });

    expect(onChangeSpy).toHaveBeenCalledTimes(1);

    const [args] = onChangeSpy.mock.calls;
    const [hsv] = args;

    expect(hsv[0]).toBe(hue);
    expect(hsv[1]).toBeCloseTo(0.5);
    expect(hsv[2]).toBeCloseTo(0.5);

    saturation.unmount();
    mount.cleanUp();
});

it('should run {onChange} with a new HSV value computed from current HSV and mouse position when the cursor is dragged', () => {
    const mount = createMount();

    const hue = 340;
    const s = 1;
    const v = 0.96;

    const onChangeSpy = jest.fn();

    const saturation = mount(
        <Saturation
            value={[hue, s, v]}
            onChange={onChangeSpy}
        />
    );

    mockRect(saturation, {
        width: 100,
        height: 100,
        top: 0,
        left: 0
    });

    saturation.simulate('mousedown', {
        pageX: 50,
        pageY: 50
    });

    simulateMousemove({
        pageX: 60,
        pageY: 40
    });

    expect(onChangeSpy).toHaveBeenCalledTimes(2);

    const [change1Args, change2Args] = onChangeSpy.mock.calls;

    const [hsv1] = change1Args;
    expect(hsv1[0]).toBe(hue);
    expect(hsv1[1]).toBeCloseTo(0.5);
    expect(hsv1[2]).toBeCloseTo(0.5);

    const [hsv2] = change2Args;
    expect(hsv2[0]).toBe(hue);
    expect(hsv2[1]).toBeCloseTo(0.6);
    expect(hsv2[2]).toBeCloseTo(0.6);

    saturation.unmount();
    mount.cleanUp();
});

it('should unbind cursor drag events if mouse is released', () => {
    const mount = createMount();

    const hue = 340;
    const s = 1;
    const v = 0.96;

    const onChangeSpy = jest.fn();

    const saturation = mount(
        <Saturation
            value={[hue, s, v]}
            onChange={onChangeSpy}
        />
    );

    mockRect(saturation, {
        width: 100,
        height: 100,
        top: 0,
        left: 0
    });

    saturation.simulate('mousedown', {
        pageX: 50,
        pageY: 50
    });

    simulateMousemove({
        pageX: 60,
        pageY: 40
    });

    simulateMouseup();

    simulateMousemove({
        pageX: 60,
        pageY: 40
    });

    expect(onChangeSpy).toHaveBeenCalledTimes(2);

    saturation.unmount();
    mount.cleanUp();
});

it('should run {onChange} with a new HSV value computed from current HSV and touch position when touched', () => {
    const mount = createMount();

    const hue = 340;
    const s = 1;
    const v = 0.96;

    const onChangeSpy = jest.fn();

    const saturation = mount(
        <Saturation
            value={[hue, s, v]}
            onChange={onChangeSpy}
        />
    );

    mockRect(saturation, {
        width: 100,
        height: 100,
        top: 0,
        left: 0
    });

    saturation.simulate('touchstart', {
        pageX: 50,
        pageY: 50
    });

    expect(onChangeSpy).toHaveBeenCalledTimes(1);

    const [args] = onChangeSpy.mock.calls;
    const [hsv] = args;

    expect(hsv[0]).toBe(hue);
    expect(hsv[1]).toBeCloseTo(0.5);
    expect(hsv[2]).toBeCloseTo(0.5);

    saturation.unmount();
    mount.cleanUp();
});

it('should run {onChange} with a new HSV value computed from current HSV and touch position when on touch move', () => {
    const mount = createMount();

    const hue = 340;
    const s = 1;
    const v = 0.96;

    const onChangeSpy = jest.fn();

    const saturation = mount(
        <Saturation
            value={[hue, s, v]}
            onChange={onChangeSpy}
        />
    );

    mockRect(saturation, {
        width: 100,
        height: 100,
        top: 0,
        left: 0
    });

    saturation.simulate('touchmove', {
        pageX: 60,
        pageY: 40
    });

    expect(onChangeSpy).toHaveBeenCalledTimes(1);

    const [args] = onChangeSpy.mock.calls;
    const [hsv] = args;
    expect(hsv[0]).toBe(hue);
    expect(hsv[1]).toBeCloseTo(0.6);
    expect(hsv[2]).toBeCloseTo(0.6);

    saturation.unmount();
    mount.cleanUp();
});

describe('calcSaturationAndBrightness()', () => {
    it('should return computed values from saturation and brightness based on rect size and mouse evt position', () => {
        const node = document.createElement('div');

        spyOn(node, 'getBoundingClientRect').and.returnValue({
            width: 100,
            height: 100,
            top: 0,
            left: 0
        });

        const evt = createMouseEvt('mousedown', {
            pageX: 50,
            pageY: 50
        });

        const {s, v} = calcSaturationAndBrightness(node, evt as any);

        expect(s).toBe(0.5);
        expect(v).toBe(0.5);
    });

    it('should work with touch events', () => {
        const node = document.createElement('div');

        spyOn(node, 'getBoundingClientRect').and.returnValue({
            width: 100,
            height: 100,
            top: 0,
            left: 0
        });

        const touch = createTouch({
            pageX: 50,
            pageY: 50
        });
        const evt = createTouchEvt('touchstart', [touch]);

        const {s, v} = calcSaturationAndBrightness(node, evt as any);

        expect(s).toBe(0.5);
        expect(v).toBe(0.5);
    });

    it('should account for rect position', () => {
        const node = document.createElement('div');

        spyOn(node, 'getBoundingClientRect').and.returnValue({
            width: 100,
            height: 100,
            top: 20,
            left: 20
        });

        const evt = createMouseEvt('mousedown', {
            pageX: 50,
            pageY: 50
        });

        const {s, v} = calcSaturationAndBrightness(node, evt as any);

        expect(s).toBe(0.3);
        expect(v).toBe(0.7);
    });

    it('should account for window scroll offset', () => {
        const restoreScroll = mockScrollOffset(20, 20);

        const node = document.createElement('div');

        spyOn(node, 'getBoundingClientRect').and.returnValue({
            width: 100,
            height: 100,
            top: 0,
            left: 0
        });

        const evt = createMouseEvt('mousedown', {
            pageX: 50,
            pageY: 50
        });

        const {s, v} = calcSaturationAndBrightness(node, evt as any);

        expect(s).toBe(0.3);
        expect(v).toBe(0.7);

        restoreScroll();
    });

    it('should be bounded on min {top, left}', () => {
        const node = document.createElement('div');

        spyOn(node, 'getBoundingClientRect').and.returnValue({
            width: 100,
            height: 100,
            top: 20,
            left: 20
        });

        const evt = createMouseEvt('mousemove', {
            pageX: 10,
            pageY: 10
        });

        const {s, v} = calcSaturationAndBrightness(node, evt as any);

        expect(s).toBe(0);
        expect(v).toBe(1);
    });

    it('should be bounded on max {top, left}', () => {
        const node = document.createElement('div');

        spyOn(node, 'getBoundingClientRect').and.returnValue({
            width: 100,
            height: 100,
            top: 0,
            left: 0
        });

        const evt = createMouseEvt('mousemove', {
            pageX: 150,
            pageY: 150
        });

        const {s, v} = calcSaturationAndBrightness(node, evt as any);

        expect(s).toBe(1);
        expect(v).toBe(0);
    });
});


function mockRect(component: ReactWrapper, rect: Partial<DOMRect>) {
    const node = component.find('div')
        .at(0)
        .getDOMNode();
    spyOn(node, 'getBoundingClientRect').and.returnValue(rect);
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

function simulateMousemove(props: Pick<MouseEvent, 'pageX' | 'pageY'>) {
    const evt = createMouseEvt('mousemove', props);
    document.dispatchEvent(evt);
}

function simulateMouseup() {
    const evt = createMouseEvt('mouseup');
    document.dispatchEvent(evt);
}

function createMouseEvt(name: string, props?: Pick<MouseEvent, 'pageX' | 'pageY'>) {
    const evt = new MouseEvent(name, {});
    if (props) {
        Object.assign(evt, props);
    }
    return evt;
}

function createTouchEvt(name: string, touches: Touch[]) {
    const evt = new TouchEvent(name, {touches});
    return evt;
}

function createTouch(props: Pick<Touch, 'pageX' | 'pageY'>) {
    // Touch does not exist in jsdom
    const touch = {
        ...props
    };
    return touch as Touch;
}
