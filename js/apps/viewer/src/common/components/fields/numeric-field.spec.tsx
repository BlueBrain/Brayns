import React from 'react';

import {createMount, createShallow, getClasses} from '@material-ui/core/test-utils';

import TextField from '@material-ui/core/TextField';
import Typography from '@material-ui/core/Typography';
import Slider from '@material-ui/lab/Slider';

import NumericField from './numeric-field';


const classes = getClasses(<NumericField />);


it('renders without crashing', () => {
    const shallow = createShallow();
    const component = shallow(<NumericField />);
    component.unmount();
});

it('updates the state when the props update', () => {
    const shallow = createShallow({dive: true});
    const component = shallow(<NumericField />);

    component.setProps({
        value: 10
    });

    expect(component.state('value')).toBe(10);

    component.unmount();
});

it('sets {className} on root node if set by props', () => {
    const mount = createMount();

    const className = 'test';
    const component = mount(<NumericField className={className} />);

    expect(component.getDOMNode()
        .classList
        .contains(className)).toBe(true);

    component.unmount();
});

it('sets full width class when {fullWidth} prop is set', () => {
    const mount = createMount();

    const component = mount(<NumericField fullWidth />);

    expect(component.getDOMNode()
        .classList
        .contains(classes.fullWidth)).toBe(true);

    component.unmount();
});

it('sets normal margin class when {margin} prop is set to "normal"', () => {
    const mount = createMount();

    const component = mount(<NumericField margin="normal" />);

    expect(component.getDOMNode()
        .classList
        .contains(classes.marginNormal)).toBe(true);

    component.unmount();
});

it('sets dense margin class when {margin} prop is set to "dense"', () => {
    const mount = createMount();

    const component = mount(<NumericField margin="dense" />);

    expect(component.getDOMNode()
        .classList
        .contains(classes.marginDense)).toBe(true);

    component.unmount();
});

it('renders a <Typography> node if {label} prop is set', () => {
    const mount = createMount();

    const label = 'Test';

    const container = document.createElement('div');
    const component = mount(<NumericField label={label} />, {
        attachTo: container
    });

    const typography = component.find(Typography);

    expect(typography.length).toBe(1);
    expect(typography.text()).toBe(label);

    component.unmount();
    mount.cleanUp();
});

it('does not render a <Typography> node if {label} prop is not set', () => {
    const mount = createMount();

    const container = document.createElement('div');
    const component = mount(<NumericField />, {
        attachTo: container
    });

    const typography = component.find(Typography);

    expect(typography.length).toBe(0);

    component.unmount();
    mount.cleanUp();
});

it('does not render a <Slider> if {min, max} props are not set', () => {
    const container = document.createElement('div');
    const mount = createMount();
    const component = mount(<NumericField />, {
        attachTo: container
    });

    const slider = component.find(Slider);
    expect(slider.length).toBe(0);

    component.unmount();
    mount.cleanUp();
});

it('does not render a <Slider> if {noSlider} prop is set', () => {
    const mount = createMount();

    const component = mount(<NumericField min={0} max={1} noSlider />);

    const slider = component.find(Slider);
    expect(slider.length).toBe(0);

    component.unmount();
    mount.cleanUp();
});

it('renders a <Slider> if {min, max} props are set', () => {
    const mount = createMount();

    const min = 0;
    const max = 1;
    const step = 0.1;

    const container = document.createElement('div');
    const component = mount(<NumericField min={min} max={max} step={step} />, {
        attachTo: container
    });

    const slider = component.find(Slider);

    expect(slider.length).toBe(1);
    expect(slider.prop('min')).toBe(min);
    expect(slider.prop('max')).toBe(max);
    expect(slider.prop('step')).toBe(step);

    component.unmount();
    mount.cleanUp();
});

it('updates the <Slider> value when the value updates', () => {
    const mount = createMount();

    const component = mount(<NumericField min={0} max={1} />);
    component.setProps({
        value: 10
    });

    const slider = component.find(Slider);
    expect(slider.prop('value')).toBe(10);

    component.unmount();
    mount.cleanUp();
});

it('runs {onChange} if the <Slider> value changes', () => {
    const mount = createMount();

    const onChangeSpy = jest.fn();

    const component = mount((
        <NumericField
            onChange={onChangeSpy}
            min={0}
            max={100}
        />
    ));

    const slider = component.find(Slider);
    slider.props()
        .onChange({}, 10);
    expect(onChangeSpy).toHaveBeenCalledWith(10);

    component.unmount();
    mount.cleanUp();
});

it('renders a <TextField>', () => {
    const mount = createMount();

    const id = 'test';
    const disabled = true;
    const inputProps = {min: 0, max: 1, step: 0.1};

    const container = document.createElement('div');
    const component = mount((
        <NumericField
            id={id}
            disabled={disabled}
            {...inputProps}
        />
    ), {
        attachTo: container
    });

    const textField = component.find(TextField);

    expect(textField.length).toBe(1);
    expect(textField.prop('InputProps')).toEqual({inputProps});
    expect(textField.prop('fullWidth')).toBe(false);
    expect(textField.prop('id')).toBe(id);
    expect(textField.prop('type')).toBe('number');
    expect(textField.prop('disabled')).toBe(disabled);

    component.unmount();
    mount.cleanUp();
});

it('renders <TextField> full width if <Slider> is not rendererd', () => {
    const mount = createMount();

    const container = document.createElement('div');
    const component = mount(<NumericField />, {
        attachTo: container
    });

    const textField = component.find(TextField);
    expect(textField.prop('fullWidth')).toBe(true);

    component.unmount();
    mount.cleanUp();
});

it('updates the <TextField> value when the value updates', () => {
    const mount = createMount();

    const component = mount(<NumericField />);
    component.setProps({
        value: 10
    });

    const textField = component.find(TextField);
    expect(textField.prop('value')).toBe(10);

    component.unmount();
    mount.cleanUp();
});

it('runs {onChange} if the <TextField> value changes', () => {
    const onChangeSpy = jest.fn();

    const mount = createMount();
    const component = mount(<NumericField onChange={onChangeSpy} />);

    const textField = component.find(TextField);
    textField.props()
        .onChange({
            target: {value: '10'}
        });

    expect(onChangeSpy).toHaveBeenCalledWith(10);

    component.unmount();
    mount.cleanUp();
});

it('runs {onChange} with {min} if <Slider> sets a value less than {min}', () => {
    const onChangeSpy = jest.fn();

    const min = 0.0001;

    const mount = createMount();
    const component = mount(<NumericField onChange={onChangeSpy} min={min} max={1}/>);

    const slider = component.find(Slider);
    slider.props()
        .onChange({}, 0);

    expect(onChangeSpy).toHaveBeenCalledWith(min);

    component.unmount();
    mount.cleanUp();
});

it('runs {onChange} with {min} if <TextField> sets a value less than {min}', () => {
    const mount = createMount();

    const onChangeSpy = jest.fn();

    const min = 1;
    const component = mount(<NumericField onChange={onChangeSpy} min={min} max={100}/>);

    const textField = component.find(TextField);
    textField.props()
        .onChange({
            target: {value: '-10'}
        });

    expect(onChangeSpy).toHaveBeenCalledWith(min);

    component.unmount();
    mount.cleanUp();
});

it('runs {onChange} with {max} if <TextField> sets a value more than {max}', () => {
    const mount = createMount();

    const onChangeSpy = jest.fn();

    const max = 1;
    const component = mount(<NumericField onChange={onChangeSpy} min={0} max={max} />);

    const textField = component.find(TextField);
    textField.props()
        .onChange({
            target: {value: '10'}
        });

    expect(onChangeSpy).toHaveBeenCalledWith(max);

    component.unmount();
    mount.cleanUp();
});

it('does not run {onChange} if <TextField> sets a non numeric value', () => {
    const shallow = createShallow({dive: true});

    const onChangeSpy = jest.fn();
    const component = shallow(<NumericField onChange={onChangeSpy} />);

    component.find(TextField)
        .props()
        .onChange({
            target: {value: '-'}
        });

    expect(onChangeSpy).not.toHaveBeenCalled();
    expect(component.state('value')).toBe('-');

    component.unmount();
});

it('runs {onChange} with int values if <TextField> sets a float and {type} is set to \'integer\'', () => {
    const mount = createMount();

    const onChangeSpy = jest.fn();
    const component = mount(<NumericField onChange={onChangeSpy} type="integer" />);

    const textField = component.find(TextField);
    textField.props()
        .onChange({
            target: {value: '1.2'}
        });

    expect(onChangeSpy).toHaveBeenCalledWith(1);

    component.unmount();
    mount.cleanUp();
});
