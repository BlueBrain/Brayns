// tslint:disable: no-empty
import React, {
    ChangeEvent,
    PureComponent
} from 'react';

import chroma, {
    Color,
    distance
} from 'chroma-js';
import classNames from 'classnames';

import FormControl, {FormControlProps} from '@material-ui/core/FormControl';
import IconButton from '@material-ui/core/IconButton';
import Input from '@material-ui/core/Input';
import InputAdornment from '@material-ui/core/InputAdornment';
import InputLabel from '@material-ui/core/InputLabel';
import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';

import {dispatchKeyboardLock} from '../../events';

import ColorPicker from '../color-picker';
import {FormControlPropsKeys} from './types';


const BUBBLE_SIZE = 21;

const styles = (theme: Theme) => createStyles({
    colorBubble: {
        display: 'block',
        width: BUBBLE_SIZE,
        height: BUBBLE_SIZE,
        border: `1px solid ${theme.palette.text.primary}`,
        borderRadius: '50%'
    },
    disabled: {
        opacity: .5
    }
});

const style = withStyles(styles);


export class ColorField extends PureComponent<Props, State> {
    static getDerivedStateFromProps(nextProps: Readonly<Props>, prevState: Readonly<State>) {
        const {value} = nextProps;

        if (Array.isArray(value)) {
            const color = chroma(value);

            if (!compareRgb(color, prevState.value)) {
                return {
                    value,
                    rgb: value,
                    hex: color.hex()
                };
            }
        }

        return null;
    }

    state: State = {
        rgb: [0, 0, 0],
        value: [0, 0, 0],
        hex: '#000'
    };

    openColorPicker = () => {
        this.setState({
            showColorPicker: true
        });
    }
    closeColorPicker = () => {
        this.setState({
            showColorPicker: false
        });
    }

    updateFromInput = (evt: ChangeEvent<HTMLInputElement>) => {
        const hexStr = addHash(evt.target.value);

        try {
            const color = chroma(hexStr);
            this.setState({
                hex: hexStr,
                rgb: color.rgb()
            });
        } catch (e) {
            this.setState({
                hex: hexStr
            });
        }
    }
    updateOnBlur = (evt: ChangeEvent<any>) => {
        const {value} = evt.target;

        try {
            const hex = chroma(value);
            if (!compareRgb(hex, this.props.value)) {
                this.updateColor(hex.rgb());
            }
        } catch {}
    }

    updateColor = (color: number[]) => {
        const {onChange} = this.props;
        if (onChange) {
            onChange(color);
        }
    }

    lockKeyboard = () => {
        dispatchKeyboardLock(true);
    }
    unlockKeyboard = () => {
        dispatchKeyboardLock(false);
    }

    render() {
        const {
            classes,
            id,
            label,
            ariaLabel,
            onChange,
            ...formControlProps
        } = this.props;
        const {rgb, hex, showColorPicker} = this.state;

        const bubbleStyle = {backgroundColor: hex};
        const colorPickerAdornment = (
            <InputAdornment position="end">
                <IconButton
                    aria-label={ariaLabel}
                    onClick={this.openColorPicker}
                    disabled={formControlProps.disabled}
                >
                    <span
                        className={classNames(classes.colorBubble, {[classes.disabled]: formControlProps.disabled})}
                        style={bubbleStyle}
                    />
                </IconButton>
            </InputAdornment>
        );

        const transitionProps = {
            onEnter: this.lockKeyboard,
            onExited: this.unlockKeyboard
        };

        return (
            <div>
                <FormControl key="color-input" {...formControlProps}>
                    <InputLabel htmlFor={id}>{label}</InputLabel>
                    <Input
                        id={id}
                        type="text"
                        onChange={this.updateFromInput}
                        onBlur={this.updateOnBlur}
                        value={hex}
                        endAdornment={colorPickerAdornment}
                    />
                </FormControl>
                <ColorPicker
                    key="color-picker"
                    open={!!showColorPicker}
                    onClose={this.closeColorPicker}
                    value={rgb}
                    TransitionProps={transitionProps}
                    onChange={this.updateColor}
                />
            </div>
        );
    }
}

export default style(ColorField);


function addHash(str: string) {
    const hasHash = str.indexOf('#') === 0;
    if (hasHash) {
        return str;
    }
    return `#${str}`;
}

function compareRgb(a: Color, b?: number[]) {
    if (!b) {
        return false;
    }
    const d: number = distance(a, chroma(b), 'rgb') as any;
    return d === 0;
}


interface Props extends Pick<FormControlProps, FormControlPropsKeys>, WithStyles<typeof styles> {
    id?: string;
    label?: string;
    ariaLabel?: string;
    value?: number[];
    onChange?(color: number[]): void;
}

interface State {
    rgb: number[];
    value: number[];
    hex: string;
    showColorPicker?: boolean;
}
