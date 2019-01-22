import React from 'react';

import {noop} from 'lodash';

import {createMount, createShallow, getClasses} from '@material-ui/core/test-utils';
import Typography from '@material-ui/core/Typography';

import NumericField from './numeric-field';
import VectorField from './vector-field';


it('renders without crashing', () => {
    const shallow = createShallow();
    const component = shallow(<VectorField items={2} value={[0, 1]} />);
    component.unmount();
});

it('has {className} property', () => {
    const shallow = createShallow({dive: true});

    const className = 'test-cls';
    const component = shallow(
        <VectorField
            className={className}
            items={2}
            value={[0, 1]}
        />
    );

    const item = component.find(`.${className}`);
    expect(item.length).toBe(1);

    component.unmount();
});

it('sets full width class when {fullWidth} prop is set', () => {
    const mount = createMount();

    const node = (
        <VectorField
            items={2}
            value={[0, 1]}
            fullWidth
        />
    );
    const classes = getClasses(node);
    const component = mount(node);

    expect(component.getDOMNode()
        .classList
        .contains(classes.fullWidth)).toBe(true);

    component.unmount();
});

it('sets normal margin class when {margin} prop is set to "normal"', () => {
    const mount = createMount();

    const node = (
        <VectorField
            items={2}
            value={[0, 1]}
            margin="normal"
        />
    );
    const classes = getClasses(node);
    const component = mount(node);

    expect(component.getDOMNode()
        .classList
        .contains(classes.marginNormal)).toBe(true);

    component.unmount();
});

it('sets dense margin class when {margin} prop is set to "dense"', () => {
    const mount = createMount();

    const node = (
        <VectorField
            items={2}
            value={[0, 1]}
            margin="dense"
        />
    );
    const classes = getClasses(node);
    const component = mount(node);

    expect(component.getDOMNode()
        .classList
        .contains(classes.marginDense)).toBe(true);

    component.unmount();
});

it('renders a <Typography> node if {label} prop is set', () => {
    const mount = createMount();

    const label = 'Test';
    const component = mount(<VectorField items={2} value={[0, 1]} label={label} />);

    const typography = component.find(Typography);

    expect(typography.length).toBe(1);
    expect(typography.text()).toBe(label);

    component.unmount();
    mount.cleanUp();
});

it('does not render a <Typography> node if {label} prop is not set', () => {
    const mount = createMount();
    const component = mount(<VectorField items={2} value={[0, 1]} />);
    const typography = component.find(Typography);

    expect(typography.length).toBe(0);

    component.unmount();
    mount.cleanUp();
});

it('renders as many <NumericField> components as {items}', () => {
    const mount = createMount();

    const component = mount(<VectorField items={2} value={[0, 1]} />);

    const fields = component.find(NumericField);
    expect(fields.length).toBe(2);

    component.unmount();
    mount.cleanUp();
});

it('renders <NumericField> components with {fullWidth, noSlider} preset', () => {
    const mount = createMount();

    const component = mount(<VectorField items={2} value={[0, 1]} />);

    const fields = component.find(NumericField);

    const x = fields.at(0);
    expect(x.prop('noSlider')).toBe(true);

    const y = fields.at(1);
    expect(y.prop('noSlider')).toBe(true);

    component.unmount();
    mount.cleanUp();
});

it('renders <NumericField> components with {disabled} inherited from the current props', () => {
    const mount = createMount();

    const component = mount(
        <VectorField
            items={2}
            value={[0, 1]}
            disabled
        />
    );

    const fields = component.find(NumericField);

    const x = fields.at(0);
    expect(x.prop('disabled')).toBe(true);

    const y = fields.at(1);
    expect(y.prop('disabled')).toBe(true);

    component.unmount();
    mount.cleanUp();
});

it('renders <NumericField> components with {type} inherited from the current props', () => {
    const mount = createMount();

    const component = mount(
        <VectorField
            items={2}
            value={[0, 1]}
            type="float"
        />
    );

    const fields = component.find(NumericField);

    const x = fields.at(0);
    expect(x.prop('type')).toBe('float');

    const y = fields.at(1);
    expect(y.prop('type')).toBe('float');

    component.unmount();
    mount.cleanUp();
});

it('renders <NumericField> components with {min, max, step} from the current props', () => {
    const mount = createMount();

    const component = mount(
        <VectorField
            items={2}
            value={[0, 1]}
            min={[0, 1]}
            max={[1, 100]}
            step={[0.01, 1]}
        />
    );

    const fields = component.find(NumericField);

    const x = fields.at(0);
    expect(x.prop('min')).toBe(0);
    expect(x.prop('max')).toBe(1);
    expect(x.prop('step')).toBe(0.01);

    const y = fields.at(1);
    expect(y.prop('min')).toBe(1);
    expect(y.prop('max')).toBe(100);
    expect(y.prop('step')).toBe(1);

    component.unmount();
    mount.cleanUp();
});

it('renders <NumericField> components with {value} inherited from the current props', () => {
    const mount = createMount();

    const component = mount(<VectorField items={2} value={[0, 1]} />);

    const fields = component.find(NumericField);

    const x = fields.at(0);
    expect(x.prop('value')).toBe(0);

    const y = fields.at(1);
    expect(y.prop('value')).toBe(1);

    component.unmount();
    mount.cleanUp();
});

it('runs {onChange} if any of the <NumericField> components\' value changes', () => {
    const mount = createMount();

    const onChangeSpy = jest.fn();

    const component = mount(
        <VectorField
            items={2}
            value={[0, 1]}
            onChange={onChangeSpy}
        />
    );

    const fields = component.find(NumericField);

    const x = fields.at(0);
    const y = fields.at(1);

    x.props()
        .onChange(0.01);
    expect(onChangeSpy).toHaveBeenCalledWith([0.01, 1]);
    y.props()
        .onChange(0.1);
    expect(onChangeSpy).toHaveBeenCalledWith([0.01, 1]);

    expect(onChangeSpy).toHaveBeenCalledTimes(2);

    component.unmount();
    mount.cleanUp();
});

it('rerenders if {value} prop changed', () => {
    const shallow = createShallow({dive: true});

    const component = shallow(<VectorField items={2} value={[0, 0]} />);

    const instance = component.instance();
    const renderSpy = spyOn(instance, 'render');

    component.setProps({
        value: [1, 1]
    });

    expect(renderSpy).toHaveBeenCalledTimes(1);

    component.unmount();
});

it('does not rerender if {value} prop is updated with the same value instance', () => {
    const shallow = createShallow({dive: true});

    const value = [0, 0, 0];
    const props = {value};
    const component = shallow(<VectorField items={2} {...props} />);

    const instance = component.instance();
    const renderSpy = spyOn(instance, 'render');

    component.setProps({value});

    expect(renderSpy).toHaveBeenCalledTimes(0);

    component.unmount();
});

it('does not rerender if {value} prop is updated with the same value', () => {
    const shallow = createShallow({dive: true});

    const component = shallow(
        <VectorField
            items={2}
            value={[0, 1]}
        />
    );

    const instance = component.instance();
    const renderSpy = spyOn(instance, 'render');

    component.setProps({
        value: [0, 1]
    });

    expect(renderSpy).toHaveBeenCalledTimes(0);

    component.unmount();
});

it('rerenders if {disabled} prop changed', () => {
    const shallow = createShallow({dive: true});

    const component = shallow(
        <VectorField
            items={2}
            value={[0, 1]}
            disabled
        />
    );

    const instance = component.instance();
    const renderSpy = spyOn(instance, 'render');

    component.setProps({
        disabled: false
    });

    expect(renderSpy).toHaveBeenCalledTimes(1);

    component.unmount();
});

it('does not rerender if {disabled} prop is updated with the same value', () => {
    const shallow = createShallow({dive: true});

    const component = shallow(
        <VectorField
            items={2}
            value={[0, 1]}
            disabled
        />
    );

    const instance = component.instance();
    const renderSpy = spyOn(instance, 'render');

    component.setProps({
        disabled: true
    });

    expect(renderSpy).toHaveBeenCalledTimes(0);

    component.unmount();
});

it('rerenders if {label} prop changed', () => {
    const shallow = createShallow({dive: true});

    const component = shallow(
        <VectorField
            items={2}
            value={[0, 1]}
            label="Label 1"
        />
    );

    const instance = component.instance();
    const renderSpy = spyOn(instance, 'render');

    component.setProps({
        label: 'Label 2'
    });

    expect(renderSpy).toHaveBeenCalledTimes(1);

    component.unmount();
});

it('does not rerender if {label} prop is updated with the same value', () => {
    const shallow = createShallow({dive: true});

    const component = shallow(
        <VectorField
            items={2}
            value={[0, 1]}
            label="Label"
        />
    );

    const instance = component.instance();
    const renderSpy = spyOn(instance, 'render');

    component.setProps({
        label: 'Label'
    });

    expect(renderSpy).toHaveBeenCalledTimes(0);

    component.unmount();
});

it('rerenders if {onChange} prop changed', () => {
    const shallow = createShallow({dive: true});

    const component = shallow(
        <VectorField
            items={2}
            value={[0, 1]}
            onChange={noop}
        />
    );

    const instance = component.instance();
    const renderSpy = spyOn(instance, 'render');

    component.setProps({
        // tslint:disable: no-empty
        onChange: () => {}
    });

    expect(renderSpy).toHaveBeenCalledTimes(1);

    component.unmount();
});

it('does not rerender if {onChange} prop is updated with the same value', () => {
    const shallow = createShallow({dive: true});

    const component = shallow(
        <VectorField
            items={2}
            value={[0, 1]}
            onChange={noop}
        />
    );

    const instance = component.instance();
    const renderSpy = spyOn(instance, 'render');

    component.setProps({
        onChange: noop
    });

    expect(renderSpy).toHaveBeenCalledTimes(0);

    component.unmount();
});

it('rerenders if {min} prop changed', () => {
    const shallow = createShallow({dive: true});

    const component = shallow(
        <VectorField
            items={2}
            value={[0, 1]}
            min={[0, 1, 0]}
        />
    );

    const instance = component.instance();
    const renderSpy = spyOn(instance, 'render');

    component.setProps({
        min: [1, 1, 0]
    });

    expect(renderSpy).toHaveBeenCalledTimes(1);

    component.unmount();
});

it('does not rerender if {min} prop is updated with the same value instance', () => {
    const shallow = createShallow({dive: true});

    const min = [0, 1, 0];
    const component = shallow(
        <VectorField
            items={2}
            value={[0, 1]}
            min={min}
        />
    );

    const instance = component.instance();
    const renderSpy = spyOn(instance, 'render');

    component.setProps({min});

    expect(renderSpy).toHaveBeenCalledTimes(0);

    component.unmount();
});

it('does not rerender if {min} prop is updated with the same value', () => {
    const shallow = createShallow({dive: true});

    const component = shallow(
        <VectorField
            items={2}
            value={[0, 1]}
            min={[0, 1, 0]}
        />
    );

    const instance = component.instance();
    const renderSpy = spyOn(instance, 'render');

    component.setProps({
        min: [0, 1, 0]
    });

    expect(renderSpy).toHaveBeenCalledTimes(0);

    component.unmount();
});

it('rerenders if {max} prop changed', () => {
    const shallow = createShallow({dive: true});

    const component = shallow(
        <VectorField
            items={2}
            value={[0, 1]}
            max={[0, 1, 0]}
        />
    );

    const instance = component.instance();
    const renderSpy = spyOn(instance, 'render');

    component.setProps({
        max: [1, 1, 0]
    });

    expect(renderSpy).toHaveBeenCalledTimes(1);

    component.unmount();
});

it('does not rerender if {max} prop is updated with the same value instance', () => {
    const shallow = createShallow({dive: true});

    const max = [0, 1, 1];
    const component = shallow(
        <VectorField
            items={2}
            value={[0, 1]}
            max={max}
        />
    );

    const instance = component.instance();
    const renderSpy = spyOn(instance, 'render');

    component.setProps({max});

    expect(renderSpy).toHaveBeenCalledTimes(0);

    component.unmount();
});

it('does not rerender if {max} prop is updated with the same value', () => {
    const shallow = createShallow({dive: true});

    const component = shallow(
        <VectorField
            items={2}
            value={[0, 1]}
            max={[0, 1, 1]}
        />
    );

    const instance = component.instance();
    const renderSpy = spyOn(instance, 'render');

    component.setProps({
        max: [0, 1, 1]
    });

    expect(renderSpy).toHaveBeenCalledTimes(0);

    component.unmount();
});

it('rerenders if {step} prop changed', () => {
    const shallow = createShallow({dive: true});

    const component = shallow(
        <VectorField
            items={2}
            value={[0, 1]}
            step={[0, 1, 0]}
        />
    );

    const instance = component.instance();
    const renderSpy = spyOn(instance, 'render');

    component.setProps({
        step: [1, 1, 0]
    });

    expect(renderSpy).toHaveBeenCalledTimes(1);

    component.unmount();
});

it('does not rerender if {step} prop is updated with the same value instance', () => {
    const shallow = createShallow({dive: true});

    const step = [0, 1, 0];
    const component = shallow(
        <VectorField
            items={2}
            value={[0, 1]}
            step={step}
        />
    );

    const instance = component.instance();
    const renderSpy = spyOn(instance, 'render');

    component.setProps({step});

    expect(renderSpy).toHaveBeenCalledTimes(0);

    component.unmount();
});

it('does not rerender if {step} prop is updated with the same value', () => {
    const shallow = createShallow({dive: true});

    const component = shallow(
        <VectorField
            items={2}
            value={[0, 1]}
            step={[0, 1, 0]}
        />
    );

    const instance = component.instance();
    const renderSpy = spyOn(instance, 'render');

    component.setProps({
        step: [0, 1, 0]
    });

    expect(renderSpy).toHaveBeenCalledTimes(0);

    component.unmount();
});
