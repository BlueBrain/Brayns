import React, {PureComponent} from 'react';

import {ClipPlane as ClipPlaneType} from 'brayns';

import List from '@material-ui/core/List';

import ClipPlane from './clip-plane';


export default class ClipPlanes extends PureComponent<Props> {
    render() {
        const {
            clipPlanes,
            selected: selection = [],
            disabled,
            onEdit,
            onSelectChange
        } = this.props;

        const items = clipPlanes.map(cp => {
            const selected = selection.indexOf(cp) !== -1;
            return (
                <ClipPlane
                    key={cp.id}
                    value={cp}
                    selected={selected}
                    disabled={disabled}
                    onEdit={onEdit}
                    onSelectChange={onSelectChange}
                />
            );
        });

        return (
            <List>
                {items}
            </List>
        );
    }
}


interface Props {
    clipPlanes: ClipPlaneType[];
    selected?: ClipPlaneType[];
    disabled?: boolean;
    onEdit?(cp: ClipPlaneType): void;
    onSelectChange?(cp: ClipPlaneType): void;
}
