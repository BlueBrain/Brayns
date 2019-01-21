import React, {PureComponent} from 'react';

import uid from 'crypto-uid';
import {findLastIndex} from 'lodash';

import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';

import {CanvasRect, Consumer, ProviderContext} from './provider';
import {ControlPoint, PointCoords} from './types';
import {createToRectScaler} from './utils';


const GRADIENT_ID = uid();

const styles = (theme: Theme) => createStyles({});
const style = withStyles(styles);

class Area extends PureComponent<Props> {
    render() {
        const {colormap, points} = this.props;
        const path = this.createPathRenderer();
        const gradient = createGradient(colormap, points);

        return (
            <g>
                {gradient}
                <Consumer>
                    {path}
                </Consumer>
            </g>
        );
    }

    private createPathRenderer() {
        const {points} = this.props;
        return (context: ProviderContext) => {
            const path = createPath(points, context.canvas);
            const gradient = `url(#${GRADIENT_ID})`;

            return (
                <path
                    fill={gradient}
                    d={path}
                />
            );
        };
    }
}

export default style(Area);


function createPath(points: ControlPoint[], rect?: CanvasRect) {
    const toRectScale = createToRectScaler(rect);

    const lines = points.map(toRectScale)
        .map(({x, y}) => `L ${x},${y}`)
        .join(' ');

    const start = createPathStart(rect);
    const end = createPathEnd(rect);

    return `
        ${start}
        ${lines}
        ${end}
        z
    `;
}

function createPathStart(rect?: CanvasRect) {
    if (rect) {
        return `M 0,${rect.height}`;
    }
    return 'M 0,0';
}

function createPathEnd(rect?: CanvasRect) {
    if (rect) {
        return `L ${rect.width},${rect.height}`;
    }
    return 'M 0,0';
}

function createGradient(colormap: string[], points: ControlPoint[]) {
    const max = colormap.length - 1;
    const findYForX = createYFinder(points);

    const stops = colormap.map((color, index) => {
        const x = index / max;
        return (
            <stop
                key={`stop-${index}`}
                offset={x}
                stopColor={color}
                stopOpacity={1 - findYForX(x)}
            />
        );
    });

    return (
        <defs>
            <linearGradient id={GRADIENT_ID}>
                {stops}
            </linearGradient>
        </defs>
    );
}

function createYFinder(points: ControlPoint[]) {
    const lastIndex = points.length - 1;
    return (x: number) => {
        const index = findLastIndex(points, (p: ControlPoint) => p.x <= x);
        const left = points[index];
        const next = index < lastIndex ? index + 1 : lastIndex;
        const right = points[next];

        if (left === right) {
            return left.y;
        }

        const intersect = createLineIntercept(left, right);
        const coords = intersect(x);

        return coords.y;
    };
}

/**
 * Create a fn to find the interesection of two lines given the 2nd line's X coord
 * @see http://paulbourke.net/geometry/pointlineplane
 */

const NAN_COORDS = {x: NaN, y: NaN};

export function createLineIntercept(p1: PointCoords, p2: PointCoords): (x: number) => PointCoords {
    // Our 2nd line will always have the coords at (x, y = 0), (x, y = 1)
    return (x3: number) => {
        if (p1.x === p2.x && p1.y === p2.y) {
            return NAN_COORDS;
        }

        const d = p2.x - p1.x;

        if (d === 0) {
            return NAN_COORDS;
        }

        const ua = (x3 - p1.x) / d;
        const ub = ((p2.x - p1.x) * p1.y - (p2.y - p1.y) * (p1.x - x3)) / d;

        if (ua < 0 || ua > 1 || ub < 0 || ub > 1) {
            return NAN_COORDS;
        }

        const x = p1.x + ua * (p2.x - p1.x);
        const y = p1.y + ua * (p2.y - p1.y);

        return {x, y};
    };
}


type Props = WithStyles<typeof styles> & AreaProps;

export interface AreaProps {
    points: ControlPoint[];
    colormap: string[];
}
