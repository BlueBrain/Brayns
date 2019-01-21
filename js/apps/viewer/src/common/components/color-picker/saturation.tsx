import classNames from 'classnames';
import React, {
    createRef,
    MouseEvent,
    PureComponent,
    RefObject,
    Touch,
    TouchEvent
} from 'react';

import {
    createStyles,
    Theme,
    WithStyles,
    withStyles
} from '@material-ui/core/styles';


const styles = (theme: Theme) => createStyles({
    root: {
        position: 'relative'
    },
    white: {
        position: 'absolute',
        top: 0,
        right: 0,
        bottom: 0,
        left: 0,
        background: 'linear-gradient(to right, #fff, rgba(255, 255, 255, 0))'
    },
    black: {
        position: 'absolute',
        top: 0,
        right: 0,
        bottom: 0,
        left: 0,
        background: 'linear-gradient(to top, #000, rgba(0, 0, 0, 0))'
    },
    pointer: {
        position: 'absolute',
        width: 12,
        height: 12,
        border: `1px solid ${theme.palette.text.primary}`,
        borderRadius: '50%',
        transform: 'translate(-6px, -6px)',
        cursor: 'pointer',
        willChange: 'left, top'
    }
});
const style = withStyles(styles);


export class Saturation extends PureComponent<Props> {
    containerRef: RefObject<HTMLDivElement> = createRef();
    get container() {
        const container = this.containerRef.current;
        return container;
    }

    addEventListeners = (evt: MouseEvent) => {
        document.addEventListener('mousemove', this.updateColor as any, false);
        document.addEventListener('mouseup', this.removeEventListeners, false);
        this.updateColor(evt);
    }
    removeEventListeners = () => {
        document.removeEventListener('mousemove', this.updateColor as any);
        document.removeEventListener('mouseup', this.removeEventListeners);
    }

    updateColor = (evt: MouseEvent | TouchEvent) => {
        const container = this.container;
        const {onChange} = this.props;
        const {value} = this.props;
        const [h] = value;
        if (container && onChange) {
            const {s, v} = calcSaturationAndBrightness(container, evt);
            const hsv = [h, s, v];
            onChange(hsv);
        }
    }

    render() {
        const {classes, className, value} = this.props;

        const [h, s, v] = value;
        const backgroundColor = getBackgroundColor(value);
        const position = getPosition([h, ...toPosition(s, v)] as any);

        return (
            <div
                ref={this.containerRef}
                onMouseDown={this.addEventListeners}
                onTouchStart={this.updateColor}
                onTouchMove={this.updateColor}
                style={{backgroundColor}}
                className={classNames(classes.root, className)}
            >
                <div className={classes.white}>
                    <div className={classes.black} />
                    <div className={classes.pointer} style={{...position}} />
                </div>
            </div>
        );
    }
}

export default style(Saturation);


export function calcSaturationAndBrightness(container: Element, evt: MouseEvent | TouchEvent) {
    const rect = container.getBoundingClientRect();
    const {width, height} = rect;
    const x = getEvtProp(evt, 'pageX');
    const y = getEvtProp(evt, 'pageY');

    let left = x - (rect.left + window.pageXOffset);
    let top = y - (rect.top + window.pageYOffset);

    if (left < 0) {
        left = 0;
    } else if (left > width) {
        left = width;
    }

    if (top < 0) {
        top = 0;
    } else if (top > height) {
        top = height;
    }

    const saturation = left / width;
    const brightness = -top / height + 1;

    return {
        s: saturation,
        v: brightness
    };
}

function toPosition(saturation: number, brightness: number) {
    return [saturation * 100, brightness * 100];
}

function getEvtProp(evt: MouseEvent | TouchEvent, key: keyof Touch) {
    if ((evt as TouchEvent).touches) {
        const touch = (evt as TouchEvent).touches[0];
        const value = (touch as any)[key];
        return value;
    }

    const value = (evt as any)[key];
    return value;
}

function getBackgroundColor(hsv: number[]) {
    const [h] = hsv;
    return `hsl(${h}, 100%, 50%)`;
}

function getPosition(hsv: number[]) {
    const [, s, v] = hsv;
    return {
        top: `${-v + 100}%`,
        left: `${s}%`
    };
}


interface Props extends WithStyles<typeof styles> {
    value: number[];
    className?: string;
    onChange?(hsv: number[]): void;
}
