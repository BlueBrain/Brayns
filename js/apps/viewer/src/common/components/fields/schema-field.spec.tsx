import React from 'react';

import {JSONSchema7} from 'json-schema';
import {noop} from 'lodash';

import {createMount, createShallow} from '@material-ui/core/test-utils';
import TextField from '@material-ui/core/TextField';

import NumericField from './numeric-field';
import SelectField from './select-field';
import SwitchField from './switch-field';
import VectorField from './vector-field';

import SchemaField, {
    getLabel,
    getNumericFieldStep,
    orderOfMagnitude
} from './schema-field';


it('renders without crashing', () => {
    const shallow = createShallow();
    const component = shallow(
        <SchemaField
            name="test"
            schema={{}}
            value={0}
            onChange={noop}
        />
    );
    component.unmount();
});

it('renders <NumericField> for schema type "number"', () => {
    const mount = createMount();

    const name = 'test';
    const schema: JSONSchema7 = {
        type: 'number'
    };

    const component = mount(
        <SchemaField
            name={name}
            schema={schema}
            value={0}
            onChange={noop}
        />
    );

    const field = component.find(NumericField);

    expect(field.length).toBe(1);
    expect(field.prop('id')).toBe(name);
    expect(field.prop('type')).toBe('float');

    component.unmount();
    mount.cleanUp();
});

it('renders <NumericField> for schema type "integer"', () => {
    const mount = createMount();

    const name = 'test';
    const schema: JSONSchema7 = {
        type: 'integer'
    };

    const component = mount(
        <SchemaField
            name={name}
            schema={schema}
            value={0}
            onChange={noop}
        />
    );

    const field = component.find(NumericField);

    expect(field.length).toBe(1);
    expect(field.prop('id')).toBe(name);
    expect(field.prop('type')).toBe('integer');

    component.unmount();
    mount.cleanUp();
});

it('uses schema {minimum, maximum} to set {min, max, step} on <NumericField>', () => {
    const mount = createMount();

    const schema: JSONSchema7 = {
        type: 'integer',
        minimum: 0,
        maximum: 100
    };

    const component = mount(
        <SchemaField
            name="test"
            schema={schema}
            value={0}
            onChange={noop}
        />
    );

    const field = component.find(NumericField);

    expect(field.prop('min')).toBe(schema.minimum);
    expect(field.prop('max')).toBe(schema.maximum);
    expect(field.prop('step')).toBe(1);

    component.unmount();
    mount.cleanUp();
});

it('uses schema {title} to set the {label} on <NumericField>', () => {
    const mount = createMount();

    const schema: JSONSchema7 = {
        type: 'integer',
        title: 'Numeric value'
    };

    const component = mount(
        <SchemaField
            name="test"
            schema={schema}
            value={0}
            onChange={noop}
        />
    );

    const field = component.find(NumericField);
    expect(field.prop('label')).toBe(schema.title);

    component.unmount();
    mount.cleanUp();
});

it('<NumericField> uses schema {default} if no value is provided', () => {
    const mount = createMount();

    const name = 'test';
    const schema: JSONSchema7 = {
        type: 'integer',
        title: 'Numeric value',
        default: 10
    };

    const component = mount(
        <SchemaField
            name={name}
            schema={schema}
            onChange={noop}
        />
    );

    expect(component.find(NumericField)
        .prop('value')).toBe(10);

    component.unmount();
    mount.cleanUp();
});

it('updates the <NumericField> value when the value updates', () => {
    const mount = createMount();

    const name = 'test';
    const schema: JSONSchema7 = {
        type: 'integer',
        title: 'Numeric value'
    };

    const component = mount(
        <SchemaField
            name={name}
            schema={schema}
            value={0}
            onChange={noop}
        />
    );

    expect(component.find(NumericField)
        .prop('value')).toBe(0);

    component.setProps({
        value: 10
    });

    expect(component.find(NumericField)
        .prop('value')).toBe(10);

    component.unmount();
    mount.cleanUp();
});

it('runs {onChange} if the <NumericField> value changes', () => {
    const mount = createMount();

    const onChangeSpy = jest.fn();
    const name = 'test';
    const schema: JSONSchema7 = {
        type: 'integer',
        title: 'Numeric value'
    };

    const component = mount(
        <SchemaField
            name={name}
            schema={schema}
            value={0}
            onChange={onChangeSpy}
        />
    );

    const field = component.find(NumericField);
    field.props()
        .onChange(10);
    expect(onChangeSpy).toHaveBeenCalledWith({
        [name]: 10
    });

    component.unmount();
    mount.cleanUp();
});

it('renders <VectorField> for schema type "array" with {items} of type "number"', () => {
    const mount = createMount();

    const name = 'test';
    const schema: JSONSchema7 = {
        type: 'array',
        maxItems: 2,
        items: {
            type: 'number'
        }
    };

    const component = mount(
        <SchemaField
            name={name}
            schema={schema}
            value={0}
            onChange={noop}
        />
    );

    const field = component.find(VectorField);

    expect(field.length).toBe(1);
    expect(field.prop('type')).toBe('float');

    component.unmount();
    mount.cleanUp();
});

it('renders <VectorField> for schema type "array" with {items} of type "integer"', () => {
    const mount = createMount();

    const name = 'test';
    const schema: JSONSchema7 = {
        type: 'array',
        maxItems: 2,
        items: {
            type: 'integer'
        }
    };

    const component = mount(
        <SchemaField
            name={name}
            schema={schema}
            value={0}
            onChange={noop}
        />
    );

    const field = component.find(VectorField);

    expect(field.length).toBe(1);
    expect(field.prop('type')).toBe('integer');

    component.unmount();
    mount.cleanUp();
});

it('uses schema {maxItems, minimum, maximum} to set {items, min, max, step} on <VectorField>', () => {
    const mount = createMount();

    const minimum = 0;
    const maximum = 100;
    const schema: JSONSchema7 = {
        type: 'array',
        maxItems: 3,
        items: {
            minimum,
            maximum,
            type: 'integer'
        }
    };

    const component = mount(
        <SchemaField
            name="test"
            schema={schema}
            value={[0, 0, 0]}
            onChange={noop}
        />
    );

    const field = component.find(VectorField);

    expect(field.prop('items')).toBe(schema.maxItems);
    expect(field.prop('min')).toEqual([minimum, minimum, minimum]);
    expect(field.prop('max')).toEqual([maximum, maximum, maximum]);
    expect(field.prop('step')).toEqual([1, 1, 1]);

    component.unmount();
    mount.cleanUp();
});

it('uses schema {title} to set the {label} on <VectorField>', () => {
    const mount = createMount();

    const schema: JSONSchema7 = {
        type: 'array',
        title: 'Numeric value',
        maxItems: 2,
        items: {
            type: 'integer'
        }
    };

    const component = mount(
        <SchemaField
            name="test"
            schema={schema}
            value={0}
            onChange={noop}
        />
    );

    const field = component.find(VectorField);
    expect(field.prop('label')).toBe(schema.title);

    component.unmount();
    mount.cleanUp();
});

it('<VectorField> uses schema {default} if no value is provided', () => {
    const mount = createMount();

    const name = 'test';
    const schema: JSONSchema7 = {
        type: 'array',
        title: 'Numeric value',
        items: {
            type: 'integer'
        },
        maxItems: 2,
        default: [0, 1]
    };

    const component = mount(
        <SchemaField
            name={name}
            schema={schema}
            onChange={noop}
        />
    );

    expect(component.find(VectorField)
        .prop('value')).toEqual([0, 1]);

    component.unmount();
    mount.cleanUp();
});

it('updates the <VectorField> value when the value updates', () => {
    const mount = createMount();

    const name = 'test';
    const schema: JSONSchema7 = {
        type: 'array',
        title: 'Numeric value',
        items: {
            type: 'integer'
        },
        maxItems: 2
    };

    const component = mount(
        <SchemaField
            name={name}
            schema={schema}
            value={[0, 0]}
            onChange={noop}
        />
    );

    expect(component.find(VectorField)
        .prop('value')).toEqual([0, 0]);

    component.setProps({
        value: [0, 1]
    });

    expect(component.find(VectorField)
        .prop('value')).toEqual([0, 1]);

    component.unmount();
    mount.cleanUp();
});

it('runs {onChange} if the <VectorField> value changes', () => {
    const mount = createMount();

    const onChangeSpy = jest.fn();
    const name = 'test';
    const schema: JSONSchema7 = {
        type: 'array',
        title: 'Numeric value',
        items: {
            type: 'integer'
        },
        maxItems: 2
    };

    const component = mount(
        <SchemaField
            name={name}
            schema={schema}
            value={[0, 0]}
            onChange={onChangeSpy}
        />
    );

    const field = component.find(VectorField);
    field.props()
        .onChange([0, 1]);
    expect(onChangeSpy).toHaveBeenCalledWith({
        [name]: [0, 1]
    });

    component.unmount();
    mount.cleanUp();
});

it('renders <TextField> for schema type "string"', () => {
    const mount = createMount();

    const name = 'test';
    const schema: JSONSchema7 = {
        type: 'string'
    };

    const component = mount(
        <SchemaField
            name={name}
            schema={schema}
            value={''}
            onChange={noop}
        />
    );

    const field = component.find(TextField);

    expect(field.length).toBe(1);
    expect(field.prop('id')).toBe(name);
    expect(field.prop('type')).toBe('text');

    component.unmount();
    mount.cleanUp();
});

it('uses schema {title} to set the {label} on <TextField>', () => {
    const mount = createMount();

    const schema: JSONSchema7 = {
        type: 'string',
        title: 'String value'
    };

    const component = mount(
        <SchemaField
            name="test"
            schema={schema}
            value={''}
            onChange={noop}
        />
    );

    const field = component.find(TextField);
    expect(field.prop('label')).toBe(schema.title);

    component.unmount();
    mount.cleanUp();
});

it('<TextField> uses schema {default} if no value is provided', () => {
    const mount = createMount();

    const name = 'test';
    const schema: JSONSchema7 = {
        type: 'string',
        title: 'Numeric value',
        default: 'what a value'
    };

    const component = mount(
        <SchemaField
            name={name}
            schema={schema}
            onChange={noop}
        />
    );

    expect(component.find(TextField)
        .prop('value')).toBe('what a value');

    component.unmount();
    mount.cleanUp();
});

it('updates the <TextField> value when the value updates', () => {
    const mount = createMount();

    const name = 'test';
    const schema: JSONSchema7 = {
        type: 'string',
        title: 'Name'
    };

    const component = mount(
        <SchemaField
            name={name}
            schema={schema}
            value={''}
            onChange={noop}
        />
    );

    expect(component.find(TextField)
        .prop('value')).toBe('');

    component.setProps({
        value: 'John Doe'
    });

    expect(component.find(TextField)
        .prop('value')).toBe('John Doe');

    component.unmount();
    mount.cleanUp();
});

it('runs {onChange} if the <TextField> value changes', () => {
    const mount = createMount();

    const onChangeSpy = jest.fn();
    const name = 'test';
    const schema: JSONSchema7 = {
        type: 'string',
        title: 'Name'
    };

    const component = mount(
        <SchemaField
            name={name}
            schema={schema}
            value={''}
            onChange={onChangeSpy}
        />
    );

    const field = component.find(TextField);
    field.props()
        .onChange({
            target: {value: 'Jane Doe'}
        });
    expect(onChangeSpy).toHaveBeenCalledWith({
        [name]: 'Jane Doe'
    });

    component.unmount();
    mount.cleanUp();
});

it('renders <SelectField> for schema type "string" with {enum}', () => {
    const mount = createMount();

    const name = 'test';
    const schema: JSONSchema7 = {
        type: 'string',
        enum: [
            'Ron',
            'Leslie'
        ]
    };

    const component = mount(
        <SchemaField
            name={name}
            schema={schema}
            value={''}
            onChange={noop}
        />
    );

    const field = component.find(SelectField);
    expect(field.length).toBe(1);
    expect(field.prop('id')).toBe(name);
    expect(field.prop('options')).toEqual(schema.enum);

    component.unmount();
    mount.cleanUp();
});

it('does not render <SelectField> for schema type "string" if there is no {enum} on the schema', () => {
    const mount = createMount();

    const name = 'test';
    const schema: JSONSchema7 = {
        type: 'string'
    };

    const component = mount(
        <SchemaField
            name={name}
            schema={schema}
            value={''}
            onChange={noop}
        />
    );

    const field = component.find(SelectField);
    expect(field.length).toBe(0);

    component.unmount();
    mount.cleanUp();
});

it('uses schema {title} to set the {label} on <SelectField>', () => {
    const mount = createMount();

    const name = 'test';
    const schema: JSONSchema7 = {
        type: 'string',
        title: 'Select value',
        enum: [
            'Ron',
            'Leslie'
        ]
    };

    const component = mount(
        <SchemaField
            name={name}
            schema={schema}
            value={''}
            onChange={noop}
        />
    );

    const field = component.find(SelectField);
    expect(field.prop('label')).toEqual(schema.title);

    component.unmount();
    mount.cleanUp();
});

it('<SelectField> uses schema {default} if no value is provided', () => {
    const mount = createMount();

    const name = 'test';
    const ron = 'Ron';
    const schema: JSONSchema7 = {
        type: 'string',
        enum: [
            ron,
            'Leslie'
        ],
        default: ron
    };

    const component = mount(
        <SchemaField
            name={name}
            schema={schema}
            onChange={noop}
        />
    );

    expect(component.find(SelectField)
        .prop('value')).toBe(ron);

    component.unmount();
    mount.cleanUp();
});

it('updates the <SelectField> value when the value updates', () => {
    const mount = createMount();

    const name = 'test';
    const ron = 'Ron';
    const schema: JSONSchema7 = {
        type: 'string',
        enum: [
            ron,
            'Leslie'
        ]
    };

    const component = mount(
        <SchemaField
            name={name}
            schema={schema}
            value={''}
            onChange={noop}
        />
    );

    expect(component.find(SelectField)
        .prop('value')).toBe('');

    component.setProps({
        value: ron
    });

    expect(component.find(SelectField)
        .prop('value')).toBe(ron);

    component.unmount();
    mount.cleanUp();
});

it('runs {onChange} if the <SelectField> value changes', () => {
    const mount = createMount();

    const onChangeSpy = jest.fn();
    const name = 'test';
    const ron = 'Ron';
    const schema: JSONSchema7 = {
        type: 'string',
        enum: [
            ron,
            'Leslie'
        ]
    };

    const component = mount(
        <SchemaField
            name={name}
            schema={schema}
            value={''}
            onChange={onChangeSpy}
        />
    );

    const field = component.find(SelectField);
    const {onChange} = field.props();
    if (onChange) {
        onChange({
            target: {value: ron}
        } as any);
    }
    expect(onChangeSpy).toHaveBeenCalledWith({
        [name]: ron
    });

    component.unmount();
    mount.cleanUp();
});

it('renders <SwitchField> for schema type "boolean"', () => {
    const mount = createMount();

    const name = 'test';
    const schema: JSONSchema7 = {
        type: 'boolean'
    };

    const component = mount(
        <SchemaField
            name={name}
            schema={schema}
            value={''}
            onChange={noop}
        />
    );

    const field = component.find(SwitchField);
    expect(field.length).toBe(1);
    expect(field.prop('id')).toBe(name);

    component.unmount();
    mount.cleanUp();
});

it('uses schema {title} to set the {label} on <SwitchField>', () => {
    const mount = createMount();

    const schema: JSONSchema7 = {
        type: 'boolean',
        title: 'Boolean value'
    };

    const component = mount(
        <SchemaField
            name="test"
            schema={schema}
            value={false}
            onChange={noop}
        />
    );

    const field = component.find(SwitchField);
    expect(field.prop('label')).toBe(schema.title);

    component.unmount();
    mount.cleanUp();
});

it('<SwitchField> uses schema {default} if no value is provided', () => {
    const mount = createMount();

    const name = 'test';
    const schema: JSONSchema7 = {
        type: 'boolean',
        default: true
    };

    const component = mount(
        <SchemaField
            name={name}
            schema={schema}
            onChange={noop}
        />
    );

    expect(component.find(SwitchField)
        .prop('value')).toBe(true);

    component.unmount();
    mount.cleanUp();
});

it('updates the <SwitchField> value when the value updates', () => {
    const mount = createMount();

    const name = 'test';
    const schema: JSONSchema7 = {
        type: 'boolean'
    };

    const component = mount(
        <SchemaField
            name={name}
            schema={schema}
            value={false}
            onChange={noop}
        />
    );

    expect(component.find(SwitchField)
        .prop('value')).toBe(false);

    component.setProps({
        value: true
    });

    expect(component.find(SwitchField)
        .prop('value')).toBe(true);

    component.unmount();
    mount.cleanUp();
});

it('runs {onChange} if the <SwitchField> value changes', () => {
    const mount = createMount();

    const onChangeSpy = jest.fn();
    const name = 'test';
    const schema: JSONSchema7 = {
        type: 'boolean'
    };

    const component = mount(
        <SchemaField
            name={name}
            schema={schema}
            value={false}
            onChange={onChangeSpy}
        />
    );

    const field = component.find(SwitchField);
    const {onChange} = field.props();
    if (onChange) {
        onChange({} as any, true);
    }
    expect(onChangeSpy).toHaveBeenCalledWith({
        [name]: true
    });

    component.unmount();
    mount.cleanUp();
});

describe('getNumericFieldStep()', () => {
    it('should return undefined if {min, max} are not set', () => {
        expect(getNumericFieldStep('number')).toBeUndefined();
    });

    it('should return 1 for integer type where the computed step is less than 1', () => {
        expect(getNumericFieldStep('integer', 0, 10)).toBe(1);
        expect(getNumericFieldStep('integer', 0, 1)).toBe(1);
    });

    it('should compute the step based on the order of magnitude for max for any other args', () => {
        expect(getNumericFieldStep('number', 0, 1000000)).toBe(10000);
        expect(getNumericFieldStep('number', 0, 100000)).toBe(1000);
        expect(getNumericFieldStep('number', 0, 10000)).toBe(100);
        expect(getNumericFieldStep('number', 0, 1000)).toBe(10);
        expect(getNumericFieldStep('number', 0, 100)).toBe(1);
        expect(getNumericFieldStep('number', 0, 10)).toBe(0.1);
        expect(getNumericFieldStep('number', 0, 1)).toBe(0.01);
        expect(getNumericFieldStep('number', 0, 0.1)).toBe(0.001);
        expect(getNumericFieldStep('number', 0, 0.01)).toBe(0.0001);
        expect(getNumericFieldStep('number', 0, 0.001)).toBe(0.00001);
        expect(getNumericFieldStep('number', 0, 0.0001)).toBe(0.000001);
        expect(getNumericFieldStep('number', 0, 0.00001)).toBe(0.0000001);
        expect(getNumericFieldStep('number', 0, 0.000001)).toBe(0.00000001);
        expect(getNumericFieldStep('number', 0, 0.0000001)).toBe(0.000000001);
    });
});

describe('getLabel()', () => {
    it('returns schema {title} if it has it', () => {
        const key = 'test';
        const schema: JSONSchema7 = {title: 'Ron Swanson'};
        expect(getLabel(key, schema)).toBe(schema.title);
    });

    it('returns pretty name if schema does not have {title}', () => {
        const key = 'ronSwanson';
        const schema: JSONSchema7 = {};
        expect(getLabel(key, schema)).toBe('Ron swanson');
    });
});

describe('orderOfMagnitude()', () => {
    it('should compute the order of magnitude for numbers', () => {
        expect(orderOfMagnitude(1000000)).toBe(6);
        expect(orderOfMagnitude(100000)).toBe(5);
        expect(orderOfMagnitude(10000)).toBe(4);
        expect(orderOfMagnitude(1000)).toBe(3);
        expect(orderOfMagnitude(100)).toBe(2);
        expect(orderOfMagnitude(10)).toBe(1);
        expect(orderOfMagnitude(1)).toBe(0);
        expect(orderOfMagnitude(0.1)).toBe(-1);
        expect(orderOfMagnitude(0.01)).toBe(-2);
        expect(orderOfMagnitude(0.001)).toBe(-3);
        expect(orderOfMagnitude(0.0001)).toBe(-4);
        expect(orderOfMagnitude(0.00001)).toBe(-5);
        expect(orderOfMagnitude(0.000001)).toBe(-6);
    });
});
