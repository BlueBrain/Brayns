// tslint:disable: no-empty max-classes-per-file member-ordering
import React, {
    Component,
    ComponentType,
    createContext,
    PureComponent
} from 'react';

import {
    BoundingBox,
    Camera,
    CAMERA_PARAMS,
    GET_CAMERA,
    GET_CAMERA_PARAMS,
    SCHEMA,
    SET_CAMERA,
    SET_CAMERA_PARAMS
} from 'brayns';
import {JSONSchema7} from 'json-schema';
import {debounce} from 'lodash';
import {Subscription} from 'rxjs';
import {mergeMap} from 'rxjs/operators';

import {dispatchNotification} from '../events';
import {rotateToBoundingBox} from '../math';
import brayns, {onReady} from './client';
import {findSchemaByTitle} from './utils';


export const CameraContext = createContext<CameraContextValue>({
    params: {},
    onCameraCoordsChange: () => {},
    onCameraTypeChange: () => Promise.resolve(),
    onCameraParamsChange: () => Promise.resolve(),
    onReset: () => Promise.resolve()
});

export class CameraProvider extends Component<{}, State> {
    state: State = {
        params: {}
    };

    private subs: Subscription[] = [];

    setCameraCoords = (coords: CameraCoords) => {
        brayns.notify(SET_CAMERA, coords);
        this.syncCameraCoords(coords);
    }

    syncCameraCoords = debounce((coords: CameraCoords) => this.setState(state => ({
        camera: {
            ...state.camera!,
            ...coords
        }
    })), 250);

    changeCameraType = async (type: string) => {
        try {
            await brayns.request(SET_CAMERA, {current: type});

            const {params} = await withCameraParams();

            this.setState(state => withCameraParamsSchema({
                params,
                schema: state.schema,
                camera: {
                    ...state.camera as Camera,
                    current: type
                }
            }));
        } catch (err) {
            dispatchNotification(err);
        }
    }

    setCameraParams = async (params: object) => {
        try {
            await brayns.request(SET_CAMERA_PARAMS, params);
            this.setState(state => ({
                params: {
                    ...state.params,
                    ...params
                }
            }));
        } catch (err) {
            dispatchNotification(err);
        }
    }

    reset = (boundingBox: BoundingBox) => {
        this.setState(({camera}) => {
            if (camera) {
                const changes = rotateToBoundingBox(camera, boundingBox);
                brayns.notify(SET_CAMERA, changes);
                return {
                    camera: {
                        ...camera,
                        ...changes
                    }
                };
            }
            return null;
        });
    }

    componentDidMount() {
        this.subs.push(...[
            onReady().pipe(mergeMap(() => getCameraState()))
                .subscribe(state => this.setState(withCameraParamsSchema(state))),
            brayns.observe(SET_CAMERA)
                .pipe(mergeMap(withCameraParams))
                .subscribe(({camera, params}) => {
                    this.setState(state => {
                        const {schema} = state;
                        if (schema) {
                            return withCameraParamsSchema({params, camera, schema});
                        }

                        return {params, camera} as any;
                    });
                }),
            brayns.observe(SET_CAMERA_PARAMS)
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
        const {camera, currentSchema, params} = this.state;
        const context: CameraContextValue = {
            camera,
            params,
            schema: currentSchema,
            onCameraCoordsChange: this.setCameraCoords,
            onCameraTypeChange: this.changeCameraType,
            onCameraParamsChange: this.setCameraParams,
            onReset: this.reset
        };
        return (
            <CameraContext.Provider value={context}>
                {children}
            </CameraContext.Provider>
        );
    }
}


export function withCamera<P>(Component: ComponentType<P & WithCamera>): ComponentType<P & WithCamera> {
    return class extends PureComponent<P & WithCamera> {
        render() {
            const childFn = (context: WithCamera) => (
                <Component
                    {...this.props}
                    {...context}
                />
            );
            return (
                <CameraContext.Consumer>
                    {childFn}
                </CameraContext.Consumer>
            );
        }
    };
}


async function getCameraState(): Promise<State> {
    const [camera, schema, params] = await Promise.all([
        brayns.request(GET_CAMERA),
        brayns.request(SCHEMA, {endpoint: CAMERA_PARAMS}),
        brayns.request(GET_CAMERA_PARAMS)
    ]);
    return {camera, schema, params};
}

async function withCameraParams(camera?: Camera): Promise<Pick<State, 'camera' | 'params'>> {
    const params = await brayns.request(GET_CAMERA_PARAMS);
    return {camera, params};
}

function withCameraParamsSchema({camera, schema, ...rest}: State): State {
    if (schema && camera) {
        const currentSchema = findSchemaByTitle(schema, camera.current);
        return {...rest, schema, camera, currentSchema};
    }
    return {camera, schema, ...rest};
}


interface State extends CameraWithParams {
    currentSchema?: JSONSchema7;
}

export type WithCamera = Partial<CameraContextValue>;

export interface CameraContextValue extends CameraWithParams {
    onCameraCoordsChange: SetCameraCoordsFn;
    onCameraTypeChange: ChangeCameraTypeFn;
    onCameraParamsChange: SetCameraParamsFn;
    onReset: ResetCameraFn;
}

export type SetCameraCoordsFn = (coords: CameraCoords) => void;
export type ChangeCameraTypeFn = (type: string) => Promise<void>;
export type SetCameraParamsFn = (params: object) => Promise<void>;
export type ResetCameraFn = (bb: BoundingBox) => void;

export type CameraCoords = Pick<Camera, 'position' | 'orientation' | 'target'>;

interface CameraWithParams {
    camera?: Camera;
    params: object;
    schema?: JSONSchema7;
}
