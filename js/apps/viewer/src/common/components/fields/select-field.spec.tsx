import React from 'react';

import {createMount, createShallow} from '@material-ui/core/test-utils';

import FormControl, {FormControlProps} from '@material-ui/core/FormControl';
import FormHelperText from '@material-ui/core/FormHelperText';
import Input from '@material-ui/core/Input';
import InputLabel from '@material-ui/core/InputLabel';
import MenuItem from '@material-ui/core/MenuItem';
import Select from '@material-ui/core/Select';

import SelectField from './select-field';


it('renders without crashing', () => {
    const shallow = createShallow();
    const component = shallow(<SelectField />);
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
    const className = 'test';
    const component = mount(
        <SelectField
            className={className}
            value=""
            {...props as any}
        />
    );

    const control = component.find(FormControl);

    expect(control.length).toBe(1);
    expect(control.prop('className')).toBe(className);
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
        <SelectField
            id={id}
            label={label}
            value=""
            shrink
        />
    );

    const inputLabel = component.find(InputLabel);

    expect(inputLabel.length).toBe(1);
    expect(inputLabel.prop('htmlFor')).toBe(id);
    expect(inputLabel.prop('shrink')).toBe(true);
    expect(inputLabel.text()).toBe(label);

    component.unmount();
    mount.cleanUp();
});

it('renders a <FormHelperText> component if {helperText} is set', () => {
    const mount = createMount();

    const component = mount(<SelectField value="" />);

    expect(component.find(FormHelperText)).toHaveLength(0);

    const text = 'Just a hint';
    component.setProps({
        helperText: text
    });

    const helperText = component.find(FormHelperText);

    expect(helperText).toHaveLength(1);
    expect(helperText.text()).toBe(text);

    component.unmount();
    mount.cleanUp();
});

it('renders a <Input> component', () => {
    const mount = createMount();

    const id = 'test';
    const label = 'Test';
    const component = mount(
        <SelectField
            id={id}
            label={label}
            value=""
        />
    );

    const input = component.find(Input);

    expect(input.length).toBe(1);
    expect(input.prop('id')).toBe(id);

    component.unmount();
    mount.cleanUp();
});

it('renders a <Select> component', () => {
    const mount = createMount();

    const options = [
        'Ron',
        'Leslie'
    ];
    const component = mount(
        <SelectField
            options={options}
            value=""
        />
    );

    const select = component.find(Select);
    expect(select.length).toBe(1);
    expect(select.prop('displayEmpty')).toBe(true);

    component.unmount();
    mount.cleanUp();
});

it('renders a <MenuItem> component for each option', () => {
    const shallow = createShallow();

    const options = [
        'Ron',
        'Leslie'
    ];
    const component = shallow((
        <SelectField
            options={options}
            value=""
        />
    ));

    component.find(Select)
        .dive()
        .simulate('click');

    component.update();

    const menuItems = component.find(MenuItem);
    expect(menuItems).toHaveLength(2);

    expect(menuItems.at(0)
        .dive()
        .render()
        .text()).toEqual(options[0]);

    expect(menuItems.at(1)
        .dive()
        .render()
        .text()).toEqual(options[1]);

    component.unmount();
});

it('renders a <MenuItem> with text mapped to {displayName} result', () => {
    const shallow = createShallow();

    const options = [
        'Ron'
    ];
    const component = shallow((
        <SelectField
            options={options}
            displayName={displayName}
            value=""
        />
    ));

    component.find(Select)
        .dive()
        .simulate('click');

    component.update();

    const menuItems = component.find(MenuItem);

    expect(menuItems.at(0)
        .dive()
        .render()
        .text()).toEqual('ron');

    component.unmount();

    function displayName(key: string) {
        return key.toLowerCase();
    }
});

it('renders <MenuItem> components from {children}', () => {
    const shallow = createShallow();

    const name = 'Michael Scott';
    const component = shallow((
        <SelectField
            options={[]}
            value=""
        >
            <MenuItem>
                {name}
            </MenuItem>
        </SelectField>
    ));

    component.find(Select)
        .dive()
        .simulate('click');

    component.update();

    const menuItems = component.find(MenuItem);
    expect(menuItems).toHaveLength(1);

    expect(menuItems.at(0)
        .dive()
        .render()
        .text()).toEqual(name);

    component.unmount();
});

it('updates the <Select> value when the value updates', () => {
    const mount = createMount();

    const option = 'Test';
    const component = mount(<SelectField value="" />);

    expect(component.find(Select)
        .prop('value')).toBe('');

    component.setProps({
        value: option
    });

    expect(component.find(Select)
        .prop('value')).toBe(option);

    component.unmount();
    mount.cleanUp();
});

it('runs {onChange} if the <Select> value changes', () => {
    const mount = createMount();

    const onChangeSpy = jest.fn();

    const component = mount(
        <SelectField
            value=""
            onChange={onChangeSpy}
        />
    );

    const change = {target: {value: 1}};
    const select = component.find(Select);
    select.props()
        .onChange(change);
    expect(onChangeSpy).toHaveBeenCalledWith(change);

    component.unmount();
    mount.cleanUp();
});
