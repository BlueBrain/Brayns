import React from 'react';

import {noop} from 'lodash';

import {createMount, createShallow} from '@material-ui/core/test-utils';

import NumericField from './numeric-field';
import Vector3Field from './vector3-field';

const defaultProps = {value: [0, 0, 0]};

it('renders without crashing', () => {
    const shallow = createShallow();
    const component = shallow(<Vector3Field {...defaultProps} />);
    component.unmount();
});

it('has {className} property', () => {
    const shallow = createShallow({dive: true});

    const className = 'test-cls';
    const component = shallow(
        <Vector3Field
            className={className}
            {...defaultProps}
        />
    );

    const item = component.find(`.${className}`);
    expect(item.length).toBe(1);

    component.unmount();
});

it('renders 3 <NumericField> components', () => {
    const mount = createMount();

    const component = mount(<Vector3Field {...defaultProps} />);

    const fields = component.find(NumericField);
    expect(fields.length).toBe(3);

    component.unmount();
    mount.cleanUp();
});

it('renders 3 <NumericField> components with {fullWidth, noSlider} preset', () => {
    const mount = createMount();

    const component = mount(<Vector3Field {...defaultProps} />);

    const fields = component.find(NumericField);

    const x = fields.at(0);
    expect(x.prop('fullWidth')).toBe(true);
    expect(x.prop('noSlider')).toBe(true);

    const y = fields.at(1);
    expect(y.prop('fullWidth')).toBe(true);
    expect(y.prop('noSlider')).toBe(true);

    const z = fields.at(2);
    expect(z.prop('fullWidth')).toBe(true);
    expect(z.prop('noSlider')).toBe(true);

    component.unmount();
    mount.cleanUp();
});

it('renders 3 <NumericField> components with {label, disabled} inherited from the current props', () => {
    const mount = createMount();

    const component = mount(
        <Vector3Field
            {...defaultProps}
            label={['X', 'Y', 'Z']}
            disabled
        />
    );

    const fields = component.find(NumericField);

    const x = fields.at(0);
    expect(x.prop('label')).toBe('X');
    expect(x.prop('disabled')).toBe(true);

    const y = fields.at(1);
    expect(y.prop('label')).toBe('Y');
    expect(y.prop('disabled')).toBe(true);

    const z = fields.at(2);
    expect(z.prop('label')).toBe('Z');
    expect(z.prop('disabled')).toBe(true);

    component.unmount();
    mount.cleanUp();
});

it('renders 3 <NumericField> components with {min, max, step} from the current props', () => {
    const mount = createMount();

    const props = {
        min: [0, 1, 0],
        max: [1, 100, 1],
        step: [0.01, 1, 0.01]
    };
    const component = mount(
        <Vector3Field
            {...defaultProps}
            {...props}
        />
    );

    const fields = component.find(NumericField);

    const x = fields.at(0);
    const xProps = propsAt(props, 0);
    expect(x.prop('min')).toBe(xProps.min);
    expect(x.prop('max')).toBe(xProps.max);
    expect(x.prop('step')).toBe(xProps.step);

    const y = fields.at(1);
    const yProps = propsAt(props, 1);
    expect(y.prop('min')).toBe(yProps.min);
    expect(y.prop('max')).toBe(yProps.max);
    expect(y.prop('step')).toBe(yProps.step);

    const z = fields.at(2);
    const zProps = propsAt(props, 2);
    expect(z.prop('min')).toBe(zProps.min);
    expect(z.prop('max')).toBe(zProps.max);
    expect(z.prop('step')).toBe(zProps.step);

    component.unmount();
    mount.cleanUp();
});

it('renders 3 <NumericField> components with {value} inherited from the current props', () => {
    const mount = createMount();

    const props = {
        value: [0.1, 1, 0.01]
    };
    const component = mount(
        <Vector3Field {...props} />
    );

    const fields = component.find(NumericField);

    const x = fields.at(0);
    const xProps = propsAt(props, 0);
    expect(x.prop('value')).toBe(xProps.value);

    const y = fields.at(1);
    const yProps = propsAt(props, 1);
    expect(y.prop('value')).toBe(yProps.value);

    const z = fields.at(2);
    const zProps = propsAt(props, 2);
    expect(z.prop('value')).toBe(zProps.value);

    component.unmount();
    mount.cleanUp();
});

it('runs {onChange} if any of the <NumericField> components\' value changes', () => {
    const mount = createMount();

    const onChangeSpy = jest.fn();

    const component = mount(
        <Vector3Field
            {...defaultProps}
            onChange={onChangeSpy}
        />
    );

    const fields = component.find(NumericField);

    const x = fields.at(0);
    const y = fields.at(1);
    const z = fields.at(2);

    x.props()
        .onChange(0.01);
    expect(onChangeSpy).toHaveBeenCalledWith([0.01, 0, 0]);
    y.props()
        .onChange(0.1);
    expect(onChangeSpy).toHaveBeenCalledWith([0, 0.1, 0]);
    z.props()
        .onChange(1);
    expect(onChangeSpy).toHaveBeenCalledWith([0, 0, 1]);

    expect(onChangeSpy).toHaveBeenCalledTimes(3);

    component.unmount();
    mount.cleanUp();
});

it('rerenders if {value} prop changed', () => {
    const shallow = createShallow({dive: true});

    const component = shallow(<Vector3Field value={[0, 0, 0]} />);

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
    const component = shallow(<Vector3Field {...props} />);

    const instance = component.instance();
    const renderSpy = spyOn(instance, 'render');

    component.setProps({value});

    expect(renderSpy).toHaveBeenCalledTimes(0);

    component.unmount();
});

it('does not rerender if {value} prop is updated with the same value', () => {
    const shallow = createShallow({dive: true});

    const component = shallow(
        <Vector3Field
            value={[1, 2, 3]}
        />
    );

    const instance = component.instance();
    const renderSpy = spyOn(instance, 'render');

    component.setProps({
        value: [1, 2, 3]
    });

    expect(renderSpy).toHaveBeenCalledTimes(0);

    component.unmount();
});

it('rerenders if {disabled} prop changed', () => {
    const shallow = createShallow({dive: true});

    const component = shallow(
        <Vector3Field
            {...defaultProps}
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
        <Vector3Field
            {...defaultProps}
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
        <Vector3Field
            {...defaultProps}
            label={['X', 'Y', 'Z']}
        />
    );

    const instance = component.instance();
    const renderSpy = spyOn(instance, 'render');

    component.setProps({
        label: ['M', 'P', 'Q']
    });

    expect(renderSpy).toHaveBeenCalledTimes(1);

    component.unmount();
});

it('does not rerender if {label} prop is updated with the same value instance', () => {
    const shallow = createShallow({dive: true});

    const label = ['X', 'Y', 'Z'];
    const props = {...defaultProps, label};
    const component = shallow(<Vector3Field {...props} />);

    const instance = component.instance();
    const renderSpy = spyOn(instance, 'render');

    component.setProps({label});

    expect(renderSpy).toHaveBeenCalledTimes(0);

    component.unmount();
});

it('does not rerender if {label} prop is updated with the same value', () => {
    const shallow = createShallow({dive: true});

    const component = shallow(
        <Vector3Field
            {...defaultProps}
            label={['X', 'Y', 'Z']}
        />
    );

    const instance = component.instance();
    const renderSpy = spyOn(instance, 'render');

    component.setProps({
        label: ['X', 'Y', 'Z']
    });

    expect(renderSpy).toHaveBeenCalledTimes(0);

    component.unmount();
});

it('rerenders if {onChange} prop changed', () => {
    const shallow = createShallow({dive: true});

    const component = shallow(
        <Vector3Field
            {...defaultProps}
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
        <Vector3Field
            {...defaultProps}
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
        <Vector3Field
            {...defaultProps}
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
    const props = {...defaultProps, min};
    const component = shallow(<Vector3Field {...props} />);

    const instance = component.instance();
    const renderSpy = spyOn(instance, 'render');

    component.setProps({min});

    expect(renderSpy).toHaveBeenCalledTimes(0);

    component.unmount();
});

it('does not rerender if {min} prop is updated with the same value', () => {
    const shallow = createShallow({dive: true});

    const component = shallow(
        <Vector3Field
            {...defaultProps}
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
        <Vector3Field
            {...defaultProps}
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
    const props = {...defaultProps, max};
    const component = shallow(<Vector3Field {...props} />);

    const instance = component.instance();
    const renderSpy = spyOn(instance, 'render');

    component.setProps({max});

    expect(renderSpy).toHaveBeenCalledTimes(0);

    component.unmount();
});

it('does not rerender if {max} prop is updated with the same value', () => {
    const shallow = createShallow({dive: true});

    const component = shallow(
        <Vector3Field
            {...defaultProps}
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
        <Vector3Field
            {...defaultProps}
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
    const props = {...defaultProps, step};
    const component = shallow(<Vector3Field {...props} />);

    const instance = component.instance();
    const renderSpy = spyOn(instance, 'render');

    component.setProps({step});

    expect(renderSpy).toHaveBeenCalledTimes(0);

    component.unmount();
});

it('does not rerender if {step} prop is updated with the same value', () => {
    const shallow = createShallow({dive: true});

    const component = shallow(
        <Vector3Field
            {...defaultProps}
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


function propsAt(props: Props, index: number): Props {
    const {min, max, step, value} = props;

    const result = {};

    if (Array.isArray(min)) {
        Object.assign(result, {
            min: min[index]
        });
    }
    if (Array.isArray(max)) {
        Object.assign(result, {
            max: max[index]
        });
    }
    if (Array.isArray(step)) {
        Object.assign(result, {
            step: step[index]
        });
    }
    if (Array.isArray(value)) {
        Object.assign(result, {
            ...result,
            value: value[index]
        });
    }

    return result;
}

interface Props {
    min?: number[];
    max?: number[];
    step?: number[];
    value?: number[];
}
