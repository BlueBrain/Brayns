import React, {
    createRef,
    PureComponent
} from 'react';

import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';

import {getColor} from './utils';


const styles = (theme: Theme) => createStyles({
    circle: {
        stroke: getColor(theme),
        strokeWidth: 0.8
    }
});
const style = withStyles(styles);


class Point extends PureComponent<Props> {
    ref = createRef<SVGGElement>();

    attachOnMouseMove = (evt: React.MouseEvent<SVGGElement>) => {
        evt.preventDefault();
        const {onChange, onRemove} = this.props;

        if (evt.shiftKey && onRemove) {
            evt.stopPropagation();
            onRemove();
            return;
        }

        document.addEventListener('mousemove', onMouseMove, false);
        document.addEventListener('mouseup', () => {
            document.removeEventListener('mousemove', onMouseMove);
        }, {once: true});

        function onMouseMove(evt: MouseEvent) {
            evt.preventDefault();
            evt.stopPropagation();

            if (onChange) {
                onChange(evt);
            }
        }
    }

    render() {
        const {cx, cy, r, color, classes} = this.props;
        return (
            <g onMouseDown={this.attachOnMouseMove}>
                <circle
                    cx={cx}
                    cy={cy}
                    r={r * 2}
                    fillOpacity={0}
                />
                <circle
                    className={classes.circle}
                    cx={cx}
                    cy={cy}
                    r={r}
                    fill={color}
                />
            </g>
        );
    }
}

export default style(Point);


type Props = WithStyles<typeof styles> & PointProps;

export interface PointProps {
    cx: number;
    cy: number;
    r: number;
    color?: string;
    onChange?(evt: MouseEvent): void;
    onRemove?(): void;
}
