import React from 'react';

import chroma from 'chroma-js';

import {createMount, createShallow, getClasses} from '@material-ui/core/test-utils';

import FormControl, {FormControlProps} from '@material-ui/core/FormControl';
import IconButton from '@material-ui/core/IconButton';
import Input from '@material-ui/core/Input';
import InputAdornment from '@material-ui/core/InputAdornment';
import InputLabel from '@material-ui/core/InputLabel';

import ColorPicker from '../color-picker';
import ColorField from './color-field';


const classes = getClasses(<ColorField />);


const mockDispatchKeyboardLock = jest.fn();
jest.mock('../../events', () => ({
    dispatchKeyboardLock(lock: boolean) {
        return mockDispatchKeyboardLock(lock);
    }
}));

afterEach(() => {
    mockDispatchKeyboardLock.mockClear();
});


it('renders without crashing', () => {
    const shallow = createShallow();
    const component = shallow(<ColorField />);
    component.unmount();
});

it('renders a <FormControl> component', () => {
    const mount = createMount();

    const props: FormControlProps = {
        margin: 'normal',
        fullWidth: true,
        required: true,
        disabled: false,
        error: false
    };
    const component = mount(<ColorField {...props as any} />);

    const control = component.find(FormControl);

    expect(control).toHaveLength(1);
    expect(control.prop('margin')).toBe(props.margin);
    expect(control.prop('fullWidth')).toBe(props.fullWidth);
    expect(control.prop('required')).toBe(props.required);
    expect(control.prop('disabled')).toBe(props.disabled);
    expect(control.prop('error')).toBe(props.error);

    component.unmount();
    mount.cleanUp();
});

it('renders a <InputLabel> component', () => {
    const mount = createMount();

    const id = 'test';
    const label = 'Test';
    const component = mount(
        <ColorField
            id={id}
            label={label}
        />
    );

    const inputLabel = component.find(InputLabel);

    expect(inputLabel).toHaveLength(1);
    expect(inputLabel.prop('htmlFor')).toBe(id);
    expect(inputLabel.text()).toBe(label);

    component.unmount();
    mount.cleanUp();
});

it('renders a <Input> component', () => {
    const mount = createMount();

    const id = 'test';
    const label = 'Test';
    const component = mount(
        <ColorField
            id={id}
            label={label}
        />
    );

    const input = component.find(Input);

    expect(input).toHaveLength(1);
    expect(input.prop('id')).toBe(id);
    expect(input.prop('type')).toBe('text');

    component.unmount();
    mount.cleanUp();
});

it('renders a <InputAdornment> component', () => {
    const mount = createMount();

    const id = 'test';
    const label = 'Test';
    const component = mount(
        <ColorField
            id={id}
            label={label}
        />
    );

    const adornment = component.find(InputAdornment);

    expect(adornment).toHaveLength(1);
    expect(adornment.prop('position')).toBe('end');

    component.unmount();
    mount.cleanUp();
});

it('renders a <IconButton> component', () => {
    const mount = createMount();

    const id = 'test';
    const ariaLabel = 'Test';
    const component = mount(
        <ColorField
            id={id}
            ariaLabel={ariaLabel}
            disabled
        />
    );

    const button = component.find(IconButton);

    expect(button).toHaveLength(1);
    expect(button.prop('aria-label')).toBe(ariaLabel);
    expect(button.prop('disabled')).toBe(true);

    component.unmount();
    mount.cleanUp();
});

it('renders a <ColorPicker> component', () => {
    const mount = createMount();
    const component = mount(<ColorField />);

    const colorPicker = component.find(ColorPicker);
    expect(colorPicker).toHaveLength(1);

    component.unmount();
    mount.cleanUp();
});

it('should open the <ColorPicker> when clicking on <IconButton>', () => {
    const mount = createMount();

    const component = mount(
        <ColorField />
    );

    expect(component.find(ColorPicker)
        .prop('open')).toBe(false);

    const button = component.find(IconButton);
    button.simulate('click');

    expect(component.find(ColorPicker)
        .prop('open')).toBe(true);

    component.unmount();
    mount.cleanUp();
});

it('should close the <ColorPicker> when {onClose} is triggered', () => {
    const mount = createMount();

    const component = mount(
        <ColorField />
    );

    const button = component.find(IconButton);
    button.simulate('click');


    expect(component.find(ColorPicker)
        .prop('open')).toBe(true);

    const picker = component.find(ColorPicker);
    picker.props()
        .onClose();
    component.update();

    expect(component.find(ColorPicker)
        .prop('open')).toBe(false);

    component.unmount();
    mount.cleanUp();
});

it('locks keyboard interaction when the <ColorPicker> is opened', () => {
    const mount = createMount();

    const component = mount(<ColorField />);

    component.find(ColorPicker)
        .prop('TransitionProps')
        .onEnter();

    expect(mockDispatchKeyboardLock).toHaveBeenCalledWith(true);

    component.unmount();
    mount.cleanUp();
});

it('unlocks keyboard interaction after the <ColorPicker> is closed', () => {
    const mount = createMount();

    const component = mount(<ColorField />);

    component.find(ColorPicker)
        .prop('TransitionProps')
        .onExited();

    expect(mockDispatchKeyboardLock).toHaveBeenCalledWith(false);

    component.unmount();
    mount.cleanUp();
});

it('renders a color bubble with current color', () => {
    const mount = createMount();

    const hex = '#fefefe';
    const rgb = hexAsRgb(hex);
    const component = mount(
        <ColorField
            value={rgb}
        />
    );

    const bubble = component.find(`.${classes.colorBubble}`);
    expect(bubble).toHaveLength(1);

    expect(bubble.prop('style')).toEqual({
        backgroundColor: hex
    });

    component.unmount();
    mount.cleanUp();
});

it('updates the state with new color from {value} if it changes', () => {
    const shallow = createShallow({dive: true});

    const hex = '#fefefe';
    const rgb = hexAsRgb(hex);
    const component = shallow(
        <ColorField
            value={rgb}
        />
    );

    expect(component.state('rgb')).toEqual(rgb);
    expect(component.state('hex')).toEqual(hex);

    const hex2 = '#efefef';
    const rgb2 = hexAsRgb(hex2);
    component.setProps({
        value: rgb2
    });

    expect(component.state('rgb')).toEqual(rgb2);
    expect(component.state('hex')).toEqual(hex2);

    component.unmount();
});

it('updates the color bubble when the {value} changes', () => {
    const mount = createMount();

    const component = mount(<ColorField />);

    const hex = '#fefefe';
    const rgb = hexAsRgb(hex);
    component.setProps({
        value: rgb
    });

    const bubble = component.find(`.${classes.colorBubble}`);
    expect(bubble).toHaveLength(1);

    expect(bubble.prop('style')).toEqual({
        backgroundColor: hex
    });

    component.unmount();
    mount.cleanUp();
});

it('runs {onChange} when the <ColorPicker> updates the color', () => {
    const mount = createMount();

    const onChangeSpy = jest.fn();
    const component = mount(
        <ColorField onChange={onChangeSpy} />
    );

    const colorPicker = component.find(ColorPicker);

    const hex = '#fefefe';
    const rgb = hexAsRgb(hex);
    colorPicker.props()
        .onChange(rgb);

    expect(onChangeSpy).toHaveBeenCalledWith(rgb);

    component.unmount();
    mount.cleanUp();
});

it('updates the state with new a color when the <Input> value changes', () => {
    const mount = createMount();

    const component = mount(<ColorField />);
    const input = component.find(Input);

    const hex = '#fefefe';
    const rgb = hexAsRgb(hex);
    input.props()
        .onChange({
            target: {value: hex}
        });

    // Because the root is actually the MUI styled component
    const instance = component.childAt(0)
        .instance();
    const state = instance.state as any;

    expect(state.rgb).toEqual(rgb);
    expect(state.hex).toEqual(hex);

    component.unmount();
    mount.cleanUp();
});

it('always prepends "#" to the <Input> value when it changes', () => {
    const mount = createMount();

    const component = mount(<ColorField />);
    const input = component.find(Input);

    const value = 'fff';
    input.props()
        .onChange({
            target: {value}
        });

    const instance = component.childAt(0)
        .instance();
    const state = instance.state as any;

    expect(state.hex).toEqual(`#${value}`);

    component.unmount();
    mount.cleanUp();
});

it('updates the state with a rgb color only if the <Input> value is a valid hex color', () => {
    const mount = createMount();

    const component = mount(<ColorField />);
    const input = component.find(Input);

    const hex = 'wpq';
    input.props()
        .onChange({
            target: {value: hex}
        });

    // Because the root is actually the MUI styled component
    const instance = component.childAt(0)
        .instance();
    const state = instance.state as any;

    expect(state.hex).toEqual(`#${hex}`);
    expect(state.rgb).toEqual([0, 0, 0]); // Initial state

    component.unmount();
    mount.cleanUp();
});

it('runs {onChange} when the <Input> updates the color', () => {
    const mount = createMount();

    const onChangeSpy = jest.fn();
    const component = mount(
        <ColorField onChange={onChangeSpy} />
    );

    const input = component.find(Input);
    const props = input.props();

    const hex = '#fefefe';
    const rgb = hexAsRgb(hex);
    const change = {
        target: {value: hex}
    };
    props.onChange(change);
    props.onBlur(change);

    expect(onChangeSpy).toHaveBeenCalledWith(rgb);

    component.unmount();
    mount.cleanUp();
});

it('does not run {onChange} if the <Input> color is not a valid hex color', () => {
    const mount = createMount();

    const onChangeSpy = jest.fn();
    const component = mount(
        <ColorField onChange={onChangeSpy} />
    );

    const input = component.find(Input);
    const props = input.props();

    const hex = '#wpq';
    const change = {
        target: {value: hex}
    };
    props.onChange(change);
    props.onBlur(change);

    expect(onChangeSpy).not.toHaveBeenCalled();

    component.unmount();
    mount.cleanUp();
});

it('does not run {onChange} if the <Input> color is the same as the current rgb color', () => {
    const mount = createMount();

    const onChangeSpy = jest.fn();
    const component = mount(
        <ColorField onChange={onChangeSpy} value={[0, 0, 0]} />
    );

    const input = component.find(Input);
    const props = input.props();

    const hex = '#000';
    const change = {
        target: {value: hex}
    };
    props.onChange(change);
    props.onBlur(change);

    expect(onChangeSpy).not.toHaveBeenCalled();

    component.unmount();
    mount.cleanUp();
});

it('rerenders if {value} prop changed', () => {
    const shallow = createShallow({dive: true});

    const component = shallow(<ColorField value={[0, 0, 0]} />);

    const instance = component.instance();
    const renderSpy = spyOn(instance, 'render');

    component.setProps({
        value: [1, 0, 0]
    });

    expect(renderSpy).toHaveBeenCalledTimes(1);

    component.unmount();
});

it('does not rerender if {value} prop is updated with the same value instance', () => {
    const shallow = createShallow({dive: true});

    const value = [0, 0, 0];
    const props = {value};
    const component = shallow(<ColorField {...props} />);

    const instance = component.instance();
    const renderSpy = spyOn(instance, 'render');

    component.setProps({value});

    expect(renderSpy).toHaveBeenCalledTimes(0);

    component.unmount();
});


function hexAsRgb(hex: string) {
    const color = chroma(hex);
    return color.rgb();
}
