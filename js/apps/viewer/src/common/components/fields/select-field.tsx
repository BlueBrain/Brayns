import React, {
    ChangeEvent,
    PureComponent
} from 'react';

import FormControl, {FormControlProps} from '@material-ui/core/FormControl';
import FormHelperText from '@material-ui/core/FormHelperText';
import Input from '@material-ui/core/Input';
import InputLabel from '@material-ui/core/InputLabel';
import MenuItem from '@material-ui/core/MenuItem';
import Select from '@material-ui/core/Select';

import {FormControlPropsKeys} from './types';


export default class SelectField extends PureComponent<SelectFieldProps> {
    render() {
        const {
            id,
            className,
            label,
            helperText,
            options = [],
            value,
            onChange,
            children,
            displayName,
            shrink,
            ...formControlProps
        } = this.props;

        const optionItems = options.map(item => {
            const option = `${item}`;
            const name = displayName ? displayName(item) : item;
            return (
                <MenuItem
                    key={option}
                    value={option}
                >
                    {name}
                </MenuItem>
            );
        });

        const inputLabelProps = shrink ? {shrink: true} : {};
        const helper = helperText && helperText.length > 0 ? <FormHelperText>{helperText}</FormHelperText> : null;

        return (
            <FormControl
                {...formControlProps}
                className={className}
            >
                <InputLabel htmlFor={id} {...inputLabelProps}>
                    {label}
                </InputLabel>
                <Select
                    value={value}
                    onChange={onChange}
                    input={<Input id={id} />}
                    displayEmpty
                >
                    {children}
                    {optionItems}
                </Select>
                {helper}
            </FormControl>
        );
    }
}

export interface SelectFieldProps extends Pick<FormControlProps, FormControlPropsKeys> {
    id?: string;
    className?: string;
    label?: string;
    helperText?: string;
    options?: string[];
    value?: string;
    shrink?: boolean;
    onChange?(evt: ChangeEvent<HTMLSelectElement>): void;
    displayName?(value: string): string;
}
