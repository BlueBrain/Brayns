// tslint:disable: member-ordering
import React, {ChangeEvent, PureComponent} from 'react';

import {BoundingBox} from 'brayns';
import {Vector3} from 'three';

import {VectorSquareIcon} from '../../common/components';

import List from '@material-ui/core/List';
import ListItem from '@material-ui/core/ListItem';
import ListItemIcon from '@material-ui/core/ListItemIcon';
import ListItemSecondaryAction from '@material-ui/core/ListItemSecondaryAction';
import ListItemText from '@material-ui/core/ListItemText';
import Switch from '@material-ui/core/Switch';
import VisibilityIcon from '@material-ui/icons/Visibility';

import {ModelInfo, ModelSettings as ModelProps} from './types';


export default class ModelSettings extends PureComponent<Props> {
    updateFromSwitch = (key: keyof ModelProps) => (evt: ChangeEvent<HTMLInputElement>, checked: boolean) => {
        this.update({
            [key]: checked
        });
    }

    toggleVisibility = this.updateFromSwitch('visible');
    toggleBoundingBox = this.updateFromSwitch('boundingBox');

    update = (updates: Partial<ModelProps>) => {
        const {onChange} = this.props;
        if (onChange) {
            onChange(updates);
        }
    }

    render() {
        const {
            className,
            bounds,
            boundingBox = false,
            visible = false,
            disabled
        } = this.props;

        const size = getSize(bounds);

        // TODO: If we need even better perf, make a comp. for each list item
        return (
            <List className={className}>
                <ListItem>
                    <ListItemIcon>
                        <VisibilityIcon />
                    </ListItemIcon>
                    <ListItemText primary="Visible" />
                    <ListItemSecondaryAction>
                        <Switch
                            checked={visible}
                            onChange={this.toggleVisibility}
                            aria-label="Toggle visibility"
                            disabled={disabled}
                        />
                    </ListItemSecondaryAction>
                </ListItem>
                <ListItem>
                    <ListItemIcon>
                        <VectorSquareIcon />
                    </ListItemIcon>
                    <ListItemText primary="Bounding box" secondary={size} />
                    <ListItemSecondaryAction>
                        <Switch
                            checked={boundingBox}
                            onChange={this.toggleBoundingBox}
                            aria-label="Toggle bounding box"
                            disabled={disabled}
                        />
                    </ListItemSecondaryAction>
                </ListItem>
            </List>
        );
    }
}


function getSize(bounds?: BoundingBox) {
    if (bounds) {
        const min = new Vector3(...bounds.min);
        const max = new Vector3(...bounds.max);
        const sizeStr = max.sub(min)
            .toArray()
            .map(v => v.toFixed(4))
            .join(', ');
        return `[${sizeStr}]`;
    }
    return null;
}


interface Props extends Partial<ModelProps>,
    Partial<Pick<ModelInfo, 'bounds'>> {
    className?: string;
    disabled?: boolean;
    onChange?(change: Partial<ModelProps>): void;
}
