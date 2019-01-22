import React, {PureComponent} from 'react';

import chroma from 'chroma-js';
import {Subject, Subscription} from 'rxjs';
import {debounceTime} from 'rxjs/operators';

import Dialog from '@material-ui/core/Dialog';
import DialogContent from '@material-ui/core/DialogContent';
import {
    createStyles,
    Theme,
    WithStyles,
    withStyles
} from '@material-ui/core/styles';

import Vector3Field, {Vector3Props} from '../fields/vector3-field';
import {SlideUp} from '../transitions/slide';
import Saturation from './saturation';
import Swatches from './swatches';
import {compareColors, toHsv} from './utils';


const styles = (theme: Theme) => createStyles({
    dialog: {
        maxWidth: 280
    },
    saturation: {
       height: 200,
       marginBottom: theme.spacing.unit * 2,
       overflow: 'hidden'
    },
    rgb: {
        marginBottom: theme.spacing.unit * 2
    },
    swatches: {}
});

const style = withStyles(styles);


// https://casesandberg.github.io/react-color/
// https://bennyalex.github.io/material-design-inspired-color-picker/
export class ColorPicker extends PureComponent<Props, State> {
    state: State = {
        ...stateFromRgb(this.props.value),
        rgbFieldProps: {
            label: ['R', 'G', 'B'],
            min: [0, 0, 0],
            max: [255, 255, 255],
            step: [1, 1, 1]
        }
    };

    private rgb = new Subject<number[]>();
    private sub?: Subscription;

    updateRgb = (rgb: number[]) => {
        this.update(stateFromRgb(rgb));
    }

    updateHsv = (hsv: number[]) => {
        this.update(stateFromHsv(hsv));
    }

    updateHex = (hex: string) => {
        this.update(stateFromHex(hex));
    }

    update = (state: Pick<State, 'hex' | 'rgb' | 'hsv'>) => {
        this.setState(state, () => {
            this.rgb.next(state.rgb);
        });
    }

    componentDidUpdate(prevProps: Readonly<Props>) {
        const {value} = this.props;
        if (Array.isArray(value)
            && !compareColors(value, prevProps.value)
            && !compareColors(value, this.state.rgb)) {
            this.setState(stateFromRgb(value));
        }
    }

    componentDidMount() {
        this.sub = this.rgb.pipe(debounceTime(250))
            .subscribe(rgb => {
                const {onChange} = this.props;
                if (onChange) {
                    onChange(rgb);
                }
            });
    }
    componentWillUnmount() {
        if (this.sub) {
            this.sub.unsubscribe();
        }
    }

    render() {
        const {
            classes,
            onClose,
            TransitionProps,
            open
        } = this.props;
        const {
            rgbFieldProps,
            hex,
            rgb,
            hsv
        } = this.state;

        return (
            <Dialog
                open={open}
                TransitionComponent={SlideUp}
                onClose={onClose}
                TransitionProps={TransitionProps}
                classes={{paper: classes.dialog}}
            >
                <Saturation
                    className={classes.saturation}
                    value={hsv}
                    onChange={this.updateHsv}
                />
                <DialogContent>
                    <div className={classes.rgb}>
                        <Vector3Field
                            value={rgb}
                            onChange={this.updateRgb}
                            {...rgbFieldProps}
                        />
                    </div>
                    <div className={classes.swatches}>
                        <Swatches
                            value={hex}
                            onChange={this.updateHex}
                        />
                    </div>
                </DialogContent>
            </Dialog>
        );
    }
}

export default style(ColorPicker);


function stateFromRgb(rgb: number[]) {
    const color = chroma(rgb);
    return {
        rgb,
        hex: color.hex(),
        hsv: toHsv(color)
    };
}

function stateFromHsv(hsv: number[]) {
    const color = chroma(hsv, 'hsv');
    return {
        hsv,
        rgb: color.rgb(),
        hex: color.hex()
    };
}

function stateFromHex(hex: string) {
    const color = chroma(hex);
    return {
        hex,
        rgb: color.rgb(),
        hsv: toHsv(color)
    };
}


interface Props extends WithStyles<typeof styles> {
    open: boolean;
    value: number[];
    TransitionProps?: any;
    onClose?(): void;
    onChange?(color: number[]): void;
}

interface State {
    rgbFieldProps: Partial<Vector3Props>;
    rgb: number[];
    hsv: number[];
    hex: string;
}
