import React, {
    ChangeEvent,
    PureComponent
} from 'react';

import {JSONSchema7} from 'json-schema';
import {capitalize, isNumber, isString} from 'lodash';

import TextField from '@material-ui/core/TextField';

import NumericField from './numeric-field';
import SelectField from './select-field';
import SwitchField from './switch-field';
import VectorField from './vector-field';


export default class SchemaField extends PureComponent<Props> {
    onNumChange = (value: number) => this.props.onChange({
        [this.props.name]: value
    })

    onVectorChange = (value: number[]) => this.props.onChange({
        [this.props.name]: value
    })

    onStrChange = (evt: ChangeEvent<HTMLInputElement>) => this.props.onChange({
        [this.props.name]: evt.target.value
    })

    onSelectChange = (evt: ChangeEvent<HTMLSelectElement>) => this.props.onChange({
        [this.props.name]: evt.target.value
    })

    onSwitchChange = (evt: ChangeEvent<HTMLInputElement>, checked: boolean) => this.props.onChange({
        [this.props.name]: checked
    })

    render() {
        const {
            name,
            schema,
            value,
            disabled
        } = this.props;

        const {type} = schema;
        const label = getLabel(name, schema);
        const v = value !== undefined ? value : schema.default;

        if (isTypeNumber(type)) {
            const min = schema.minimum;
            const max = schema.maximum;
            const fieldType = getNumericFieldType(type);
            const step = getNumericFieldStep(type, min, max);
            return (
                <NumericField
                    id={name}
                    label={label}
                    value={v}
                    onChange={this.onNumChange}
                    min={min}
                    max={max}
                    step={step}
                    type={fieldType}
                    disabled={disabled}
                    margin="normal"
                    fullWidth
                />
            );
        } else if (type === 'string') {
            const enums = schema.enum;
            if (Array.isArray(enums)) {
                const selected = !isString(v) ? '' : v; // If the select value is undefined, MUI will complain
                const options = enums.map(item => `${item}`);

                return (
                    <SelectField
                        id={name}
                        label={label}
                        options={options}
                        value={selected}
                        onChange={this.onSelectChange}
                        margin="normal"
                        disabled={disabled}
                        fullWidth
                    />
                );
            } else {
                return (
                    <TextField
                        id={name}
                        label={label}
                        value={v}
                        onChange={this.onStrChange}
                        type="text"
                        disabled={disabled}
                        margin="normal"
                        fullWidth
                    />
                );
            }
        } else if (type === 'array') {
            const items = schema.items;
            if (!Array.isArray(items) && typeof items === 'object' && isTypeNumber(items.type)) {
                const length = schema.maxItems!;
                const minimum = items.minimum;
                const min = isNumber(minimum) ? arrayWith(length, minimum) : undefined;
                const maximum = items.maximum;
                const max = isNumber(maximum) ? arrayWith(length, maximum) : undefined;
                const type = items.type;
                const fieldType = getNumericFieldType(type);
                const step = getNumericFieldStep(type, minimum, maximum);
                const steps = step ? arrayWith(length, step) : undefined;

                return (
                    <VectorField
                        items={length}
                        label={label}
                        value={v}
                        onChange={this.onVectorChange}
                        min={min}
                        max={max}
                        step={steps}
                        type={fieldType}
                        margin="normal"
                        disabled={disabled}
                    />
                );
            }
        } else if (type === 'boolean') {
            return (
                <SwitchField
                    id={name}
                    label={label}
                    value={!!v}
                    onChange={this.onSwitchChange}
                    disabled={disabled}
                    fullWidth
                />
            );
        }

        return null;
    }
}


export function getLabel(key: string, schema: JSONSchema7) {
    if (schema.title) {
        return schema.title;
    }
    return capitalize(key
        .replace(/([A-Z])/g, $1 => ` ${$1.toLowerCase()}`)
        .trim()
    );
}

function getNumericFieldType(type: 'number' | 'integer') {
    return type === 'number'
        ? 'float'
        : 'integer';
}

function isTypeNumber(type: any): type is 'number' | 'integer' {
    return type === 'number' || type === 'integer';
}

function arrayWith(length: number, value: any) {
    return Array(length)
        .fill(value);
}

export function getNumericFieldStep(type: 'number' | 'integer', min?: number, max?: number) {
    if (!isNumber(min) || !isNumber(max)) {
        return undefined;
    }

    const order = orderOfMagnitude(max);
    const factor = order - 2;
    const step = Math.pow(10, factor);

    if (type === 'integer' && step < 1) {
        return 1;
    }

    return step;
}

export function orderOfMagnitude(num: number) {
    const epsilon = 0.000000001;
    return Math.floor(Math.log(num) / Math.LN10 + epsilon);
}


interface Props {
    name: string;
    schema: JSONSchema7;
    value?: any;
    disabled?: boolean;
    onChange(change: Change): void;
}

export interface Change {
    [key: string]: any;
}
