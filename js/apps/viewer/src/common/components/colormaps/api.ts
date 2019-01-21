// TODO: This should be a package of colormaps.io
import {ColorSpaces} from 'chroma-js';

const COLORMAPS_API_URL = 'https://api.colormaps.io';

export async function getColormapsList(): Promise<string[]> {
    try {
        const response = await fetch(`${COLORMAPS_API_URL}/colormap`);
        const data = await response.json();
        return data;
    } catch {
        return [];
    }
}

export async function findColormapsByName(name: string, signal?: AbortSignal): Promise<string[]> {
    const response = await fetch(`${COLORMAPS_API_URL}/colormap/search`, {
        signal,
        method: 'POST',
        body: JSON.stringify({name})
    });

    if (response.status !== 200) {
        const err = await response.json();
        throw new Error(err.message);
    }

    const data = await response.json();
    return data;
}

export async function getColormap<T = string>(name: string, format: Format = 'hex'): Promise<T[]> {
    try {
        const response = await fetch(`${COLORMAPS_API_URL}/colormap/${name}?format=${format}`);
        const data = await response.json();
        return data;
    } catch {
        return [];
    }
}

export type Format = 'hex' | keyof Pick<ColorSpaces, 'rgb'
    | 'hsl'
    | 'hsv'
    | 'hsi'
    | 'lab'
    | 'lch'
    | 'hcl'
    | 'cmyk'
    | 'gl'>;
