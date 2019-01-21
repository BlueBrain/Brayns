// tslint:disable: member-ordering
import React, {Component} from 'react';

import classNames from 'classnames';
import {memoize} from 'lodash';

import {compareVectors} from '../../math';

import {PropTypes} from '@material-ui/core';
import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';
import Typography from '@material-ui/core/Typography';

import NumericField, {NumType} from './numeric-field';


const styles = (theme: Theme) => createStyles({
    root: {
        display: 'inline-flex',
        flexDirection: 'column',
        minWidth: 0
    },
    fullWidth: {
        width: '100%'
    },
    marginNormal: {
        marginTop: theme.spacing.unit * 2,
        marginBottom: theme.spacing.unit
    },
    marginDense: {
        marginTop: theme.spacing.unit,
        marginBottom: theme.spacing.unit / 2
    },
    fields: {
        display: 'flex',
        flexDirection: 'row'
    },
    gutterRight: {
        marginRight: theme.spacing.unit * 2
    }
});

const style = withStyles(styles, {withTheme: true});


class VectorField extends Component<Props> {
    createUpdate = memoize((index: number) => (value: number) => {
        const vector = [...this.props.value];
        vector[index] = value;
        this.saveChanges(vector);
    });

    saveChanges = (value: number[]) => {
        const {onChange} = this.props;
        if (onChange) {
            onChange(value);
        }
    }

    shouldComponentUpdate(nextProps: Readonly<Props>) {
        return !compareVectors(nextProps.value, this.props.value)
            || nextProps.label !== this.props.label
            || !compareVectors(nextProps.min, this.props.min)
            || !compareVectors(nextProps.max, this.props.max)
            || !compareVectors(nextProps.step, this.props.step)
            || nextProps.onChange !== this.props.onChange
            || nextProps.disabled !== this.props.disabled
            || nextProps.theme !== this.props.theme;
    }

    render() {
        const {
            classes,
            className,
            label,
            value,
            items,
            min = [],
            max = [],
            step = [],
            type,
            margin,
            fullWidth,
            disabled
        } = this.props;

        const rootClasses = {
            [classes.marginNormal]: margin === 'normal',
            [classes.marginDense]: margin === 'dense',
            [classes.fullWidth]: fullWidth
        };

        const labelNode = !!label ? (
            <Typography variant="caption" color="textSecondary">
                {label}
            </Typography>
        ) : null;

        const fields = Array(items)
            .fill(0)
            .map((_, index) => {
                const className = index < items - 1 ? classes.gutterRight : '';

                return (
                    <NumericField
                        key={index}
                        className={className}
                        value={value[index]}
                        onChange={this.createUpdate(index)}
                        min={min[index]}
                        max={max[index]}
                        step={step[index]}
                        disabled={disabled}
                        type={type}
                        fullWidth
                        noSlider
                    />
                );
            });

        return (
            <div className={classNames(classes.root, rootClasses, className)}>
                {labelNode}
                <div className={classes.fields}>
                    {fields}
                </div>
            </div>
        );
    }
}


export default style(VectorField);


type Props = VectorFieldProps
    & WithStyles<typeof styles, true>;


export interface VectorFieldProps {
    className?: string;
    label?: string;
    value: number[];
    items: number;
    type?: NumType;
    min?: number[];
    max?: number[];
    step?: number[];
    margin?: PropTypes.Margin;
    fullWidth?: boolean;
    disabled?: boolean;
    onChange?(value: number[]): void;
}
