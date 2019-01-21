import {createContext} from 'react';

export const {
    Provider,
    Consumer
} = createContext<ProviderContext>({});

export interface ProviderContext {
    canvas?: CanvasRect;
}

export type CanvasRect = Pick<ClientRect, 'width'
    | 'height'
    | 'top'
    | 'left'>;
