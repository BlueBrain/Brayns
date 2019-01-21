import React, {PureComponent} from 'react';

import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';

import {CanvasRect, Consumer, ProviderContext} from './provider';
import {ControlPoint} from './types';
import {createToRectScaler, getColor} from './utils';


const styles = (theme: Theme) => createStyles({
    path: {
        stroke: getColor(theme),
        strokeWidth: 0.8
    }
});
const style = withStyles(styles);

class Path extends PureComponent<Props> {
    render() {
        const path = this.createPathRenderer();
        return (
            <Consumer>
                {path}
            </Consumer>
        );
    }

    private createPathRenderer() {
        const {classes, points} = this.props;
        return (context: ProviderContext) => {
            const path = createPath(points, context.canvas);

            return (
                <path
                    className={classes.path}
                    fillOpacity={0}
                    d={path}
                />
            );
        };
    }
}

export default style(Path);


function createPath(points: ControlPoint[], rect?: CanvasRect) {
    if (points.length < 2) {
        return '';
    }

    const toRectScale = createToRectScaler(rect);

    const [x0, ...rest] = points.map(toRectScale);
    const lines = rest.map(({x, y}) => `L ${x},${y}`)
        .join(' ');

    const start = `M ${x0.x},${x0.y}`;

    return `
        ${start}
        ${lines}
    `;
}


type Props = WithStyles<typeof styles> & PathProps;

export interface PathProps {
    points: any[];
}
