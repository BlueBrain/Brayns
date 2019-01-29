import React, {
    ChangeEvent,
    PureComponent
} from 'react';

import classNames from 'classnames';
import {isNumber, isString} from 'lodash';

import {PropTypes} from '@material-ui/core';
import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';
import TextField from '@material-ui/core/TextField';
import Typography from '@material-ui/core/Typography';


// TODO: Import properly after https://github.com/mui-org/material-ui/issues/11736 is fixed
// tslint:disable-next-line: no-var-requires
const Slider = require('@material-ui/lab/Slider').default;


const styles = (theme: Theme) => createStyles({
    root: {
        display: 'inline-flex',
        flexDirection: 'column',
        minWidth: 0
    },
    slider: {
        // TODO: Remove when https://github.com/mui-org/material-ui/issues/13455 is fixed
        padding: 24,
        margin: -24,
        overflow: 'hidden',
        boxSizing: 'content-box'
    },
    marginNormal: {
        marginTop: theme.spacing.unit * 2,
        marginBottom: theme.spacing.unit
    },
    marginDense: {
        marginTop: theme.spacing.unit,
        marginBottom: theme.spacing.unit / 2
    },
    fullWidth: {
        width: '100%'
    },
    fields: {
        display: 'flex',
        alignItems: 'center'
    },
    input: {
        marginRight: theme.spacing.unit * 2
    }
});

const style = withStyles(styles);


class NumericField extends PureComponent<Props, State> {
    static getDerivedStateFromProps(nextProps: Readonly<Props>, state: Readonly<State>) {
        const {value} = nextProps;
        if (isNumber(value) && state.propValue !== value) {
            return {
                value,
                propValue: value
            };
        }
        return null;
    }

    state: State = {
        value: 0
    };

    updateFromSlider = (evt: ChangeEvent<HTMLInputElement>, value: number) => {
        const {min, max} = this.props;
        const num = getBoundedValue({value, min, max});
        this.saveChanges(num);
    }

    updateFromInput = (evt: ChangeEvent<HTMLInputElement>) => {
        const {value} = evt.target;
        const {type, min, max} = this.props;
        const num = parseValue({value, type, min, max});

        this.setState({
            value: num
        }, () => {
            if (isNumber(num)) {
                this.saveChanges(num);
            }
        });
    }

    saveChanges = (value: number) => {
        const {onChange} = this.props;
        if (onChange) {
            onChange(value);
        }
    }

    render() {
        const {
            classes,
            id,
            label,
            className,
            min,
            max,
            step = 1,
            noSlider,
            fullWidth,
            margin,
            disabled
        } = this.props;
        const {value = 0} = this.state;

        const inputProps = {min, max, step};

        const labelId = isString(label) ? label.toLowerCase() : undefined;
        const showLabel = isString(label);
        const labelNode = showLabel ? (
            <Typography id={labelId} variant="caption" color="textSecondary">
                {label}
            </Typography>
        ) : null;

        const showSlider = isNumber(min) && isNumber(max) && !noSlider;
        const slider = showSlider ? (
            <Slider
                className={classes.slider}
                value={getValueForSlider(value, this.props.value)}
                onChange={this.updateFromSlider}
                min={min}
                max={max}
                step={step}
                aria-labelledby={labelId}
                disabled={disabled}
            />
        ) : null;

        const rootClasses = {
            [classes.marginNormal]: margin && margin === 'normal',
            [classes.marginDense]: margin && margin === 'dense',
            [classes.fullWidth]: fullWidth
        };

        return (
            <div className={classNames(classes.root, rootClasses, className)}>
                {labelNode}
                <div className={classes.fields}>
                    <TextField
                        id={id}
                        className={classNames({[classes.input]: showSlider})}
                        value={value}
                        onChange={this.updateFromInput}
                        type="number"
                        InputProps={{inputProps}}
                        disabled={disabled}
                        fullWidth={!showSlider}
                    />
                    {slider}
                </div>
            </div>
        );
    }
}


export default style(NumericField);


function parseValue({value, type, min, max}: {
    value: string;
    type?: NumType;
    min?: number;
    max?: number;
}) {
    const num = type === 'integer'
        ? parseInt(value, 10)
        : parseFloat(value);

    if (Number.isNaN(num)) {
        return value;
    }

    return getBoundedValue({
        min,
        max,
        value: num
    });
}

function getBoundedValue({value, min, max}: {
    value: number;
    min?: number;
    max?: number;
}) {
    if (isNumber(min) && value < min) {
        return min;
    } else if (isNumber(max) && value > max) {
        return max;
    }

    return value;
}

function getValueForSlider(value: string | number, fallback?: number) {
    return isNumber(value)
        ? value
        : fallback || 0;
}


type Props = NumericFieldProps & WithStyles<typeof styles>;

export interface NumericFieldProps {
    id?: string;
    label?: string;
    value?: number;
    className?: string;
    min?: number;
    max?: number;
    step?: number;
    type?: NumType;
    noSlider?: boolean;
    fullWidth?: boolean;
    margin?: PropTypes.Margin;
    disabled?: boolean;
    onChange?(value: number): void;
}

export type NumType = 'integer' | 'float';

interface State {
    propValue?: number;
    value: number | string;
}
