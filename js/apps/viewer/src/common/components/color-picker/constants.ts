import {Color} from '@material-ui/core';
import * as colors from '@material-ui/core/colors';

export const MATERIAL_PALETTES: Color[] = Object.entries(colors)
    .filter(([key]) => key !== 'common')
    .map(([, palette]) => palette) as any;
