// tslint:disable: member-ordering
import React, {PureComponent} from 'react';

import {ClipPlane as ClipPlaneType} from 'brayns';

import Checkbox from '@material-ui/core/Checkbox';
import ListItem from '@material-ui/core/ListItem';
import ListItemText from '@material-ui/core/ListItemText';

import {composeEvtHandler} from './utils';


const FLOAT_PRECISION = 2;

export default class ClipPlane extends PureComponent<Props, State> {
    state: State = {
        disableRipple: false
    };

    enableRipple = () => this.setState({disableRipple: false});
    disableRipple = () => this.setState({disableRipple: true});

    edit = () => {
        const {onEdit, value} = this.props;
        if (onEdit) {
            onEdit(value);
        }
    }

    toggleChecked = composeEvtHandler(() => {
        const {onSelectChange, value} = this.props;
        if (onSelectChange) {
            onSelectChange(value);
        }
    });

    render() {
        const {
            value,
            selected,
            disabled
        } = this.props;
        const {disableRipple} = this.state;

        const [x, y, z, d] = value.plane;
        const rotation = planeRotationStr([x, y, z]);
        const distance = (<span>D: {toPrecision(d)}</span>);

        return (
            <ListItem
                onClick={this.edit}
                disabled={disabled}
                disableRipple={disableRipple}
                disableTouchRipple={disableRipple}
                button
            >
                <Checkbox
                    onMouseOver={this.disableRipple}
                    onMouseOut={this.enableRipple}
                    onClick={this.toggleChecked}
                    checked={selected}
                />
                <ListItemText
                    primary={rotation}
                    secondary={distance}
                />
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

interface State {
    disableRipple?: boolean;
}
