import {Model} from 'brayns';

export type ModelSettings = Pick<Model, 'visible' | 'boundingBox'>;
export type ModelInfo = Pick<Model, 'id' | 'name' | 'bounds' | 'metadata'>;
export type ModelProps = Pick<Model, 'transformation'>
    & ModelSettings;

export type ModelId = Model['id'];
