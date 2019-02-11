// tslint:disable: max-classes-per-file member-ordering
import React, {
    Component,
    ComponentType,
    createContext,
    PureComponent
} from 'react';

import {
    GET_LOADERS,
    Loader,
    LOADERS_SCHEMA
} from 'brayns';
import {JSONSchema7} from 'json-schema';
import {Subscription} from 'rxjs';
import {mergeMap} from 'rxjs/operators';

import brayns, {onReady} from './client';


export const LoadersContext = createContext<State>({
    loaders: []
});


export class LoadersProvider extends Component<{}, State> {
    state: State = {
        loaders: []
    };

    private subs: Subscription[] = [];

    componentDidMount() {
        this.subs.push(...[
            onReady()
                .pipe(mergeMap(() => getLoadersWithSchema()))
                .subscribe(loaders => {
                    this.setState({loaders});
                })
        ]);
    }

    componentWillUnmount() {
        while (this.subs.length) {
            const sub = this.subs.pop();
            sub!.unsubscribe();
        }
    }

    render() {
        const {children} = this.props;
        const {loaders} = this.state;
        const context = {loaders};
        return (
            <LoadersContext.Provider value={context}>
                {children}
            </LoadersContext.Provider>
        );
    }
}


export function withLoaders<P>(Component: ComponentType<P & WithLoaders>): ComponentType<P & WithLoaders> {
    return class extends PureComponent<P & WithLoaders> {
        render() {
            const childFn = (context: WithLoaders) => (
                <Component
                    {...this.props}
                    {...context}
                />
            );
            return (
                <LoadersContext.Consumer>
                    {childFn}
                </LoadersContext.Consumer>
            );
        }
    };
}


async function getLoadersWithSchema(): Promise<LoaderWithSchema[]> {
    const loaders = await getLoaders();
    const schema = await brayns.request(LOADERS_SCHEMA);
    const schemas = schema.oneOf!;
    return loaders.map(loader => ({
        ...loader,
        schema: schemas.find(s => (s as JSONSchema7).title === loader.name) as JSONSchema7
    }));
}

async function getLoaders() {
    const items = await brayns.request(GET_LOADERS);
    return items.sort((a, b) => {
        const nameA = a.name;
        const nameB = b.name;
        if (nameA > nameB) {
            return 1;
        }
        if (nameA < nameB) {
            return -1;
        }
        return 0;
    });
}


export type WithLoaders = Partial<State>;

export interface LoaderWithSchema extends Loader {
    schema: JSONSchema7;
}

interface State {
    loaders?: LoaderWithSchema[];
}
