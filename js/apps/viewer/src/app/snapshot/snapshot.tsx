// tslint:disable: member-ordering
import React, {ChangeEvent, Component} from 'react';

import {
    GET_ANIMATION_PARAMS,
    GET_RENDERER,
    ImageFormat,
    SNAPSHOT
} from 'brayns';
import classNames from 'classnames';
import {isNumber} from 'lodash';
import {Subscription} from 'rxjs';
import saveFile from 'save-as-file';

import {PropTypes} from '@material-ui/core';
import Button from '@material-ui/core/Button';
import Dialog from '@material-ui/core/Dialog';
import DialogActions from '@material-ui/core/DialogActions';
import DialogContent from '@material-ui/core/DialogContent';
import DialogTitle from '@material-ui/core/DialogTitle';
import FormControl from '@material-ui/core/FormControl';
import FormGroup from '@material-ui/core/FormGroup';
import IconButton from '@material-ui/core/IconButton';
import Input from '@material-ui/core/Input';
import InputLabel from '@material-ui/core/InputLabel';
import MenuItem from '@material-ui/core/MenuItem';
import Select from '@material-ui/core/Select';
import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';
import {Breakpoint} from '@material-ui/core/styles/createBreakpoints';
import TextField from '@material-ui/core/TextField';
import Tooltip from '@material-ui/core/Tooltip';
import withWidth, {isWidthDown, WithWidth} from '@material-ui/core/withWidth';
import PhotoCameraIcon from '@material-ui/icons/PhotoCamera';

import brayns, {withCamera, WithCamera} from '../../common/client';
import {NumericField, SlideUp} from '../../common/components';
import {
    APP_BAR_HEIGHT,
    APP_BAR_HEIGHT_XS,
    KeyCode,
    TOOLTIP_DELAY
} from '../../common/constants';
import {
    dispatchKeyboardLock,
    dispatchNotification,
    dispatchRequest,
    onKeyboardLockChange,
    onRequestCancel
} from '../../common/events';


const fileTypes = Object.values(ImageFormat)
    .map(type => <MenuItem key={type} value={type}>{type}</MenuItem>);

const styles = (theme: Theme) => createStyles({
    dialog: {
        [theme.breakpoints.up('xs')]: {
            minWidth: 400
        }
    },
    cancel: {
        width: theme.spacing.unit * 4,
        height: theme.spacing.unit * 4
    },
    marginRight: {
        marginRight: theme.spacing.unit * 2
    },
    gutter: {
        marginTop: theme.spacing.unit * 2,
        marginBottom: theme.spacing.unit
    },
    fields: {
        display: 'flex',
        alignItems: 'center',
        marginTop: theme.spacing.unit * 2,
        marginBottom: theme.spacing.unit
    },
    flex: {
        flex: 1
    }
});

const style = withStyles(styles);


export class Snapshot extends Component<Props, State> {
    state: State = {
        ...defaultSize(this.props.width),
        data: undefined,
        canShowSnackBar: true,
        showDialog: false,
        quality: 100,
        samplesPerPixel: 8,
        filename: '',
        format: ImageFormat.Jpeg
    };

    private keyboardLocked = false;
    private subs: Subscription[] = [];

    takeSnapshot = async () => {
        const {
            filename,
            width,
            height,
            quality,
            samplesPerPixel,
            format
        } = this.state;

        const [animationParameters, renderer] = await Promise.all([
            brayns.request(GET_ANIMATION_PARAMS),
            brayns.request(GET_RENDERER)
        ]);

        dispatchKeyboardLock(false);
        this.setState({
            showDialog: false
        });

        const snapshot = brayns.request(SNAPSHOT, {
            animationParameters,
            renderer,
            format,
            quality,
            samplesPerPixel,
            camera: this.props.camera,
            name: `${filename}.${format}`,
            size: [width, height] as number[]
        } as any);

        const sub = onRequestCancel(snapshot)
            .subscribe(() => {
                snapshot.cancel();
            });

        try {
            // Show progress in notification center
            dispatchRequest(snapshot);

            const response = await snapshot;

            const b64Str = setMimeType(response.data);
            const blob = await b64ToBlob(b64Str);

            // Save data as file
            saveFile(blob, `${filename}.${format}`);

            sub.unsubscribe();

            // Reset state
            this.reset();
        } catch (err) {
            dispatchNotification(err);
            sub.unsubscribe();
        }
    }
    reset = () => {
        // Reset the state
        this.setState({
            ...defaultSize(this.props.width),
            canShowSnackBar: true,
            filename: '',
            format: ImageFormat.Jpeg,
            quality: 100,
            samplesPerPixel: 8
        });
    }

    openDialog = () => {
        dispatchKeyboardLock(true);
        this.setState({
            ...defaultSize(this.props.width),
            showDialog: true
        });
    }
    openDialogOnKeydown = (evt: KeyboardEvent) => {
        const {disabled} = this.props;
        if (!disabled && evt.keyCode === KeyCode.S && evt.shiftKey && !this.keyboardLocked) {
            this.openDialog();
        }
    }
    closeDialog = () => {
        dispatchKeyboardLock(false);
        this.setState({
            showDialog: false
        });
    }

    closeSnackBar = () => {
        this.reset();
    }

    updateFileType = (evt: ChangeEvent<HTMLSelectElement>) => {
        this.setState({
            format: evt.target.value as ImageFormat
        });
    }
    updateFromInput = (key: keyof Pick<State, 'filename'>) => (evt: ChangeEvent<HTMLInputElement>) => {
        const {value} = evt.target;
        this.setState({
            [key]: value
        });
    }

    updateFilename = this.updateFromInput('filename');

    updateFromNumber = (key: keyof Pick<State, 'width' | 'height' | 'samplesPerPixel' | 'quality'>) => (value: number) => {
        this.setState({
            [key]: value
        });
    }

    updateWidth = this.updateFromNumber('width');
    updateHeight = this.updateFromNumber('height');
    updateSppx = this.updateFromNumber('samplesPerPixel');
    updateQuality = this.updateFromNumber('quality');

    updateFromSlider = (key: keyof Pick<State, 'quality'>) => (evt: ChangeEvent<HTMLInputElement>, value: number) => {
        this.setState({
            [key]: value
        });
    }

    componentDidMount() {
        window.addEventListener('keydown', this.openDialogOnKeydown, false);
        this.subs.push(...[
            onKeyboardLockChange()
                .subscribe(locked => {
                    this.keyboardLocked = locked;
                })
        ]);
    }
    componentWillUnmount() {
        window.removeEventListener('keydown', this.openDialogOnKeydown);
        while (this.subs.length) {
            const sub = this.subs.pop();
            sub!.unsubscribe();
        }
    }

    render() {
        const {
            color = 'default',
            disabled,
            classes,
            width
        } = this.props;
        const {
            showDialog,
            filename,
            samplesPerPixel,
            format,
            quality,
            width: snapshotWidth,
            height
        } = this.state;

        const fullScreen = isWidthDown('xs', width);

        const hasFilenameError = !filename || !filename.length;
        const isGenerateProhibited = disabled || hasFilenameError;

        const dialogTitle = 'Take a snapshot';
        const dialogTitleId = 'snapshot-dialog-title';

        const filenameHelperText = hasFilenameError ? 'Required' : '';

        const sizeInputProps = {
            min: 0,
            max: 100000,
            step: 1
        };

        let qualityField = null;
        if (format === ImageFormat.Jpeg || format === ImageFormat.Png) {
            qualityField = (
                <div>
                    <NumericField
                        label="Image quality"
                        value={quality}
                        onChange={this.updateQuality}
                        type="integer"
                        min={1}
                        max={100}
                        step={1}
                        margin="normal"
                        fullWidth
                    />
                </div>
            );
        }

        return (
            <div>
                <Tooltip title={dialogTitle} placement="bottom" {...TOOLTIP_DELAY}>
                    <div>
                        <IconButton
                            onClick={this.openDialog}
                            color={color}
                            aria-label={dialogTitle}
                            disabled={disabled}
                        >
                            <PhotoCameraIcon />
                        </IconButton>
                    </div>
                </Tooltip>
                <Dialog
                    open={showDialog as boolean}
                    classes={{paper: classes.dialog}}
                    onClose={this.closeDialog}
                    TransitionComponent={SlideUp}
                    aria-labelledby={dialogTitleId}
                    fullScreen={fullScreen}
                >
                    <DialogTitle id={dialogTitleId}>Snapshot</DialogTitle>
                    <DialogContent>
                        <FormGroup row>
                            <NumericField
                                label="Width"
                                className={classNames(classes.flex, classes.marginRight)}
                                value={snapshotWidth}
                                onChange={this.updateWidth}
                                type="integer"
                                min={sizeInputProps.min}
                                max={sizeInputProps.max}
                                step={sizeInputProps.step}
                                margin="normal"
                                noSlider
                            />

                            <NumericField
                                label="Height"
                                className={classes.flex}
                                value={height}
                                onChange={this.updateHeight}
                                type="integer"
                                min={sizeInputProps.min}
                                max={sizeInputProps.max}
                                step={sizeInputProps.step}
                                margin="normal"
                                noSlider
                            />
                        </FormGroup>
                        <FormGroup>
                            <NumericField
                                label="Samples"
                                value={samplesPerPixel}
                                onChange={this.updateSppx}
                                type="integer"
                                step={1}
                                margin="normal"
                                fullWidth
                            />
                        </FormGroup>
                        <FormGroup row>
                            <TextField
                                id="snapshot-filename"
                                className={classes.marginRight}
                                style={{flex: 3}}
                                type="text"
                                value={filename}
                                onChange={this.updateFilename}
                                margin="normal"
                                label="File name"
                                helperText={filenameHelperText}
                                error={hasFilenameError}
                                required
                            />
                            <FormControl className={classes.flex} margin="normal">
                                <InputLabel htmlFor="snapshot-format">Format</InputLabel>
                                <Select value={format} onChange={this.updateFileType} input={<Input id="snapshot-format" />}>
                                    {fileTypes}
                                </Select>
                            </FormControl>
                        </FormGroup>
                        {qualityField}
                    </DialogContent>
                    <DialogActions>
                        <Button onClick={this.closeDialog}>
                            Cancel
                        </Button>
                        <Button onClick={this.takeSnapshot} color="primary" disabled={isGenerateProhibited}>
                            Take
                        </Button>
                    </DialogActions>
                </Dialog>
            </div>
        );
    }
}

export default withWidth()(
    style(withCamera(Snapshot))
);


function setMimeType(data: string, format?: ImageFormat) {
    const type = `image/${format || ImageFormat.Jpeg}`;
    if (data.indexOf(type) === -1) {
        return `data:image/${format || ImageFormat.Jpeg};base64,${data}`;
    }
    return data;
}

function defaultSize(currentWidth: Breakpoint): {
    width: number;
    height: number;
} {
    const [width, height] = [
        window.innerWidth,
        window.innerHeight - (isWidthDown('xs', currentWidth) ? APP_BAR_HEIGHT_XS : APP_BAR_HEIGHT)
    ].map(toPixelRatio);

    return {
        width,
        height
    };
}


/**
 * Apply current device pixel ratio to a value
 * @param {number} value
 */
export function toPixelRatio(value: number): number {
    if (isNumber(window.devicePixelRatio)) {
        return value * devicePixelRatio;
    }
    return value;
}


/**
 * Convert base64 encoded string to Blob
 * Use Fetch API as it's faster and non-blocking
 * @see https://jsperf.com/base64-string-to-blob/1
 * @param b64Str
 */
function b64ToBlob(b64Str: string) {
    return fetch(b64Str)
        .then(res => res.blob());
}


interface Props extends WithStyles<typeof styles>, WithWidth, WithCamera {
    color?: PropTypes.Color;
    disabled?: boolean;
}

interface State {
    data?: string;
    showDialog?: boolean;
    canShowSnackBar?: boolean;
    filename?: string;
    width?: number;
    height?: number;
    samplesPerPixel?: number;
    format?: ImageFormat;
    quality?: number;
}
