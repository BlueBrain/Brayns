import {ImageFormat} from './constants';

/**
 * Scene/Model
 */
export interface Scene {
    bounds: BoundingBox;
    models: Model[];
}

export interface Model extends ModelParams {
    id: string | number;
    bounds: BoundingBox;
    metadata: object;
}

export interface UploadParams extends Pick<BinaryParams, 'type'>,
    Partial<ModelParams> {
    chunksId: string | number;
    size: number;
}

export interface BinaryParams extends Partial<ModelParams> {
    file: File;
    type?: string;
    chunkSize?: number;
}

export type PathParams = Partial<ModelParams>;

export interface BinaryError {
    index: number;
    supportedTypes: string[];
}

export interface ChunkParams {
    id: string | number;
}

export interface ModelUpdateParams extends Partial<ModelParams> {
    id: string | number;
}

export type GetModelPropsParams = Pick<ModelPropsParams, 'id'>;
export type GetModelPropsSchemaParams = GetModelPropsParams;

export interface ModelPropsParams {
    id: string | number;
    properties: {
        [key: string]: any;
    };
}

export interface ModelParams {
    name: string;
    path: string;
    transformation: Transformation;
    boundingBox: boolean;
    visible: boolean;
    loaderName: string;
    loaderProperties: object;
}

export interface Transformation {
    translation: number[];
    rotation: number[];
    rotationCenter: number[];
    scale: number[];
}

export interface SetTransferFunctionParams extends TransferFunctionParams {
    transferFunction: Partial<TransferFunction>;
}

export interface UpstreamSetTransferFunctionParams extends TransferFunctionParams {
    transferFunction: TransferFunction;
}

export interface TransferFunctionParams {
    id: string | number;
}

export interface TransferFunction {
    opacityCurve: Vector2d[];
    colormap: ColorMap;
    range: Vector2d;
}

export interface ColorMap {
    name: string;
    colors: Vector3d[]; // [[r, g, b], ...]
}

export interface Loader {
    name: string;
    extensions: string[];
}


/**
 * Clip planes
 */
export interface ClipPlane {
    id: number;
    plane: Plane;
}

export type Plane = Vector4d;


export interface ApplicationParameters {
    jpegCompression: number;
    frameExportFolder: string;
    synchronousMode: boolean;
    imageStreamFps: number;
    viewport: number[];
}

export interface CancelParams {
    id: string | number;
}

export type SetAnimationParameters = Partial<Pick<AnimationParameters, 'current' | 'delta' | 'playing'>>;

export interface AnimationParameters {
    current: number;
    delta: number;
    dt: number;
    frameCount: number;
    playing: boolean;
    unit: string;
}


// Coords of a point in 2d space (e.g. [0 ,1])
export type InspectParams = Vector2d;

export interface InspectCoords {
    hit: boolean;
    position: number[];
}


export type RendererParams = Partial<Pick<Renderer, 'current' | 'samplesPerPixel' | 'subsampling' | 'maxAccumFrames' | 'backgroundColor' | 'headLight' | 'varianceThreshold'>>;
export interface Renderer extends AbstractObject {
    samplesPerPixel: number;
    subsampling: number;
    maxAccumFrames: number;
    backgroundColor: number[];
    headLight: boolean;
    varianceThreshold: number;
}

export type CameraParams = Partial<Pick<Camera, 'current' | 'position' | 'orientation' | 'target'>>;
export interface Camera extends AbstractObject {
    position: number[];
    orientation: number[];
    target: number[];
}

// TODO: Find better name?
export interface AbstractObject {
    current: string;
    types: string[];
}

export interface BoundingBox {
    max: number[];
    min: number[];
}

export interface SnapshotParams {
    name: string;
    size: number[];
    samplesPerPixel: number;
    format: ImageFormat;
    quality: number;
    animationParameters?: Partial<AnimationParameters>;
    renderer?: RendererParams;
    camera?: Partial<Camera>;
}

export interface Snapshot {
    /**
     * Base 64 encoded image
     */
    data: string;
}

export interface Statistics {
    fps: number;
    sceneSizeInBytes: number;
}

export interface SchemaParams {
    endpoint: string;
}

export interface EnvironmentMap {
    filename: string;
}

export interface Version {
    major: number;
    minor: number;
    patch: number;
    abi: number;
    revision: string;
}


export interface Vector2d extends Array<number> {
    0: number;
    1: number;
    length: 2;
}

export interface Vector3d extends Array<number> {
    0: number;
    1: number;
    2: number;
    length: 3;
}

export interface Vector4d extends Array<number> {
    0: number;
    1: number;
    2: number;
    3: number;
    length: 4;
}
