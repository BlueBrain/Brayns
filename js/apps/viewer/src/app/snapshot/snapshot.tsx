// tslint:disable: member-ordering
import React, {
    ChangeEvent,
    Component
} from 'react';

import {ImageFormat, SNAPSHOT} from 'brayns';
import classNames from 'classnames';
import {isNumber} from 'lodash';
import saveFile from 'save-as-file';

import Button from '@material-ui/core/Button';
import Checkbox from '@material-ui/core/Checkbox';
import Dialog from '@material-ui/core/Dialog';
import DialogActions from '@material-ui/core/DialogActions';
import DialogContent from '@material-ui/core/DialogContent';
import DialogTitle from '@material-ui/core/DialogTitle';
import FormControl from '@material-ui/core/FormControl';
import FormControlLabel from '@material-ui/core/FormControlLabel';
import FormGroup from '@material-ui/core/FormGroup';
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
import withWidth, {isWidthDown, WithWidth} from '@material-ui/core/withWidth';

import brayns, {
    withAnimation,
    WithAnimation,
    withCamera,
    WithCamera,
    withConnectionStatus,
    WithConnectionStatus,
    withRenderer,
    WithRenderer
} from '../../common/client';
import {NumericField, SlideUp} from '../../common/components';
import {APP_BAR_HEIGHT, APP_BAR_HEIGHT_XS} from '../../common/constants';
import {
    dispatchKeyboardLock,
    dispatchRequest,
    onRequestCancel
} from '../../common/events';


const DIALOG_TITLE_ID = 'snapshot-dialog-title';

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
        ...getViewportSize(this.props.width),
        data: undefined,
        quality: 100,
        samplesPerPixel: 8,
        filename: '',
        format: ImageFormat.Jpeg,
        useViewport: true
    };

    takeSnapshot = async () => {
        const {
            filename,
            quality,
            samplesPerPixel,
            format
        } = this.state;

        const {width, height} = getSnapshotSize(this.state, this.props.width);
        const size = [width, height];

        dispatchKeyboardLock(false);
        this.props.onClose();

        const snapshot = brayns.request(SNAPSHOT, {
            format,
            quality,
            samplesPerPixel,
            size,
            camera: this.props.camera,
            renderer: this.props.renderer,
            animationParameters: this.props.animationParams,
            name: `${filename}.${format}`
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
        } catch {} // tslint:disable-line: no-empty

        sub.unsubscribe();
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
        } as any);
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

    toggleUseViewport = (evt: ChangeEvent<HTMLInputElement>, useViewport: boolean) => {
        this.setState({useViewport});
    }

    render() {
        const {
            online,
            classes,
            width: screenWidth,
            open,
            onClose
        } = this.props;
        const {
            filename,
            samplesPerPixel,
            format,
            quality,
            useViewport
        } = this.state;

        const fullScreen = isWidthDown('xs', screenWidth);

        const {width, height} = getSnapshotSize(this.state, screenWidth);

        const offline = !online;
        const hasFilenameError = !filename || !filename.length;
        const isGenerateProhibited = offline || hasFilenameError;

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

        const checkbox = (
            <Checkbox
                checked={useViewport}
                onChange={this.toggleUseViewport}
            />
        );

        return (
            <Dialog
                open={open}
                classes={{paper: classes.dialog}}
                onClose={onClose}
                TransitionComponent={SlideUp}
                aria-labelledby={DIALOG_TITLE_ID}
                fullScreen={fullScreen}
            >
                <DialogTitle id={DIALOG_TITLE_ID}>Snapshot</DialogTitle>
                <DialogContent>
                    <FormGroup row>
                        <NumericField
                            label="Width"
                            className={classNames(classes.flex, classes.marginRight)}
                            value={width}
                            onChange={this.updateWidth}
                            type="integer"
                            min={sizeInputProps.min}
                            max={sizeInputProps.max}
                            step={sizeInputProps.step}
                            margin="normal"
                            disabled={useViewport}
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
                            disabled={useViewport}
                            noSlider
                        />
                    </FormGroup>
                    <FormGroup row>
                        <FormControlLabel
                            control={checkbox}
                            label="Use current viewport size"
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
                    <Button onClick={onClose}>
                        Cancel
                    </Button>
                    <Button onClick={this.takeSnapshot} color="primary" disabled={isGenerateProhibited}>
                        Take
                    </Button>
                </DialogActions>
            </Dialog>
        );
    }
}

export default withWidth()(
    style(
        withConnectionStatus(
            withRenderer(
                withCamera(
                    withAnimation(Snapshot))))));


function setMimeType(data: string, format?: ImageFormat) {
    const type = `image/${format || ImageFormat.Jpeg}`;
    if (data.indexOf(type) === -1) {
        return `data:image/${format || ImageFormat.Jpeg};base64,${data}`;
    }
    return data;
}

function getSnapshotSize(state: State, screenWidth: Breakpoint): SnapshotSize {
    return state.useViewport ? getViewportSize(screenWidth) : {
        width: state.width,
        height: state.height
    };
}

function getViewportSize(currentWidth: Breakpoint): SnapshotSize {
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


interface Props extends WithStyles<typeof styles>,
    WithWidth,
    WithConnectionStatus,
    WithCamera,
    WithRenderer,
    WithAnimation {
    open: boolean;
    onClose(): void;
}

interface State {
    width: number;
    height: number;
    data?: string;
    filename?: string;
    samplesPerPixel?: number;
    format?: ImageFormat;
    quality?: number;
    useViewport?: boolean;
}

interface SnapshotSize {
    width: number;
    height: number;
}
