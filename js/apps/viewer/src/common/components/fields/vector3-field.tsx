// tslint:disable: member-ordering
import React, {Component} from 'react';

import classNames from 'classnames';

import {compareVectors} from '../../math';

import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';

import NumericField from './numeric-field';


const styles = (theme: Theme) => createStyles({
    root: {
        display: 'flex',
        flexDirection: 'row'
    },
    gutterRight: {
        marginRight: theme.spacing.unit * 2
    }
});

const style = withStyles(styles, {withTheme: true});


class Vector3Field extends Component<Props> {
    updatePosition = (index: number) => (value: number) => {
        const vector = [...this.props.value];
        vector[index] = value;
        this.saveChanges(vector);
    }

    updateX = this.updatePosition(0);
    updateY = this.updatePosition(1);
    updateZ = this.updatePosition(2);

    saveChanges = (value: number[]) => {
        const {onChange} = this.props;
        if (onChange) {
            onChange(value);
        }
    }

    shouldComponentUpdate(nextProps: Readonly<Props>) {
        return !compareVectors(nextProps.value, this.props.value)
            || !compareVectors(nextProps.label, this.props.label)
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
            label = [],
            value,
            min = [],
            max = [],
            step = [],
            disabled
        } = this.props;

        const [labelX, labelY, labelZ] = label;
        const [x, y, z] = value;
        const [xMin, yMin, zMin] = min;
        const [xMax, yMax, zMax] = max;
        const [xStep, yStep, zStep] = step;

        const commonProps = {
            fullWidth: true,
            noSlider: true,
            disabled
        };

        return (
            <div className={classNames(classes.root, className)}>
                <NumericField
                    label={labelX}
                    className={classes.gutterRight}
                    value={x}
                    onChange={this.updateX}
                    min={xMin}
                    max={xMax}
                    step={xStep}
                    {...commonProps}
                />
                <NumericField
                    label={labelY}
                    className={classes.gutterRight}
                    value={y}
                    onChange={this.updateY}
                    min={yMin}
                    max={yMax}
                    step={yStep}
                    {...commonProps}
                />
                <NumericField
                    label={labelZ}
                    value={z}
                    onChange={this.updateZ}
                    min={zMin}
                    max={zMax}
                    step={zStep}
                    {...commonProps}
                />
            </div>
        );
    }
}


export default style(Vector3Field);


type Props = Vector3Props
    & WithStyles<typeof styles, true>;


export interface Vector3Props {
    className?: string;
    label?: string[];
    value: number[];
    min?: number[];
    max?: number[];
    step?: number[];
    disabled?: boolean;
    onChange?(value: number[]): void;
}
