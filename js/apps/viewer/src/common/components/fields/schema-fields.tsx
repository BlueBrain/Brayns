import React, {Fragment, PureComponent} from 'react';

import {JSONSchema7} from 'json-schema';
import {isObject} from 'lodash';
import SchemaField, {Change} from './schema-field';


export default class SchemaFields extends PureComponent<Props> {
    render() {
        const {schema, values, onChange, disabled} = this.props;
        const fields = renderFields({
            schema,
            values,
            disabled,
            onChange
        });

        return (
            <Fragment>
                {fields}
            </Fragment>
        );
    }
}


function renderFields({
    schema,
    values,
    disabled,
    onChange
}: RenderFieldsArgs) {
    const {properties} = schema;

    if (isObject(properties)) {
        return Object.entries(properties as JSONSchema7)
            .filter(([, schema]) => !schema.readOnly)
            .map(([key, schema]) => {
                const value = values[key];
                return (
                    <SchemaField
                        key={key}
                        name={key}
                        schema={schema}
                        value={value}
                        onChange={onChange}
                        disabled={disabled}
                    />
                );
            });
    }

    return null;
}


interface Props {
    schema: JSONSchema7;
    values: {[key: string]: any};
    disabled?: boolean;
    onChange(change: Change): void;
}

type RenderFieldsArgs = Props;
