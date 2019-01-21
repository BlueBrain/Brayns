import React, {
    ComponentType,
    createContext
} from 'react';

import {Loader} from 'brayns';
import {JSONSchema7} from 'json-schema';
import {LoaderWithSchema} from './types';

export const {
    Provider,
    Consumer
} = createContext<LoadersContext>({
    loaders: []
});

export function withLoaders<P>(Component: ComponentType<P>): ComponentType<P & WithLoaders> {
    return (props: P) => (
        <Consumer children={render(props)} />
    );

    function render(props: P) {
        return (context: LoadersContext) => (
            <Component
                {...props}
                {...context}
            />
        );
    }
}

export function createLoadersContext(loaders: Loader[], schemas: JSONSchema7[]): LoadersContext {
    return {
        loaders: loaders.map(loader => ({
            ...loader,
            schema: schemas.find(s => s.title === loader.name)!
        }))
    };
}


export type WithLoaders = LoadersContext;

export interface LoadersContext {
    loaders?: LoaderWithSchema[];
}
