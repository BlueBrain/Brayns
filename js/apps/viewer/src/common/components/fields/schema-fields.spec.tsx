import React from 'react';

import {JSONSchema7} from 'json-schema';
import {noop} from 'lodash';

import {createMount, createShallow} from '@material-ui/core/test-utils';

import SchemaField from './schema-field';
import SchemaFields from './schema-fields';


it('renders without crashing', () => {
    const shallow = createShallow();
    const schema: JSONSchema7 = {};
    const values = {};
    const component = shallow(
        <SchemaFields
            schema={schema}
            values={values}
            onChange={noop}
        />
    );
    component.unmount();
});

it('renders a <SchemaField> for every property of schema {properties}', () => {
    const mount = createMount();

    const schema: JSONSchema7 = {
        properties: {
            ron: {type: 'boolean'},
            leslieKnope: {type: 'boolean'}
        }
    };
    const values = {
        ron: true,
        leslieKnope: true
    };

    const component = mount(
        <SchemaFields
            schema={schema}
            values={values}
            onChange={noop}
        />
    );

    const fields = component.find(SchemaField);
    expect(fields.length).toBe(2);

    const ronField = fields.at(0);
    expect(ronField.prop('name')).toBe('ron');
    expect(ronField.prop('schema')).toEqual(schema.properties!.ron);
    expect(ronField.prop('value')).toBe(values.ron);

    const knopeField = fields.at(1);
    expect(knopeField.prop('name')).toBe('leslieKnope');
    expect(knopeField.prop('schema')).toEqual(schema.properties!.leslieKnope);
    expect(knopeField.prop('value')).toBe(values.leslieKnope);

    component.unmount();
    mount.cleanUp();
});

it('keeps {readOnly} properties', () => {
    const mount = createMount();

    const schema: JSONSchema7 = {
        properties: {
            ron: {type: 'boolean'},
            leslieKnope: {
                type: 'boolean',
                readOnly: true
            }
        }
    };
    const values = {
        ron: true
    };

    const component = mount(
        <SchemaFields
            schema={schema}
            values={values}
            onChange={noop}
        />
    );

    const fields = component.find(SchemaField);
    expect(fields.length).toBe(1);

    const ronField = fields.at(0);
    expect(ronField.prop('name')).toBe('ron');
    expect(ronField.prop('schema')).toEqual(schema.properties!.ron);
    expect(ronField.prop('value')).toBe(values.ron);

    component.unmount();
    mount.cleanUp();
});

it('updates the <SchemaField> value when the value updates', () => {
    const mount = createMount();

    const schema: JSONSchema7 = {
        properties: {
            ron: {type: 'boolean'}
        }
    };
    const values = {
        ron: false
    };

    const component = mount(
        <SchemaFields
            schema={schema}
            values={values}
            onChange={noop}
        />
    );

    expect(component.find(SchemaField)
        .prop('value')).toBe(false);

    component.setProps({
        values: {ron: true}
    });

    expect(component.find(SchemaField)
        .prop('value')).toBe(true);

    component.unmount();
    mount.cleanUp();
});

it('runs {onChange} if the <SchemaField> value changes', () => {
    const mount = createMount();

    const onChangeSpy = jest.fn();
    const schema: JSONSchema7 = {
        properties: {
            ron: {type: 'boolean'}
        }
    };
    const values = {
        ron: false
    };

    const component = mount(
        <SchemaFields
            schema={schema}
            values={values}
            onChange={onChangeSpy}
        />
    );

    const field = component.find(SchemaField);
    field.props()
        .onChange({
            ron: true
        });
    expect(onChangeSpy).toHaveBeenCalledWith({
        ron: true
    });

    component.unmount();
    mount.cleanUp();
});
