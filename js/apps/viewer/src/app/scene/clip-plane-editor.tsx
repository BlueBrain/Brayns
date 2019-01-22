import React, {PureComponent} from 'react';

import {ClipPlane, Plane} from 'brayns';

import {NumericField, Vector3Field} from '../../common/components';

import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';


const styles = (theme: Theme) => createStyles({
    root: {
        backgroundColor: theme.palette.background.paper,
        padding: theme.spacing.unit * 2,
        display: 'flex',
        flexDirection: 'column',
        flex: 1,
        minWidth: 0
    }
});
const style = withStyles(styles);


export class ClipPlaneEditor extends PureComponent<Props, State> {
    state: State = {
        xyzLabel: ['X', 'Y', 'Z'],
        xyzStep: [0.01, 0.01, 0.01]
    };

    updateRotation = (value: number[]) => {
        const cp = this.props.value;
        const [, , , d] = cp.plane;
        this.update({
            ...cp,
            plane: [...value, d] as Plane
        });
    }

    updateDistance = (value: number) => {
        const cp = this.props.value;
        const [x, y, z] = cp.plane;
        this.update({
            ...cp,
            plane: [x, y, z, value]
        });
    }

    update = (plane: ClipPlane) => {
        const {onUpdate} = this.props;
        if (onUpdate) {
            onUpdate(plane);
        }
    }

    render() {
        const {
            classes,
            disabled,
            value
        } = this.props;
        const {xyzLabel, xyzStep} = this.state;

        const [x, y, z, d] = value.plane;
        const rotation = [x, y, z];
        const [step] = xyzStep;

        return (
            <div className={classes.root}>
                <Vector3Field
                    label={xyzLabel}
                    value={rotation}
                    step={xyzStep}
                    disabled={disabled}
                    onChange={this.updateRotation}
                />

                <NumericField
                    label="Distance"
                    value={d}
                    step={step}
                    disabled={disabled}
                    onChange={this.updateDistance}
                    margin="normal"
                    fullWidth
                />
            </div>
        );
    }
}


export default style(ClipPlaneEditor);


interface Props extends WithStyles<typeof styles> {
    value: ClipPlane;
    disabled?: boolean;
    onUpdate?(value: ClipPlane): void;
}

interface State {
    xyzLabel: string[];
    xyzStep: number[];
}
