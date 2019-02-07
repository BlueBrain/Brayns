import React, {
    createRef,
    PureComponent
} from 'react';

import {scale, Scale} from 'chroma-js';
import classNames from 'classnames';
import uid from 'crypto-uid';
import {axisBottom, axisLeft} from 'd3-axis';
import {scaleLinear} from 'd3-scale';
import {select, Selection} from 'd3-selection';
import {
    clone,
    findLastIndex,
    isEqual,
    memoize
} from 'lodash';
import {Subject, Subscription} from 'rxjs';
import {
    debounceTime,
    distinctUntilChanged,
    map
} from 'rxjs/operators';

import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';

import {WithRect, withResizeObserver} from '../resize-observer';

import Area from './area';
import Path from './path';
import Point from './point';
import {CanvasRect, Provider} from './provider';
import {ControlPoint, PointCoords} from './types';
import {
    getColor,
    getPointCoords,
    isPointOutsideCanvas
} from './utils';


const EPSILON = 0.015;
const UID_BYTE_SIZE = 4;
export const POINT_SIZE = 4;

const styles = (theme: Theme) => {
    const padding = getPadding(theme);

    return createStyles({
        root: {
            ...padding,
            // IMPORTANT: Needs to be set,
            // otherwise mousedown on points does not react if it's outside the ticks
            overflow: 'visible',
            // https://developer.mozilla.org/en-US/docs/Web/SVG/Attribute/text-rendering
            textRendering: 'optimizeLegibility',
            width: '100%',
            transition: 'opacity .25s ease',
            opacity: 1
        },
        disabled: {
            pointerEvents: 'none',
            userSelect: 'none',
            opacity: 0.4
        },
        axisLeft: {},
        axisBottom: {}
    });
};
const style = withStyles(styles, {
    withTheme: true
});


class TransferFunction extends PureComponent<Props, State> {
    state: State = toState(this.props.data, this.props.colormap);

    axisLeftRef = createRef<SVGGElement>();
    axisBottomRef = createRef<SVGGElement>();

    changes = new Subject<ControlPoint[]>();
    sub?: Subscription;

    updateAxisLeft = memoize((height: number, theme: Theme) => this.renderAxisLeft(height, theme), axisLeftKey);
    updateAxisBottom = memoize((range: number[], width: number, theme: Theme) => this.renderAxisBottom(range, width, theme), axisBottomKey);

    createPointUpdater = memoize((point: ControlPoint) => (evt: MouseEvent) => this.setState(state => {
        const copy = clone(state.points);
        const index = copy.findIndex(p => p.id === point.id);
        const canvas = this.canvas;

        if (index !== -1 && canvas) {
            const min = copy[index - 1];
            const max = copy[index + 1];

            const coords = getPointCoords(evt, canvas);
            const x = getBoundedXCoord(coords, canvas, min, max);

            copy.splice(index, 1, {
                ...point,
                ...coords,
                x,
                color: state.interpolate(x)
                    .hex()
            });

            return {
                points: copy
            };
        }

        return {} as any;
    }, this.emitChanges), pointKey);

    createPointRemover = memoize((point: ControlPoint) => () => this.setState(state => {
        const copy = clone(state.points);
        const index = copy.findIndex(p => p.id === point.id);

        if (index > 0 && index < copy.length - 1) {
            copy.splice(index, 1);
            return {
                points: copy
            };
        }

        return {} as any;
    }, this.emitChanges), pointKey);

    get canvas() {
        const {rect, theme} = this.props;
        return computeCanvasRect(rect!, theme!);
    }

    addPoint = (evt: React.MouseEvent<SVGGElement>) => {
        evt.persist();
        evt.preventDefault();

        this.setState(state => {
            const copy = clone(state.points);
            const canvas = this.canvas;

            if (canvas) {
                const coords = getPointCoords(evt, canvas);
                const index = findLastIndex(copy, p => p.x < coords.x);

                if (isPointOutsideCanvas(evt, canvas) || isPointOnAxis(copy, canvas, coords)) {
                    return {};
                }

                copy.splice(index + 1, 0, {
                    ...coords,
                    id: uid(UID_BYTE_SIZE),
                    color: state.interpolate(coords.x)
                        .hex()
                });

                return {
                    points: copy
                };
            }

            return {} as any;
        }, this.emitChanges);
    }

    emitChanges = () => {
        this.changes.next(this.state.points);
    }

    componentDidMount() {
        this.updateAxis();
        this.sub = this.changes.pipe(
            debounceTime(250),
            map(toPointCoords),
            distinctUntilChanged(isEqual))
            .subscribe(changes => {
                this.setState({data: changes}, () => {
                    const {onChange} = this.props;
                    if (onChange) {
                        onChange(changes);
                    }
                });
            });
    }

    componentDidUpdate(prevProps: Readonly<Props>) {
        this.updateAxis();
        this.updateTf(prevProps);
    }

    componentWillUnmount() {
        if (this.sub) {
            this.sub.unsubscribe();
        }
    }

    render() {
        const {points} = this.state;
        const {colormap, classes, disabled, rect, rectRef} = this.props;
        const canvas = this.canvas;

        const axisBottomTranslate = translateToBottom(canvas);

        const pointElements = points.map(point => {
            const update = this.createPointUpdater(point);
            const remove = this.createPointRemover(point);
            const key = `point-${point.id}`;
            const cx = point.x * canvas.width;
            const cy = point.y * canvas.height;

            return (
                <Point
                    key={key}
                    cx={cx}
                    cy={cy}
                    r={POINT_SIZE}
                    color={point.color}
                    onChange={update}
                    onRemove={remove}
                />
            );
        });

        return (
            <div ref={rectRef}>
                <svg
                    className={classNames(classes.root, {[classes.disabled]: disabled})}
                    width={rect!.width}
                    height={200}
                    onMouseDown={this.addPoint}
                >
                    <Provider value={{canvas}}>
                        <Area
                            points={points}
                            colormap={colormap}
                        />
                        <Path points={points} />
                        <g
                            className={classes.axisLeft}
                            ref={this.axisLeftRef}
                        />
                        <g
                            className={classes.axisBottom}
                            ref={this.axisBottomRef}
                            transform={axisBottomTranslate}
                        />
                        <g>{pointElements}</g>
                    </Provider>
                </svg>
            </div>
        );
    }

    private updateAxis() {
        const canvas = this.canvas;
        const {range, theme} = this.props;
        if (canvas) {
            this.updateAxisLeft(canvas.height, theme!);
            this.updateAxisBottom(range, canvas.width, theme!);
        }
    }

    private renderAxisLeft(height: number, theme: Theme) {
        const {current} = this.axisLeftRef;
        if (current) {
            // Keep only the last cached args
            // Lodash memoize() stores args for all calls over time
            this.updateAxisLeft.cache.clear!();

            const node = select(current);
            const axis = createAxisLeft(height);
            node.call(axis);

            styleTicks(node, theme);
        }
    }

    private renderAxisBottom(domain: number[], width: number, theme: Theme) {
        const {current} = this.axisBottomRef;
        if (current) {
            // Keep only the last cached args
            // Lodash memoize() stores args for all calls over time
            this.updateAxisBottom.cache.clear!();

            const node = select(current);
            const axis = createAxisBottom(domain, width);
            node.call(axis);

            styleTicks(node, theme);
        }
    }

    private updateTf(prevProps: Readonly<Props>) {
        const {data, colormap} = this.props;
        const {points} = this.state;
        if ((!isEqual(data, prevProps.data) && !isEqual(data, toPointCoords(points)))
            || !isEqual(colormap, prevProps.colormap)) {
            this.setState(toState(data, colormap));
        }
    }
}

export default style(
    withResizeObserver(TransferFunction));


function toState(data: PointCoords[], colormap: string[]): State {
    const interpolate = scale(colormap);
    const toControlPoint = createControlPointMapper(interpolate);
    const points = data.map(toControlPoint);

    return {
        data,
        interpolate,
        points
    };
}

function computeCanvasRect(rect: ClientRect, theme: Theme) {
    const {
        paddingBottom,
        paddingLeft,
        paddingRight,
        paddingTop
    } = getPadding(theme);
    const {width, height, top, left} = rect;

    return {
        width: width - paddingLeft - paddingRight,
        left: left + paddingLeft,
        height: height - paddingTop - paddingBottom,
        top: top + paddingTop
    };
}

function createControlPointMapper(interpolate: Scale) {
    return (datum: PointCoords) => ({
        ...datum,
        id: uid(UID_BYTE_SIZE),
        y: 1 - datum.y,
        color: interpolate(datum.x)
            .hex()
    });
}

function isPointOnAxis(points: ControlPoint[], rect: CanvasRect, {x}: PointCoords) {
    const rx = POINT_SIZE / rect.width;

    const match = points.findIndex(point => {
        const a = point.x - rx - EPSILON;
        const c = point.x + rx + EPSILON;
        return x >= a && x <= c;
    });

    return match !== -1;
}

function getBoundedXCoord({x}: PointCoords, rect: CanvasRect, min?: PointCoords, max?: PointCoords) {
    if (!min) {
        return 0;
    } else if (!max) {
        return 1;
    }

    const rx = POINT_SIZE / rect.width;
    const d = rx * 2;
    const x0 = x - rx;
    const x1 = x + rx;

    if (min && x0 <= (min.x + rx)) {
        return min.x + d;
    } else if (max && x1 >= (max.x - rx)) {
        return max.x - d;
    }

    return x;
}

function createAxisLeft(height: number) {
    const scale = scaleLinear()
        .range([height, 0])
        .domain([0, 1]);
    const axis = axisLeft(scale);
    return axis;
}

function createAxisBottom(domain: number[], width: number) {
    const scale = scaleLinear()
        .range([0, width])
        .domain(domain);
    const axis = axisBottom(scale);
    return axis;
}

function axisLeftKey(height: number, theme: Theme) {
    return `${height}:${theme.palette.type}`;
}

function axisBottomKey([min, max]: number[], width: number, theme: Theme) {
    return `${width}:${min}:${max}:${theme.palette.type}`;
}

function pointKey(point: ControlPoint) {
    return point.id;
}

function translateToBottom(rect?: CanvasRect) {
    const height = rect ? rect.height : 0;
    return `translate(0, ${height})`;
}

function getPadding(theme: Theme) {
    const {unit} = theme.spacing;
    return {
        paddingTop: unit,
        paddingRight: unit,
        paddingBottom: unit * 3,
        paddingLeft: unit * 3
    };
}

function styleTicks(node: Selection<SVGGElement, {}, null, undefined>, theme: Theme) {
    const color = getColor(theme);
    node.attr('font-family', theme.typography.fontFamily!)
        .attr('color', color);
}

function toPointCoords(points: ControlPoint[]): PointCoords[] {
    return points.map(point => ({
        x: point.x,
        y: 1 - point.y
    }));
}


type Props = TransferFunctionProps
    & WithStyles<typeof styles, true>
    & WithRect;

interface State {
    data: PointCoords[];
    points: ControlPoint[];
    interpolate: Scale;
}

export interface TransferFunctionProps {
    data: PointCoords[];
    colormap: string[];
    range: number[];
    disabled?: boolean;
    onChange?(data: PointCoords[]): void;
}
