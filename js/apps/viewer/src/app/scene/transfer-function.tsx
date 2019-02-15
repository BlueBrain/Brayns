// tslint:disable: member-ordering
import React, {PureComponent} from 'react';

import {
    ColorMap,
    GET_MODEL_TRANSFER_FUNCTION,
    SET_MODEL_TRANSFER_FUNCTION,
    TransferFunction,
    Vector2d,
    Vector3d
} from 'brayns';
import chroma from 'chroma-js';
import {memoize} from 'lodash';
import {Subscription} from 'rxjs';
import {filter, mergeMap} from 'rxjs/operators';

import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';
import Typography from '@material-ui/core/Typography';

import brayns, {onReady} from '../../common/client';
import {
    ColormapAutocomplete,
    getColormap,
    NumericField,
    PointCoords,
    TransferFunction as ReactTransferFunction
} from '../../common/components';

import {ModelId} from './types';


const styles = (theme: Theme) => createStyles({
    range: {
        display: 'flex',
        flexDirection: 'row'
    },
    gutterRight: {
        marginRight: theme.spacing.unit * 2
    }
});
const style = withStyles(styles);


export class ModelTransferFunction extends PureComponent<Props, State> {
    state: State = {
        colors: ['#fff', '#222'],
        data: [
            {x: 0, y: 0},
            {x: 1, y: 1}
        ],
        colormap: {name: ''},
        range: [0, 1]
    };

    private subs: Subscription[] = [];

    update = (data: PointCoords[]) => {
        this.setState({data}, () => {
            this.broadcast({
                opacityCurve: data.map(toVec2d)
            });
        });
    }

    setColormap = async (name: string) => {
        const colors = await getColormap<Vector3d>(name, 'gl');
        const colormap = {name, colors};
        this.setState({colormap, colors: colors.map(fromVec3d)}, () => {
            this.broadcast({colormap});
        });
    }

    updateRange = memoize((key: 'min' | 'max') => (value: number) => this.setState(state => {
        const {range} = state;
        const [min, max] = range;

        if (key === 'min') {
            return {
                range: [value, max]
            };
        }

        return {
            range: [min, value]
        };
    }, () => {
        const {range} = this.state;
        this.broadcast({
            range: range as Vector2d
        });
    }));

    broadcast(transferFunction: Partial<TransferFunction>) {
        brayns.notify(SET_MODEL_TRANSFER_FUNCTION, {
            id: this.props.id,
            transferFunction
        });
    }

    componentDidMount() {
        const id = this.props.id!;
        this.subs.push(...[
            brayns.observe(SET_MODEL_TRANSFER_FUNCTION)
                .pipe(filter(params => params.id === id))
                .subscribe(({transferFunction}) => {
                    this.setState(toState(transferFunction));
                }),
            onReady().pipe(mergeMap(() => brayns.request(GET_MODEL_TRANSFER_FUNCTION, {id})))
                .subscribe(tf => {
                    this.setState(toState(tf));
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
        const {classes, disabled} = this.props;
        const {data, colormap, range, colors} = this.state;
        const [min, max] = range;

        return (
            <div>
                <ReactTransferFunction
                    data={data}
                    colormap={colors}
                    range={range}
                    onChange={this.update}
                    disabled={disabled}
                />

                <Typography variant="caption" color="textSecondary">
                    * Click and drag to move points, click to add or hold SHIFT and click to remove them
                </Typography>

                <div className={classes.range}>
                    <NumericField
                        label="Min"
                        value={min}
                        onChange={this.updateRange('min')}
                        className={classes.gutterRight}
                        margin="normal"
                        disabled={disabled}
                    />

                    <NumericField
                        label="Max"
                        value={max}
                        onChange={this.updateRange('max')}
                        margin="normal"
                        disabled={disabled}
                    />
                </div>

                <div>
                    <ColormapAutocomplete
                        value={colormap.name}
                        onChange={this.setColormap}
                        margin="normal"
                        disabled={disabled}
                        fullWidth
                    />
                </div>
            </div>
        );
    }
}


export default style(ModelTransferFunction);


function toState(tf: TransferFunction): State {
    const {opacityCurve, colormap, range} = tf;

    if (opacityCurve.length > 1) {
        return {
            colormap,
            range,
            data: opacityCurve.map(fromVec2d),
            colors: colormap.colors.map(fromVec3d)
        };
    }

    return {} as any;
}

function toVec2d(point: PointCoords): Vector2d {
    return [point.x, point.y];
}

function fromVec2d(vec: Vector2d): PointCoords {
    const [x, y] = vec;
    return {x, y};
}

function fromVec3d(rgb: Vector3d): string {
    const hex = chroma(rgb, 'gl')
        .hex();
    return hex;
}


interface Props extends WithStyles<typeof styles> {
    id: ModelId;
    disabled?: boolean;
}

interface State {
    data: PointCoords[];
    colormap: Partial<ColorMap>;
    colors: string[];
    range: number[];
}
