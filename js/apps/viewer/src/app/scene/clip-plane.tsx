import React, {PureComponent} from 'react';

import {ClipPlane as ClipPlaneType} from 'brayns';

import Checkbox from '@material-ui/core/Checkbox';
import IconButton from '@material-ui/core/IconButton';
import ListItem from '@material-ui/core/ListItem';
import ListItemSecondaryAction from '@material-ui/core/ListItemSecondaryAction';
import ListItemText from '@material-ui/core/ListItemText';
import ControlCameraIcon from '@material-ui/icons/ControlCamera';


const FLOAT_PRECISION = 2;

export default class ClipPlane extends PureComponent<Props> {
    edit = () => {
        const {onEdit, value} = this.props;
        if (onEdit) {
            onEdit(value);
        }
    }

    toggleChecked = () => {
        const {onSelectChange, value} = this.props;
        if (onSelectChange) {
            onSelectChange(value);
        }
    }

    render() {
        const {
            value,
            selected,
            disabled
        } = this.props;

        const [x, y, z, d] = value.plane;
        const rotation = planeRotationStr([x, y, z]);
        const distance = (<span>D: {toPrecision(d)}</span>);

        return (
            <ListItem
                button
                onClick={this.toggleChecked}
                disabled={disabled}
            >
                <Checkbox
                    onChange={this.toggleChecked}
                    checked={selected}
                    tabIndex={-1}
                    disableRipple
                />
                <ListItemText primary={rotation} secondary={distance} />
                <ListItemSecondaryAction>
                    <IconButton
                        onClick={this.edit}
                        disabled={disabled}
                        aria-label="Move clip plane"
                    >
                        <ControlCameraIcon />
                    </IconButton>
                </ListItemSecondaryAction>
            </ListItem>
        );
    }
}


function planeRotationStr(rotation: number[]) {
    const [x, y, z] = rotation.map(toPrecision);
    return `[X: ${x}, Y: ${y}, Z: ${z}]`;
}

function toPrecision(num: number) {
    return num.toPrecision(FLOAT_PRECISION);
}


interface Props {
    value: ClipPlaneType;
    selected?: boolean;
    disabled?: boolean;
    onEdit?(cp: ClipPlaneType): void;
    onSelectChange?(cp: ClipPlaneType): void;
}
