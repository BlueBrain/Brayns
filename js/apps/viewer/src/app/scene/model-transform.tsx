// tslint:disable: member-ordering
import React, {Component} from 'react';


import {Transformation} from 'brayns';
import {Euler, Math, Quaternion} from 'three';

import {Vector3Field} from '../../common/components';
import {compareVectors} from '../../common/math';

import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';
import Typography from '@material-ui/core/Typography';


const styles = (theme: Theme) => createStyles({
    subheader: {
        marginBottom: theme.spacing.unit
    },
    fields: {
        marginTop: theme.spacing.unit * 2,
        marginBottom: theme.spacing.unit,
        display: 'inline-flex',
        flexDirection: 'column',
        minWidth: 0,
        width: '100%'
    }
});
// https://material-ui-next.com/guides/typescript/
const style = withStyles(styles, {withTheme: true});


class ModelTransform extends Component<Props, State> {
    state: State = {
        translationStep: [0.1, 0.1, 0.1],
        rotationMin: [-180, -90, -180],
        rotationMax: [180, 90, 180],
        rotationStep: [1, 1, 1],
        scaleMin: [0, 0, 0],
        scaleStep: [0.1, 0.1, 0.1]
    };

    updateTranformation = (key: keyof Transformation) => (value: number[]) => {
        const {rotation, scale, translation, rotationCenter} = this.props;
        this.update({
            rotation,
            scale,
            translation,
            rotationCenter,
            [key]: key === 'rotation'
                ? toQuaternion(toRad(value))
                : value
        });
    }

    updateTranslation = this.updateTranformation('translation');
    updateRotation = this.updateTranformation('rotation');
    updateRotationCenter = this.updateTranformation('rotationCenter');
    updateScale = this.updateTranformation('scale');

    update = (updates: Transformation) => {
        const {onChange} = this.props;
        if (onChange) {
            onChange(updates);
        }
    }

    shouldComponentUpdate(nextProps: Readonly<Props>) {
        return nextProps.disabled !== this.props.disabled
            || !compareVectors(nextProps.rotation, this.props.rotation)
            || !compareVectors(nextProps.rotationCenter, this.props.rotationCenter)
            || !compareVectors(nextProps.scale, this.props.scale)
            || !compareVectors(nextProps.translation, this.props.translation)
            || nextProps.theme !== this.props.theme;
    }

    render() {
        const {
            classes,
            rotation: quaternion,
            rotationCenter,
            scale,
            translation,
            disabled
        } = this.props;
        const {
            rotationMax,
            rotationMin,
            rotationStep,
            scaleMin,
            scaleStep,
            translationStep
        } = this.state;

        // Easier for the user to edit the rotation in deg
        const rotation = toDeg(toEuler(quaternion));

        return (
            <div>
                <div className={classes.fields}>
                    <Typography
                        className={classes.subheader}
                        variant="caption"
                        color="textSecondary"
                    >
                        Translation
                    </Typography>
                    <Vector3Field
                        value={translation}
                        onChange={this.updateTranslation}
                        step={translationStep}
                        disabled={disabled}
                    />
                </div>

                <div className={classes.fields}>
                    <Typography
                        className={classes.subheader}
                        variant="caption"
                        color="textSecondary"
                    >
                        Rotation
                    </Typography>
                    <Vector3Field
                        value={rotation}
                        onChange={this.updateRotation}
                        min={rotationMin}
                        max={rotationMax}
                        step={rotationStep}
                        disabled={disabled}
                    />
                </div>

                <div className={classes.fields}>
                    <Typography
                        className={classes.subheader}
                        variant="caption"
                        color="textSecondary"
                    >
                        Rotation Center
                    </Typography>
                    <Vector3Field
                        value={rotationCenter}
                        onChange={this.updateRotationCenter}
                        disabled={disabled}
                    />
                </div>

                <div className={classes.fields}>
                    <Typography
                        className={classes.subheader}
                        variant="caption"
                        color="textSecondary"
                    >
                        Scale
                    </Typography>
                    <Vector3Field
                        value={scale}
                        onChange={this.updateScale}
                        min={scaleMin}
                        step={scaleStep}
                        disabled={disabled}
                    />
                </div>
            </div>
        );
    }
}


export default style(ModelTransform);


function toDeg(rotation: number[]) {
    return rotation.map(rad => Math.radToDeg(rad));
}

function toRad(roation: number[]) {
    return roation.map(deg => Math.degToRad(deg));
}

/**
 * Convert a Quaternion to Euler
 * @param quaternion
 */
function toEuler(quaternion: number[]) {
    const q = new Quaternion(...quaternion);
    const euler = new Euler();
    euler.setFromQuaternion(q.normalize());
    return euler.toVector3()
        .toArray();
}

/**
 * Convert Euler to Quaternion
 * @param euler
 */
function toQuaternion(euler: number[]) {
    const e = new Euler(...euler);
    const quaternion = new Quaternion();
    quaternion.setFromEuler(e);
    return quaternion.toArray();
}


interface Props extends WithStyles<typeof styles, true>, Transformation {
    disabled?: boolean;
    onChange?(updates: Transformation): void;
}

interface State {
    translationStep: number[];
    rotationMin: number[];
    rotationMax: number[];
    rotationStep: number[];
    scaleMin: number[];
    scaleStep: number[];
}
