import React from 'react';

import {Theme} from '@material-ui/core';

import {CanvasRect} from './provider';
import {ControlPoint} from './types';

export function getColor(theme: Theme) {
    return theme.palette.type === 'dark' ? theme.palette.grey.A100 : theme.palette.grey.A400;
}

export function createToRectScaler(rect?: CanvasRect) {
    return (point: ControlPoint) => ({
        ...point,
        x: toRectScale(point.x, 'width', rect),
        y: toRectScale(point.y, 'height', rect)
    });
}

function toRectScale(value: number, coord: keyof CanvasRect, rect?: CanvasRect) {
    if (rect) {
        return value * rect[coord];
    }
    return value;
}

export function getPointCoords(
    evt: MouseEvent | React.MouseEvent,
    rect: CanvasRect
) {
    const {width, height} = rect;
    let {x, y} = getRelativeCoords(evt, rect);

    if (x < 0) {
        x = 0;
    } else if (x > width) {
        x = width;
    }

    if (y < 0) {
        y = 0;
    } else if (y > height) {
        y = height;
    }

    return {
        x: x / width,
        y: y / height
    };
}

export function isPointOutsideCanvas(
    evt: MouseEvent | React.MouseEvent,
    rect: CanvasRect
) {
    const {width, height} = rect;
    const {x, y} = getRelativeCoords(evt, rect);

    if (x < 0 || x > width) {
        return true;
    }

    if (y < 0 || y > height) {
        return true;
    }

    return false;
}

function getRelativeCoords(
    evt: MouseEvent | React.MouseEvent,
    rect: CanvasRect
) {
    const {left, top} = rect;
    const x = evt.pageX - (left + window.pageXOffset);
    const y = evt.pageY - (top + window.pageYOffset);

    return {x, y};
}
