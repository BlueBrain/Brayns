/**
 * Generally (unless specified otherwise):
 * 'get-*' - RPC request
 * 'set-*' - RPC request/notification
 */


/**
 * Animation params
 */
export const SET_ANIMATION_PARAMS = 'set-animation-parameters';
export type SET_ANIMATION_PARAMS_TYPE = typeof SET_ANIMATION_PARAMS;
export const GET_ANIMATION_PARAMS = 'get-animation-parameters';
export type GET_ANIMATION_PARAMS_TYPE = typeof GET_ANIMATION_PARAMS;
export const ANIMATION_PARAMS = 'animation-parameters';
export type ANIMATION_PARAMS_TYPE = typeof ANIMATION_PARAMS;


/**
 * App params
 */
export const SET_APP_PARAMS = 'set-application-parameters';
export type SET_APP_PARAMS_TYPE = typeof SET_APP_PARAMS;
export const GET_APP_PARAMS = 'get-application-parameters';
export type GET_APP_PARAMS_TYPE = typeof GET_APP_PARAMS;
export const APP_PARAMS = 'application-parameters';
export type APP_PARAMS_TYPE = typeof APP_PARAMS;


/**
 * Statistics (rendering fps, scene size, etc.)
 */
export const GET_STATISTICS = 'get-statistics';
export type GET_STATISTICS_TYPE = typeof GET_STATISTICS;
export const STATISTICS = 'statistics';
export type STATISTICS_TYPE = typeof STATISTICS;


/**
 * Version
 */
export const GET_VERSION = 'get-version';
export type GET_VERSION_TYPE = typeof GET_VERSION;
export const VERSION = 'version';
export type VERSION_TYPE = typeof VERSION;


/**
 * Notifications from server -> client
 * NOTE: Clients cannot update these as they are side effects of other operations
 */
// Statistics (rendering fps, scene size, etc.)
export const SET_STATISTICS = 'set-statistics';
export type SET_STATISTICS_TYPE = typeof SET_STATISTICS;

// Image as binary
export const IMAGE_JPEG = 'image-jpeg';
export type IMAGE_JPEG_TYPE = typeof IMAGE_JPEG;


/**
 * Renderer
 */
// Renderer
export const GET_RENDERER = 'get-renderer';
export type GET_RENDERER_TYPE = typeof GET_RENDERER;
export const SET_RENDERER = 'set-renderer';
export type SET_RENDERER_TYPE = typeof SET_RENDERER;
export const RENDERER = 'renderer';
export type RENDERER_TYPE = typeof RENDERER;

// Renderer params
export const GET_RENDERER_PARAMS = 'get-renderer-params';
export type GET_RENDERER_PARMS_TYPE = typeof GET_RENDERER_PARAMS;
export const SET_RENDERER_PARAMS = 'set-renderer-params';
export type SET_RENDERER_PARMS_TYPE = typeof SET_RENDERER_PARAMS;
export const RENDERER_PARAMS = 'renderer-params';
export type RENDERER_PARMS_TYPE = typeof RENDERER_PARAMS;


/**
 * Camera
 */
// Camera
export const SET_CAMERA = 'set-camera';
export type SET_CAMERA_TYPE = typeof SET_CAMERA;
export const GET_CAMERA = 'get-camera';
export type GET_CAMERA_TYPE = typeof GET_CAMERA;
export const CAMERA = 'camera';
export type CAMERA_TYPE = typeof CAMERA;

// Camera params
export const GET_CAMERA_PARAMS = 'get-camera-params';
export type GET_CAMERA_PARMS_TYPE = typeof GET_CAMERA_PARAMS;
export const SET_CAMERA_PARAMS = 'set-camera-params';
export type SET_CAMERA_PARMS_TYPE = typeof SET_CAMERA_PARAMS;
export const CAMERA_PARAMS = 'camera-params';
export type CAMERA_PARMS_TYPE = typeof CAMERA_PARAMS;


/**
 * Model
 */
// Scene
// 'set-scene' cannot be a RPC notification
export const SET_SCENE = 'set-scene';
export type SET_SCENE_TYPE = typeof SET_SCENE;
export const GET_SCENE = 'get-scene';
export type GET_SCENE_TYPE = typeof GET_SCENE;
export const SCENE = 'scene';
export type SCENE_TYPE = typeof SCENE;

// Load
// RPC request
export const LOAD_MODEL = 'add-model';
export type LOAD_MODEL_TYPE = typeof LOAD_MODEL;

// Upload
// RPC request
export const UPLOAD_MODEL = 'request-model-upload';
export type UPLOAD_MODEL_TYPE = typeof UPLOAD_MODEL;
// Inform server of incoming chunk
// Works in conjecture with upload model:
// before any binary chunk upload we must send a chunk notification with some info.
// RPC notification
export const CHUNK = 'chunk';
export type CHUNK_TYPE = typeof CHUNK;

// Remove
// RPC request
export const REMOVE_MODEL = 'remove-model';
export type REMOVE_MODEL_TYPE = typeof REMOVE_MODEL;

// Update
// RPC request
export const UPDATE_MODEL = 'update-model';
export type UPDATE_MODEL_TYPE = typeof UPDATE_MODEL;

// Props
export const GET_MODEL_PROPERTIES = 'get-model-properties';
export type GET_MODEL_PROPERTIES_TYPE = typeof GET_MODEL_PROPERTIES;
export const SET_MODEL_PROPERTIES = 'set-model-properties';
export type SET_MODEL_PROPERTIES_TYPE = typeof SET_MODEL_PROPERTIES;
export const MODEL_PROPERTIES_SCHEMA = 'model-properties-schema';
export type MODEL_PROPERTIES_SCHEMA_TYPE = typeof MODEL_PROPERTIES_SCHEMA;

// Transfer function
export const SET_MODEL_TRANSFER_FUNCTION = 'set-model-transfer-function';
export type SET_MODEL_TRANSFER_FUNCTION_TYPE = typeof SET_MODEL_TRANSFER_FUNCTION;
export const GET_MODEL_TRANSFER_FUNCTION = 'get-model-transfer-function';
export type GET_MODEL_TRANSFER_FUNCTION_TYPE = typeof GET_MODEL_TRANSFER_FUNCTION;

// Loader
export const GET_LOADERS = 'get-loaders';
export type GET_LOADERS_TYPE = typeof GET_LOADERS;
export const LOADERS_SCHEMA = 'loaders-schema';
export type LOADERS_SCHEMA_TYPE = typeof LOADERS_SCHEMA;


/**
 * Clip planes
 */
export const ADD_CLIP_PLANE = 'add-clip-plane';
export type ADD_CLIP_PLANE_TYPE = typeof ADD_CLIP_PLANE;
export const UPDATE_CLIP_PLANE = 'update-clip-plane';
export type UPDATE_CLIP_PLANE_TYPE = typeof UPDATE_CLIP_PLANE;
export const GET_CLIP_PLANES = 'get-clip-planes';
export type GET_CLIP_PLANES_TYPE = typeof GET_CLIP_PLANES;
export const REMOVE_CLIP_PLANES = 'remove-clip-planes';
export type REMOVE_CLIP_PLANES_TYPE = typeof REMOVE_CLIP_PLANES;


/**
 * RPC requests
 */
// Inspect
export const INSPECT = 'inspect';
export type INSPECT_TYPE = typeof INSPECT;

// Take snapshot
export const SNAPSHOT = 'snapshot';
export type SNAPSHOT_TYPE = typeof SNAPSHOT;

// Schema
export const SCHEMA = 'schema';
export type SCHEMA_TYPE = typeof SCHEMA;

// Environment map
export const SET_ENVIRONMENT_MAP = 'set-environment-map';
export type SET_ENVIRONMENT_MAP_TYPE = typeof SET_ENVIRONMENT_MAP;
export const GET_ENVIRONMENT_MAP = 'get-environment-map';
export type GET_ENVIRONMENT_MAP_TYPE = typeof GET_ENVIRONMENT_MAP;

/**
 * RPC Notifications from client -> server
 */
// Quit
export const QUIT = 'quit';
export type QUIT_TYPE = typeof QUIT;


// See http://freeimage.sourceforge.net/features.html for more file formats
export enum ImageFormat {
    Jpeg = 'jpeg',
    Png = 'png',
    Bmp = 'bmp',
    Tiff = 'tiff'
}
