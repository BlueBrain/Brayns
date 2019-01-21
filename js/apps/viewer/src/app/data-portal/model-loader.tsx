// tslint:disable: member-ordering
import React, {
    ChangeEvent,
    PureComponent
} from 'react';

import {memoize, startCase} from 'lodash';

import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';

import {
    Change,
    SchemaFields,
    SelectField
} from '../../common/components';

import {
    withLoaders,
    WithLoaders
} from './provider';
import {LoaderWithSchema} from './types';
import {defaultProps} from './utils';


const styles = (theme: Theme) => createStyles({});

const style = withStyles(styles);

export class ModelLoader extends PureComponent<Props> {
    setLoader = (evt: ChangeEvent<HTMLSelectElement>) => {
        const {value} = evt.target;
        this.update({
            name: value,
            properties: defaultProps(value, this.props.loaders!)
        });
    }

    updateProps = (change: Change) => {
        const {value} = this.props;
        this.update({
            properties: {
                ...value.properties,
                ...change
            }
        });
    }

    update = (change: Partial<LoaderDescriptor>) => {
        const {onChange, value} = this.props;
        if (onChange) {
            onChange({
                ...value,
                ...change
            });
        }
    }

    getLoaderNames = memoize((loaders: LoaderWithSchema[]) => loaders.map(l => l.name), namesCacheKey);
    getLoaderSchema = memoize((loaders: LoaderWithSchema[], name: string) => {
        const match = loaders.find(l => l.name === name);
        if (match) {
            return match.schema;
        }
        return;
    }, schemaCacheKey);

    render() {
        const {
            className,
            value,
            loaders
        } = this.props;
        const {name, properties} = value;

        const options = this.getLoaderNames(loaders!);
        const schema = this.getLoaderSchema(loaders!, name);
        const fields = schema ? (
            <SchemaFields
                schema={schema}
                values={properties}
                onChange={this.updateProps}
            />
        ) : null;

        return (
            <div className={className}>
                <SelectField
                    label="Model Type"
                    displayName={loaderName}
                    options={options}
                    value={name}
                    onChange={this.setLoader}
                    margin="normal"
                    error={!name.length}
                    required
                    fullWidth
                />

                {fields}
            </div>
        );
    }
}

const ModelLoaderWithLoaders = withLoaders(ModelLoader);
export default style(ModelLoaderWithLoaders);


function namesCacheKey(loaders: LoaderWithSchema[]) {
    return loaders.map(l => l.name)
        .join(',');
}

function schemaCacheKey(loaders: LoaderWithSchema[], name: string) {
    const namesKey = namesCacheKey(loaders);
    return `${namesKey}:${name}`;
}

function loaderName(name: string) {
    return startCase(name);
}


interface Props extends WithStyles<typeof styles>, WithLoaders {
    className?: string;
    value: LoaderDescriptor;
    onChange?(value: LoaderDescriptor): void;
}

export interface LoaderDescriptor {
    name: string;
    properties: object;
}
