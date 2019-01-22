import React, {
    ChangeEvent,
    PureComponent
} from 'react';

import FormControl, {FormControlProps} from '@material-ui/core/FormControl';
import FormControlLabel from '@material-ui/core/FormControlLabel';
import Switch from '@material-ui/core/Switch';

import {FormControlPropsKeys} from './types';

export default class SwitchField extends PureComponent<Props> {
    render() {
        const {
            id,
            label,
            value,
            onChange,
            ...formControlProps
        } = this.props;

        const switchControl = (
            <Switch
                checked={value}
                onChange={onChange}
                aria-label={label}
            />
        );
        return (
            <FormControl {...formControlProps}>
                <FormControlLabel
                    id={id}
                    label={label}
                    control={switchControl}
                />
            </FormControl>
        );
    }
}

interface Props extends Pick<FormControlProps, FormControlPropsKeys> {
    id?: string;
    label?: string;
    value?: boolean;
    onChange?(evt: ChangeEvent<HTMLInputElement>, checked: boolean): void;
}
