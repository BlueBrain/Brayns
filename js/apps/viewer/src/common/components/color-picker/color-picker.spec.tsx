import React from 'react';
import ReactDOM from 'react-dom';

import chroma from 'chroma-js';

import {createMount, createShallow} from '@material-ui/core/test-utils';

import Dialog from '@material-ui/core/Dialog';
import {TransitionProps} from '@material-ui/core/transitions/transition';

import Vector3Field from '../fields/vector3-field';
import ColorPicker from './color-picker';
import {MATERIAL_PALETTES} from './constants';
import Saturation from './saturation';
import Swatches from './swatches';
import {toHsv} from './utils';


it('renders without crashing', () => {
    const div = document.createElement('div');
    const open = false;
    const rgb = [0, 0, 0];
    ReactDOM.render((
        <ColorPicker
            open={open}
            value={rgb}
        />
    ), div);
});

it('renders <Saturation> if {open} is true', () => {
    const mount = createMount();

    const rgb = [0, 0, 0];
    const colorPicker = mount(
        <ColorPicker
            open
            value={rgb}
        />
    );
    const saturation = colorPicker.find(Saturation);

    expect(saturation).toHaveLength(1);

    colorPicker.unmount();
    mount.cleanUp();
});

it('does not render <Saturation> if {open} is false', () => {
    const mount = createMount();

    const rgb = [0, 0, 0];
    const colorPicker = mount(
        <ColorPicker
            open={false}
            value={rgb}
        />
    );
    const saturation = colorPicker.find(Saturation);

    expect(saturation).toHaveLength(0);

    colorPicker.unmount();
    mount.cleanUp();
});

it('passes on {TransitionProps} to the dialog', () => {
    const mount = createMount();

    const rgb = [0, 0, 0];
    const onExitedSpy = jest.fn();
    const transitionProps: TransitionProps = {onExited: onExitedSpy};
    const colorPicker = mount(
        <ColorPicker
            value={rgb}
            TransitionProps={transitionProps}
            open
        />
    );

    colorPicker.find(Dialog)
        .prop('TransitionProps')
        .onExited();

    expect(onExitedSpy).toHaveBeenCalled();

    colorPicker.unmount();
    mount.cleanUp();
});

it('renders <Swatches> if {open} is true', () => {
    const mount = createMount();

    const rgb = [0, 0, 0];
    const colorPicker = mount(
        <ColorPicker
            value={rgb}
            open
        />
    );
    const swatches = colorPicker.find(Swatches);

    expect(swatches).toHaveLength(1);

    colorPicker.unmount();
    mount.cleanUp();
});

it('does not render <Swatches> if {open} is false', () => {
    const mount = createMount();

    const rgb = [0, 0, 0];
    const colorPicker = mount(
        <ColorPicker
            value={rgb}
            open={false}
        />
    );
    const swatches = colorPicker.find(Swatches);

    expect(swatches).toHaveLength(0);

    colorPicker.unmount();
    mount.cleanUp();
});

it('renders <Vector3Field> if {open} is true', () => {
    const mount = createMount();

    const color = [0, 0, 0];
    const colorPicker = mount(
        <ColorPicker
            value={color}
            open
        />
    );
    const rgb = colorPicker.find(Vector3Field);

    expect(rgb).toHaveLength(1);

    colorPicker.unmount();
    mount.cleanUp();
});

it('does not render <Vector3Field> if {open} is false', () => {
    const mount = createMount();

    const color = [0, 0, 0];
    const colorPicker = mount(
        <ColorPicker
            value={color}
            open={false}
        />
    );
    const rgb = colorPicker.find(Vector3Field);

    expect(rgb).toHaveLength(0);

    colorPicker.unmount();
    mount.cleanUp();
});

it('sets <Vector3Field> value to the value of {value} prop', () => {
    const mount = createMount();

    const color = [254, 254, 254];
    const colorPicker = mount(
        <ColorPicker
            value={color}
            open
        />
    );

    const rgb = colorPicker.find(Vector3Field);

    expect(rgb.prop('value')).toEqual(color);

    colorPicker.unmount();
    mount.cleanUp();
});

it('runs {onChange} if <Vector3Field> value changes', () => {
    const mount = createMount();
    jest.useFakeTimers();

    const onChangeSpy = jest.fn();
    const colorPicker = mount(
        <ColorPicker
            value={[0, 0, 0]}
            onChange={onChangeSpy}
            open
        />
    );

    const rgb = colorPicker.find(Vector3Field);

    const color = [254, 254, 254];
    rgb.props()
        .onChange(color);

    jest.runOnlyPendingTimers();

    expect(onChangeSpy).toHaveBeenCalledWith(color);

    jest.useRealTimers();
    colorPicker.unmount();
    mount.cleanUp();
});

it('sets <Saturation> value to the HSV value of {value} prop', () => {
    const mount = createMount();

    const color = [254, 254, 254];
    const colorPicker = mount(
        <ColorPicker
            value={color}
            open
        />
    );

    const saturation = colorPicker.find(Saturation);

    const hsv = rgbAsHsv(color);
    expect(saturation.prop('value')).toEqual(hsv);

    colorPicker.unmount();
    mount.cleanUp();
});

it('runs {onChange} if <Saturation> value changes', () => {
    const mount = createMount();
    jest.useFakeTimers();

    const onChangeSpy = jest.fn();
    const colorPicker = mount(
        <ColorPicker
            value={[0, 0, 0]}
            onChange={onChangeSpy}
            open
        />
    );

    const saturation = colorPicker.find(Saturation);

    const color = [254, 254, 254];
    const hsv = rgbAsHsv(color);

    saturation.props()
        .onChange(hsv);

    jest.runOnlyPendingTimers();

    expect(onChangeSpy).toHaveBeenCalledWith(color);

    jest.useRealTimers();
    colorPicker.unmount();
    mount.cleanUp();
});

it('sets <Swatches> value to the HEX value of {value} prop', () => {
    const mount = createMount();

    const hex = MATERIAL_PALETTES[0][500];
    const color = hexAsRgb(hex);

    const colorPicker = mount(
        <ColorPicker
            value={color}
            open
        />
    );

    const swatches = colorPicker.find(Swatches);

    expect(swatches.prop('value')).toEqual(hex);

    colorPicker.unmount();
    mount.cleanUp();
});

it('runs {onChange} if <Swatches> value changes', () => {
    const mount = createMount();
    jest.useFakeTimers();

    const onChangeSpy = jest.fn();
    const colorPicker = mount(
        <ColorPicker
            value={[0, 0, 0]}
            onChange={onChangeSpy}
            open
        />
    );

    const swatches = colorPicker.find(Swatches);

    const hex = MATERIAL_PALETTES[0][500];
    const color = hexAsRgb(hex);

    swatches.props()
        .onChange(hex);

    jest.runOnlyPendingTimers();

    expect(onChangeSpy).toHaveBeenCalledWith(color);

    jest.useRealTimers();
    colorPicker.unmount();
    mount.cleanUp();
});

it('uses memoization to compute the HSV color for <Saturation>', () => {
    const mount = createMount();

    const colorPicker = mount(
        <ColorPicker
            value={[254, 254, 254]}
            open
        />
    );

    const saturation = colorPicker.find(Saturation);

    const instance = saturation.instance();
    const renderSpy = spyOn(instance, 'render').and.callThrough();
    const hsv = saturation.prop('value');

    colorPicker.setProps({
        value: [254, 254, 254]
    });
    colorPicker.update();

    expect(renderSpy).toHaveBeenCalledTimes(1);

    expect(colorPicker.find(Saturation)
        .prop('value') === hsv).toBe(true);

    colorPicker.unmount();
    mount.cleanUp();
});

it('updates the state from the value of {value} prop', () => {
    const shallow = createShallow({dive: true});

    const colorPicker = shallow(
        <ColorPicker
            value={[255, 255, 255]}
            open
        />
    );

    const rgb = [254, 254, 254];
    const hex = rgbAsHex(rgb);
    const hsv = rgbAsHsv(rgb);
    colorPicker.setProps({
        value: rgb
    });

    expect(colorPicker.state('rgb')).toEqual(rgb);
    expect(colorPicker.state('hex')).toEqual(hex);
    expect(colorPicker.state('hsv')).toEqual(hsv);

    colorPicker.unmount();
});

it('skips state updates when {value} changes if the current state has the same value', () => {
    const shallow = createShallow({dive: true});

    const colorPicker = shallow(
        <ColorPicker
            value={[255, 255, 255]}
            open
        />
    );

    const instance = colorPicker.instance();
    const setStateSpy = spyOn(instance, 'setState').and.callThrough();

    colorPicker.setProps({
        value: [255, 255, 255]
    });

    expect(setStateSpy).toHaveBeenCalledTimes(0);

    colorPicker.unmount();
});


function rgbAsHsv(rgb: number[]) {
    const color = chroma(rgb);
    return toHsv(color);
}

function hexAsRgb(hex: string) {
    const color = chroma(hex);
    return color.rgb();
}

function rgbAsHex(rgb: number[]) {
    const color = chroma(rgb);
    return color.hex();
}
