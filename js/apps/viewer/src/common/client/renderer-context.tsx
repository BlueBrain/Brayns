// tslint:disable: no-empty max-classes-per-file member-ordering
import React, {
    Component,
    ComponentType,
    createContext,
    PureComponent
} from 'react';

import {
    EnvironmentMap,
    GET_ENVIRONMENT_MAP,
    GET_RENDERER,
    GET_RENDERER_PARAMS,
    Renderer,
    RENDERER_PARAMS,
    SCHEMA,
    SET_ENVIRONMENT_MAP,
    SET_RENDERER,
    SET_RENDERER_PARAMS
} from 'brayns';
import {JSONSchema7} from 'json-schema';
import {debounce} from 'lodash';
import {Subscription} from 'rxjs';
import {mergeMap} from 'rxjs/operators';

import {dispatchNotification} from '../events';
import brayns, {onReady} from './client';
import {findSchemaByTitle} from './utils';


export const RendererContext = createContext<RendererContextValue>({
    params: {},
    onRendererPropsChange: () => {},
    onRendererTypeChange: () => Promise.resolve(),
    onRendererParamsChange: () => Promise.resolve(),
    onEnvMapChange: () => Promise.resolve()
});

export class RendererProvider extends Component<{}, State> {
    state: State = {
        params: {}
    };

    private subs: Subscription[] = [];

    changeRendererType = async (type: string) => {
        try {
            await brayns.request(SET_RENDERER, {current: type});

            const {params} = await withRendererParams();

            this.setState(state => withRendererParamsSchema({
                params,
                schema: state.schema,
                renderer: {
                    ...state.renderer as Renderer,
                    current: type
                }
            }));
        } catch (err) {
            dispatchNotification(err);
        }
    }

    changeRendererProps = (props: Partial<RendererProps>) => {
        this.syncRendererProps(props);
        this.setState(state => ({
            renderer: {
                ...state.renderer as Renderer,
                ...props
            }
        }));
    }
    syncRendererProps = debounce(async (props: Partial<RendererProps>) => {
        try {
            await brayns.request(SET_RENDERER, props);
        } catch (err) {
            dispatchNotification(err);
        }
    }, 250);

    setRendererParams = async (params: object) => {
        this.syncRendererParams(params);
        this.setState(state => ({
            params: {
                ...state.params,
                ...params
            }
        }));
    }
    syncRendererParams = debounce(async (params: object) => {
        try {
            await brayns.request(SET_RENDERER_PARAMS, params);
        } catch (err) {
            dispatchNotification(err);
        }
    }, 250);

    setEnvMap = async (filename: string) => {
        this.syncEnvMap(filename);
        this.setState({
            envMap: {filename}
        });
    }
    syncEnvMap = debounce(async (filename: string) => {
        try {
            const isEnvMapValid = await brayns.request(SET_ENVIRONMENT_MAP, {filename});
            this.setState({isEnvMapValid});
        } catch (err) {
            dispatchNotification(err);
        }
    }, 250);

    componentDidMount() {
        this.subs.push(...[
            onReady().pipe(mergeMap(() => getRendererState()))
                .subscribe(state => this.setState(withRendererParamsSchema(state))),
            brayns.observe(SET_ENVIRONMENT_MAP)
                .subscribe(envMap => this.setState({envMap})),
            brayns.observe(SET_RENDERER)
                .pipe(mergeMap(withRendererParams))
                .subscribe(({renderer, params}) => {
                    this.setState(state => {
                        const {schema} = state;
                        if (schema) {
                            return withRendererParamsSchema({params, renderer, schema});
                        }

                        return {params, renderer} as any;
                    });
                }),
            brayns.observe(SET_RENDERER_PARAMS)
                .subscribe(params => {
                    this.setState({params});
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
        const {renderer, currentSchema, params, envMap, isEnvMapValid = true} = this.state;
        const context: RendererContextValue = {
            renderer,
            params,
            envMap,
            isEnvMapValid,
            schema: currentSchema,
            onRendererPropsChange: this.changeRendererProps,
            onRendererTypeChange: this.changeRendererType,
            onRendererParamsChange: this.setRendererParams,
            onEnvMapChange: this.setEnvMap
        };
        return (
            <RendererContext.Provider value={context}>
                {children}
            </RendererContext.Provider>
        );
    }
}


export function withRenderer<P>(Component: ComponentType<P & WithRenderer>): ComponentType<P & WithRenderer> {
    return class extends PureComponent<P & WithRenderer> {
        render() {
            const childFn = (context: WithRenderer) => (
                <Component
                    {...this.props}
                    {...context}
                />
            );
            return (
                <RendererContext.Consumer>
                    {childFn}
                </RendererContext.Consumer>
            );
        }
    };
}


async function getRendererState(): Promise<State> {
    const [renderer, schema, params, envMap] = await Promise.all([
        brayns.request(GET_RENDERER),
        brayns.request(SCHEMA, {endpoint: RENDERER_PARAMS}),
        brayns.request(GET_RENDERER_PARAMS),
        brayns.request(GET_ENVIRONMENT_MAP)
    ]);
    return {renderer, schema, params, envMap};
}

async function withRendererParams(renderer?: Renderer): Promise<Pick<State, 'renderer' | 'params'>> {
    const params = await brayns.request(GET_RENDERER_PARAMS);
    return {renderer, params};
}

function withRendererParamsSchema({renderer, schema, ...rest}: State): State {
    if (schema && renderer) {
        const currentSchema = findSchemaByTitle(schema, renderer.current);
        return {...rest, schema, renderer, currentSchema};
    }
    return {renderer, schema, ...rest};
}


interface State extends RendererWithParams {
    currentSchema?: JSONSchema7;
}

export type WithRenderer = Partial<RendererContextValue>;

export interface RendererContextValue extends RendererWithParams {
    onRendererTypeChange: ChangeRendererTypeFn;
    onRendererPropsChange: ChangeRendererPropsFn;
    onRendererParamsChange: SetRendererParamsFn;
    onEnvMapChange: SetEnvMapFn;
}

export type ChangeRendererPropsFn = (props: Partial<RendererProps>) => void;
export type ChangeRendererTypeFn = (type: string) => Promise<void>;
export type SetRendererParamsFn = (params: object) => Promise<void>;
export type SetEnvMapFn = (filename: string) => Promise<void>;

export type RendererProps = Pick<Renderer, 'backgroundColor'
    | 'headLight'
    | 'maxAccumFrames'
    | 'samplesPerPixel'
    | 'subsampling'
    | 'varianceThreshold'>;

interface RendererWithParams {
    renderer?: Renderer;
    params?: object;
    schema?: JSONSchema7;
    envMap?: EnvironmentMap;
    isEnvMapValid?: boolean;
}
