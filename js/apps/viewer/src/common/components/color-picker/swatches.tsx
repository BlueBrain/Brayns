import React, {PureComponent} from 'react';

import classNames from 'classnames';

import {Color} from '@material-ui/core';
import common from '@material-ui/core/colors/common';
import {
    createStyles,
    Theme,
    WithStyles,
    withStyles
} from '@material-ui/core/styles';

import {MATERIAL_PALETTES} from './constants';


export const size = 22;
const bubbleInnerSize = 16;
const bubbleInnerPosition = (size - bubbleInnerSize) / 2;

const styles = (theme: Theme) => createStyles({
    container: {
        width: (size + theme.spacing.unit) * 8 - theme.spacing.unit
    },
    content: {
        display: 'flex',
        flexWrap: 'wrap',
        marginLeft: -theme.spacing.unit / 2,
        marginRight: -theme.spacing.unit / 2
    },
    bubble: {
        position: 'relative',
        width: size,
        height: size,
        borderRadius: '50%',
        margin: theme.spacing.unit / 2,
        cursor: 'pointer',
        transform: 'scale(1)',
        transition: theme.transitions.create('transform', {
            easing: theme.transitions.easing.sharp,
            duration: theme.transitions.duration.leavingScreen
        })
    },
    bubbleShift: {
        transform: 'scale(1.2)',
        transition: theme.transitions.create('transform', {
            easing: theme.transitions.easing.easeOut,
            duration: theme.transitions.duration.enteringScreen
        })
    },
    bubbleInner: {
        position: 'absolute',
        width: bubbleInnerSize,
        height: bubbleInnerSize,
        top: bubbleInnerPosition,
        left: bubbleInnerPosition,
        borderRadius: '50%',
        backgroundColor: common.white,
        transform: 'scale(0)',
        transition: theme.transitions.create('transform', {
            easing: theme.transitions.easing.sharp,
            duration: theme.transitions.duration.leavingScreen
        })
    },
    bubbleInnerShift: {
        transform: 'scale(1)',
        transition: theme.transitions.create('transform', {
            easing: theme.transitions.easing.easeOut,
            duration: theme.transitions.duration.enteringScreen
        })
    }
});

// https://material-ui-next.com/guides/typescript/
const style = withStyles(styles);

export class Swatches extends PureComponent<Props, State> {
    state: State = {};

    setFocus = (color: string) => () => {
        this.setState({
            inFocus: color
        });
    }
    unsetFocus = () => {
        this.setState({
            inFocus: undefined
        });
    }

    select = (color: string) => () => {
        const {onChange} = this.props;
        if (onChange) {
            onChange(color);
        }
    }

    render() {
        const {classes, value} = this.props;
        const {inFocus} = this.state;
        const swatches = MATERIAL_PALETTES.map(primaryColor)
            .map(renderSwatches(classes, this.setFocus, this.unsetFocus, this.select, value, inFocus));
        return (
            <div className={classes.container}>
                <div className={classes.content}>
                    {swatches}
                </div>
            </div>
        );
    }
}

export default style(Swatches);


function renderSwatches(
    classes: Record<any, string>,
    setFocus: (color: string) => () => void,
    unsetFocus: () => void,
    select: (color: string) => () => void,
    value?: string,
    inFocus?: string
) {
    return (color: string) => {
        const isInFocus = inFocus === color;
        const isSelected = color === value;
        const state = {
            [classes.bubbleShift]: isInFocus || isSelected
        };

        return (
            <div
                className={classNames(classes.bubble, state)}
                key={color}
                style={{backgroundColor: color}}
                onMouseEnter={setFocus(color)}
                onMouseLeave={unsetFocus}
                onClick={select(color)}
            >
                <div className={classNames(classes.bubbleInner, {[classes.bubbleInnerShift]: isSelected})} />
            </div>
        );
    };
}

function primaryColor(palette: Color): string {
    const color = palette['500'];
    return `${color}`;
}


interface Props extends WithStyles<typeof styles> {
    value?: string;
    onChange?(hex?: string): void;
}

interface State {
    inFocus?: string;
}
