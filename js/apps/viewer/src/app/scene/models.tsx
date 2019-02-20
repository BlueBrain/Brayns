import React, {PureComponent} from 'react';

import {BoundingBox, Model as BraynsModel} from 'brayns';
import {keys} from 'lodash';

import List from '@material-ui/core/List';

import Model from './model';
import {ModelId} from './types';


export default class Models extends PureComponent<Props> {
    focusOnModel = (id: string | number) => {
        const {onFocus} = this.props;
        const model = this.props.models.find(m => m.id === id);
        if (onFocus && model) {
            onFocus(model.bounds);
        }
    }

    render() {
        const {
            models,
            selected = [],
            disabled,
            onSelectChange,
            onShowInfo,
            onEdit,
            onVisibilityChange,
            onDelete
        } = this.props;

        const items = models.map(model => {
            const {id} = model;
            const checked = selected.indexOf(id) !== -1;
            const hasMetadata = keys(model.metadata).length > 0;
            return (
                <Model
                    id={id}
                    key={id}
                    name={model.name}
                    path={model.path}
                    checked={checked}
                    boundingBox={model.boundingBox}
                    visible={model.visible}
                    hasMetadata={hasMetadata}
                    disabled={disabled}
                    onSelectChange={onSelectChange}
                    onFocus={this.focusOnModel}
                    onShowInfo={onShowInfo}
                    onEdit={onEdit}
                    onVisibilityChange={onVisibilityChange}
                    onDelete={onDelete}
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
    models: BraynsModel[];
    selected?: ModelId[];
    disabled?: boolean;
    onSelectChange?(id: ModelId): void;
    onFocus?(box: BoundingBox): void;
    onShowInfo?(id: ModelId): void;
    onEdit?(id: ModelId): void;
    onVisibilityChange?(id: ModelId, visible: boolean): void;
    onDelete?(id: ModelId): void;
}
