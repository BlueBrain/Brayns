// tslint:disable: member-ordering
import uid from 'crypto-uid';
import {JSONSchema7} from 'json-schema';
import {
    isString,
    noop,
    pick
} from 'lodash';
import {
    Client as RocketsClient,
    Notification,
    Request,
    RequestTask
} from 'rockets-client';
import {
    BehaviorSubject,
    Observable,
    ReplaySubject
} from 'rxjs';
import {toArrayBuffer} from 'rxjs-file';
import {filter, map} from 'rxjs/operators';
import {
    ADD_CLIP_PLANE_TYPE,
    ANIMATION_PARAMS_TYPE,
    APP_PARAMS_TYPE,
    CAMERA_TYPE,
    CHUNK,
    GET_ANIMATION_PARAMS_TYPE,
    GET_APP_PARAMS_TYPE,
    GET_CAMERA_PARMS_TYPE,
    GET_CAMERA_TYPE,
    GET_CLIP_PLANES_TYPE,
    GET_ENVIRONMENT_MAP_TYPE,
    GET_LOADERS_TYPE,
    GET_MODEL_PROPERTIES_TYPE,
    GET_MODEL_TRANSFER_FUNCTION_TYPE,
    GET_RENDERER_PARMS_TYPE,
    GET_RENDERER_TYPE,
    GET_SCENE_TYPE,
    GET_STATISTICS_TYPE,
    GET_VERSION_TYPE,
    IMAGE_JPEG,
    IMAGE_JPEG_TYPE,
    INSPECT_TYPE,
    LOAD_MODEL_TYPE,
    LOADERS_SCHEMA_TYPE,
    MODEL_PROPERTIES_SCHEMA_TYPE,
    QUIT_TYPE,
    REMOVE_CLIP_PLANES_TYPE,
    REMOVE_MODEL_TYPE,
    RENDERER_PARMS_TYPE,
    RENDERER_TYPE,
    SCENE_TYPE,
    SCHEMA_TYPE,
    SET_ANIMATION_PARAMS_TYPE,
    SET_APP_PARAMS_TYPE,
    SET_CAMERA_PARMS_TYPE,
    SET_CAMERA_TYPE,
    SET_ENVIRONMENT_MAP_TYPE,
    SET_MODEL_PROPERTIES_TYPE,
    SET_MODEL_TRANSFER_FUNCTION_TYPE,
    SET_RENDERER_PARMS_TYPE,
    SET_RENDERER_TYPE,
    SET_SCENE_TYPE,
    SET_STATISTICS_TYPE,
    SNAPSHOT_TYPE,
    STATISTICS_TYPE,
    UPDATE_CLIP_PLANE_TYPE,
    UPDATE_MODEL_TYPE,
    UPLOAD_MODEL,
    UPLOAD_MODEL_TYPE,
    VERSION_TYPE
} from './constants';
import {
    AnimationParameters,
    ApplicationParameters,
    BinaryParams,
    Camera,
    CameraParams,
    ClipPlane,
    EnvironmentMap,
    GetModelPropsParams,
    GetModelPropsSchemaParams,
    InspectCoords,
    InspectParams,
    Loader,
    Model,
    ModelPropsParams,
    ModelUpdateParams,
    PathParams,
    Plane,
    Renderer,
    RendererParams,
    Scene,
    SchemaParams,
    SetAnimationParameters,
    SetTransferFunctionParams,
    Snapshot,
    SnapshotParams,
    Statistics,
    TransferFunction,
    TransferFunctionParams,
    UploadParams,
    UpstreamSetTransferFunctionParams,
    Version
} from './types';
import {
    getFileDesc,
    timeout,
    toCamelCase,
    toSnakeCase
} from './utils';


export const BRAYNS_WS_PATH = 'ws';


/**
 * JavaScript/TypeScript client for Brayns
 * https://github.com/BlueBrain/Brayns
 *
 * Upon init, the client will try to establish a connection with the Brayns server.
 * If it fails, it will retry every 5s.
 *
 * NOTE: Params for requests and outgoing notifications will be converted to snakecase,
 * and params for incoming notifications will be converted to camelcase.
 *
 * @example
 * import {
 *     CANCEL,
 *     Client,
 *     GET_APP_PARAMS,
 *     SET_APP_PARAMS
 * } from 'brayns';
 * import {take} from 'rxjs/operators';
 *
 * // Create and connect
 * const brayns = new Client('myhost.io');
 *
 * // Listen to notifications
 * brayns.observe(SET_APP_PARAMS)
 *     .subscribe(params => {
 *         console.log(params);
 *     });
 *
 * // Send a notification
 * brayns.notify(CANCEL, {id: 1});
 *
 * // Make a request
 * try {
 *     const params = await brayns.request(GET_APP_PARAMS);
 *     console.log(params);
 * } catch (err) {
 *     console.error(err);
 * }
 *
 * // Upload some model
 * try {
 *     await brayns.upload({file});
 * } catch (err) {
 *     console.error(err);
 * }
 */
export class Client {
    private binary = new ReplaySubject<Blob>(1);
    private notifications = new ReplaySubject<Notification<any>>(1);

    private connection = new BehaviorSubject<boolean>(false);
    ready = this.connection.asObservable();

    private rockets?: RocketsClient;

    /**
     * Create a client and setup a connection with the Brayns server
     * @param host
     */
    constructor(host: string) {
        const url = appendWsPath(host);
        this.connect(url);
    }

    /**
     * Send an RPC notification
     * @param method
     * @param params
     */
    notify(method: SET_ANIMATION_PARAMS_TYPE, params: Partial<AnimationParameters>): void;
    notify(method: SET_APP_PARAMS_TYPE, params: Partial<ApplicationParameters>): void;
    notify(method: SET_CAMERA_TYPE, params: Partial<Camera>): void;
    notify(method: SET_RENDERER_TYPE, params: Partial<RendererParams>): void;
    notify(method: SET_CAMERA_PARMS_TYPE | SET_RENDERER_PARMS_TYPE, params: object): void;
    notify(method: SET_SCENE_TYPE, params: Partial<Scene>): void;
    notify(method: SET_MODEL_PROPERTIES_TYPE, params: ModelPropsParams): void;
    notify(method: SET_MODEL_TRANSFER_FUNCTION_TYPE, params: SetTransferFunctionParams): void;
    notify(method: QUIT_TYPE): void;
    notify<P>(method: NotificationType | string, params?: P) {
        this.rockets!.notify(method, params);
    }

    /**
     * Make an RPC request
     * @param method
     */
    request(method: GET_ANIMATION_PARAMS_TYPE): RequestTask<undefined, AnimationParameters>;
    request(method: SET_ANIMATION_PARAMS_TYPE, params: SetAnimationParameters): RequestTask<SetAnimationParameters, boolean>;
    request(method: GET_APP_PARAMS_TYPE): RequestTask<undefined, ApplicationParameters>;
    request(method: SET_CAMERA_TYPE, params: CameraParams): RequestTask<CameraParams, boolean>;
    request(method: GET_CAMERA_TYPE): RequestTask<undefined, Camera>;
    request(method: GET_RENDERER_TYPE): RequestTask<undefined, Renderer>;
    request(method: SET_RENDERER_TYPE, params: RendererParams): RequestTask<RendererParams, boolean>;
    request(method: GET_CAMERA_PARMS_TYPE | GET_RENDERER_PARMS_TYPE): RequestTask<undefined, object>;
    request(method: SET_CAMERA_PARMS_TYPE | SET_RENDERER_PARMS_TYPE, params: object): RequestTask<object, boolean>;
    request(method: GET_STATISTICS_TYPE): RequestTask<undefined, Statistics>;
    request(method: GET_VERSION_TYPE): RequestTask<undefined, Version>;
    request(method: INSPECT_TYPE, params: InspectParams): RequestTask<InspectParams, InspectCoords>;
    request(method: GET_SCENE_TYPE): RequestTask<undefined, Scene>;
    request(method: SET_SCENE_TYPE, params: Partial<Scene>): RequestTask<Partial<Scene>, boolean>;
    request(method: LOAD_MODEL_TYPE, params: PathParams): RequestTask<PathParams, Model>;
    request(method: REMOVE_MODEL_TYPE, params: Array<string | number>): RequestTask<Array<string | number>, boolean>;
    request(method: UPDATE_MODEL_TYPE, params: ModelUpdateParams): RequestTask<ModelUpdateParams, boolean>;
    request(method: GET_MODEL_PROPERTIES_TYPE, params: GetModelPropsParams): RequestTask<GetModelPropsParams, object>;
    request(method: SET_MODEL_PROPERTIES_TYPE, params: ModelPropsParams): RequestTask<ModelPropsParams, boolean>;
    request(method: MODEL_PROPERTIES_SCHEMA_TYPE, params: GetModelPropsSchemaParams): RequestTask<GetModelPropsSchemaParams, JSONSchema7>;
    request(method: GET_MODEL_TRANSFER_FUNCTION_TYPE, params: TransferFunctionParams): RequestTask<TransferFunctionParams, TransferFunction>;
    request(method: SET_MODEL_TRANSFER_FUNCTION_TYPE, params: SetTransferFunctionParams): RequestTask<SetTransferFunctionParams, boolean>;
    request(method: GET_LOADERS_TYPE): RequestTask<undefined, Loader[]>;
    request(method: LOADERS_SCHEMA_TYPE): RequestTask<undefined, JSONSchema7>;
    request(method: ADD_CLIP_PLANE_TYPE, params: Plane): RequestTask<Plane, ClipPlane>;
    request(method: UPDATE_CLIP_PLANE_TYPE, params: ClipPlane): RequestTask<ClipPlane, boolean>;
    request(method: GET_CLIP_PLANES_TYPE): RequestTask<undefined, ClipPlane[]>;
    request(method: REMOVE_CLIP_PLANES_TYPE, params: number[]): RequestTask<number[], boolean>;
    request(method: SNAPSHOT_TYPE, params: SnapshotParams): RequestTask<SnapshotParams, Snapshot>;
    request(method: SCHEMA_TYPE, params: SchemaParams): RequestTask<SchemaParams, JSONSchema7>;
    request(method: SET_ENVIRONMENT_MAP_TYPE, params: EnvironmentMap): RequestTask<EnvironmentMap, boolean>;
    request(method: GET_ENVIRONMENT_MAP_TYPE): RequestTask<undefined, EnvironmentMap>;
    request<R, P>(method: RequestType | string, params?: P): RequestTask<P, R> {
        return this.rockets!.request<P, R>(method, params);
    }

    /**
     * Observe notifications broadcasted by the Brayns server
     * @param method
     */
    observe(method: SET_ANIMATION_PARAMS_TYPE): Observable<AnimationParameters>;
    observe(method: SET_APP_PARAMS_TYPE): Observable<ApplicationParameters>;
    observe(method: SET_CAMERA_TYPE): Observable<Camera>;
    observe(method: SET_CAMERA_PARMS_TYPE | SET_RENDERER_PARMS_TYPE): Observable<object>;
    observe(method: SET_RENDERER_TYPE): Observable<Renderer>;
    observe(method: SET_SCENE_TYPE): Observable<Scene>;
    observe(method: SET_MODEL_PROPERTIES_TYPE): Observable<ModelPropsParams>;
    observe(method: SET_MODEL_TRANSFER_FUNCTION_TYPE): Observable<UpstreamSetTransferFunctionParams>;
    observe(method: UPDATE_CLIP_PLANE_TYPE): Observable<ClipPlane>;
    observe(method: REMOVE_CLIP_PLANES_TYPE): Observable<number[]>;
    observe(method: SET_STATISTICS_TYPE): Observable<Statistics>;
    observe(method: IMAGE_JPEG_TYPE): Observable<Blob>;
    observe(method: SET_ENVIRONMENT_MAP_TYPE): Observable<EnvironmentMap>;
    observe<R>(method: ObservableType | IMAGE_JPEG_TYPE | string): Observable<R | Blob> {
        if (method === IMAGE_JPEG) {
            return this.binary.asObservable();
        }
        return this.notifications.asObservable()
            .pipe(filter(notification => notification.method === method), map(notification => notification.params));
    }

    /**
     * Upload a model
     * @param data
     */
    upload(data: BinaryParams): RequestTask<UploadParams, Model> {
        const {
            file,
            chunkSize
        } = data;

        const chunksId = uid(6);
        const desc = getFileDesc(file);
        const params = {
            ...desc,
            ...pick(data, [
                'name',
                'path',
                'type',
                'transformation',
                'boundingBox',
                'visible',
                'loaderName',
                'loaderProperties'
            ]),
            chunksId
        };

        const buffer = toArrayBuffer(file, {chunkSize});
        const task = this.rockets!.request<UploadParams, Model>(UPLOAD_MODEL, params);

        const sub = buffer.subscribe(chunk => {
            this.rockets!.notify(CHUNK, {id: chunksId});
            this.rockets!.ws.next(chunk);
        });

        task.then(noop, () => {
            // Stop sending chunks if the request failed,
            // or if it was canceled.
            sub.unsubscribe();
        });

        return task;
    }

    private connect(url: string) {
        this.rockets = RocketsClient.create({
            url,
            serializer,
            deserializer,
            protocol: ['rockets'], // TODO: Let's rename this to 'brayns'
            onConnected: () => {
                // Update connection status
                this.connection.next(true);
            },
            onClosed: () => {
                this.reconnect(url);
            }
        });

        // Setup subscriptions
        this.addListeners();
    }

    private addListeners() {
        // Setup subscriptions,
        // but ignore any erros that may occur.
        // NOTE: No need to unsub from these when the ws connection is closed as they will error/complete when that happens,
        // and subsequently the subscriptions will be removed.
        this.rockets!.ws.pipe(filter(blobFilter), map(evt => evt.data))
            .subscribe(data => {
                this.binary.next(data);
            }, noop);
        this.rockets!.subscribe(notification => {
            this.notifications.next(notification);
        }, noop);
    }

    private reconnect(host: string) {
        // Update connection status
        this.connection.next(false);
        // Schedule a connection attempt in 5s from now
        timeout(5000)
            .subscribe(() => {
                this.connect(host);
            });
    }
}


export function appendWsPath(url: string) {
    if (url.lastIndexOf('/') === (url.length - 1)) {
        return `${url}${BRAYNS_WS_PATH}`;
    }
    return `${url}/${BRAYNS_WS_PATH}`;
}

function blobFilter(evt: MessageEvent) {
    return evt.data instanceof Blob;
}

function serializer(data: Notification | Request) {
    const obj = toSnakeCase(data.toJSON());
    const json = JSON.stringify(obj);
    return json;
}

async function deserializer(evt: MessageEvent) {
    const {data} = evt;
    if (!isString(data)) {
        return;
    }
    try {
        const json = JSON.parse(evt.data);
        const obj = toCamelCase(json);
        return obj;
    } catch {
        return;
    }
}


export type RequestType = GET_ANIMATION_PARAMS_TYPE
    | SET_ANIMATION_PARAMS_TYPE
    | GET_APP_PARAMS_TYPE
    | GET_CAMERA_TYPE
    | SET_CAMERA_TYPE
    | GET_CAMERA_PARMS_TYPE
    | SET_CAMERA_PARMS_TYPE
    | GET_RENDERER_TYPE
    | SET_RENDERER_TYPE
    | GET_RENDERER_PARMS_TYPE
    | SET_RENDERER_PARMS_TYPE
    | GET_STATISTICS_TYPE
    | GET_VERSION_TYPE
    | INSPECT_TYPE
    | GET_SCENE_TYPE
    | SET_SCENE_TYPE
    | LOAD_MODEL_TYPE
    | UPLOAD_MODEL_TYPE
    | REMOVE_MODEL_TYPE
    | UPDATE_MODEL_TYPE
    | MODEL_PROPERTIES_SCHEMA_TYPE
    | GET_MODEL_PROPERTIES_TYPE
    | SET_MODEL_PROPERTIES_TYPE
    | GET_MODEL_TRANSFER_FUNCTION_TYPE
    | SET_MODEL_TRANSFER_FUNCTION_TYPE
    | GET_LOADERS_TYPE
    | LOADERS_SCHEMA_TYPE
    | ADD_CLIP_PLANE_TYPE
    | UPDATE_CLIP_PLANE_TYPE
    | GET_CLIP_PLANES_TYPE
    | REMOVE_CLIP_PLANES_TYPE
    | SNAPSHOT_TYPE
    | SCHEMA_TYPE
    | SET_ENVIRONMENT_MAP_TYPE
    | GET_ENVIRONMENT_MAP_TYPE;

export type NotificationType = SET_ANIMATION_PARAMS_TYPE
    | SET_APP_PARAMS_TYPE
    | SET_CAMERA_TYPE
    | SET_CAMERA_PARMS_TYPE
    | SET_RENDERER_TYPE
    | SET_RENDERER_PARMS_TYPE
    | SET_SCENE_TYPE
    | SET_MODEL_PROPERTIES_TYPE
    | UPDATE_CLIP_PLANE_TYPE
    | REMOVE_CLIP_PLANES_TYPE
    | SET_MODEL_TRANSFER_FUNCTION_TYPE
    | QUIT_TYPE;

export type ObservableType = SET_ANIMATION_PARAMS_TYPE
    | SET_APP_PARAMS_TYPE
    | SET_CAMERA_TYPE
    | SET_CAMERA_PARMS_TYPE
    | SET_RENDERER_TYPE
    | SET_RENDERER_PARMS_TYPE
    | SET_SCENE_TYPE
    | SET_MODEL_PROPERTIES_TYPE
    | SET_MODEL_TRANSFER_FUNCTION_TYPE
    | SET_STATISTICS_TYPE
    | SET_ENVIRONMENT_MAP_TYPE;

export type SchemaType = RequestType
    | NotificationType
    | ObservableType
    | ANIMATION_PARAMS_TYPE
    | APP_PARAMS_TYPE
    | CAMERA_TYPE
    | RENDERER_TYPE
    | RENDERER_PARMS_TYPE
    | SCENE_TYPE
    | STATISTICS_TYPE
    | VERSION_TYPE;
