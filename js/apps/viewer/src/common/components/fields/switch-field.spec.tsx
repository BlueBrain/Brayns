import React from 'react';

import {createMount, createShallow} from '@material-ui/core/test-utils';

import FormControl, {FormControlProps} from '@material-ui/core/FormControl';
import FormControlLabel from '@material-ui/core/FormControlLabel';
import Switch from '@material-ui/core/Switch';

import SwitchField from './switch-field';


it('renders without crashing', () => {
    const shallow = createShallow();
    const component = shallow(<SwitchField />);
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
    const component = mount(<SwitchField {...props as any} />);

    const control = component.find(FormControl);

    expect(control.length).toBe(1);
    expect(control.prop('margin')).toBe(props.margin);
    expect(control.prop('fullWidth')).toBe(props.fullWidth);
    expect(control.prop('required')).toBe(props.required);
    expect(control.prop('disabled')).toBe(props.disabled);
    expect(control.prop('error')).toBe(props.error);

    component.unmount();
    mount.cleanUp();
});

it('renders a <FormControlLabel> component', () => {
    const mount = createMount();

    const id = 'test';
    const label = 'Test';
    const component = mount(<SwitchField id={id} label={label} />);

    const control = component.find(FormControlLabel);

    expect(control.length).toBe(1);
    expect(control.prop('id')).toBe(id);
    expect(control.prop('label')).toBe(label);

    component.unmount();
    mount.cleanUp();
});

it('renders a <Switch> component', () => {
    const mount = createMount();

    const label = 'Test';
    const component = mount(<SwitchField label={label} />);

    const switchToggle = component.find(Switch);

    expect(switchToggle.length).toBe(1);
    expect(switchToggle.prop('aria-label')).toBe(label);

    component.unmount();
    mount.cleanUp();
});

it('updates the <Switch> value when the value updates', () => {
    const mount = createMount();

    const component = mount(<SwitchField value={false} />);

    expect(component.find(Switch)
        .prop('checked')).toBe(false);

    component.setProps({
        value: true
    });

    expect(component.find(Switch)
        .prop('checked')).toBe(true);

    component.unmount();
    mount.cleanUp();
});

it('runs {onChange} if the <Slider> value changes', () => {
    const mount = createMount();

    const onChangeSpy = jest.fn();

    const component = mount(
        <SwitchField
            value={false}
            onChange={onChangeSpy}
        />
    );

    const control = component.find(Switch);
    control.props()
        .onChange({}, true);
    expect(onChangeSpy).toHaveBeenCalledWith({}, true);

    component.unmount();
    mount.cleanUp();
});
