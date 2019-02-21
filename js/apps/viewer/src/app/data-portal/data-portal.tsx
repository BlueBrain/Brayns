// tslint:disable: object-literal-key-quotes
import React, {
    createRef,
    Fragment,
    KeyboardEvent,
    PureComponent,
    RefObject
} from 'react';
import Dropzone, {DropzoneRenderArgs} from 'react-dropzone';

import {BinaryParams} from 'brayns';
import classNames from 'classnames';
import {fromEvent} from 'file-selector';
import {noop} from 'lodash';
import {Subscription} from 'rxjs';

import Button from '@material-ui/core/Button';
import Dialog from '@material-ui/core/Dialog';
import DialogActions from '@material-ui/core/DialogActions';
import Fade from '@material-ui/core/Fade';
import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';
import Typography from '@material-ui/core/Typography';
import withWidth, {isWidthDown, WithWidth} from '@material-ui/core/withWidth';
import InsertDriveFileIcon from '@material-ui/icons/InsertDriveFile';

import brayns, {
    withConnectionStatus,
    WithConnectionStatus,
    withLoaders,
    WithLoaders
} from '../../common/client';
import {SlideUp} from '../../common/components';
import {KeyCode} from '../../common/constants';
import {
    dispatchKeyboardLock,
    dispatchRequest,
    onRequestCancel
} from '../../common/events';

import ModelList, {ModelWithLoader} from './model-list';
import {
    defaultProps,
    findLoader,
    needsUserInput
} from './utils';


const MAX_CHUNK_SIZE = 1024 * 500; // 500kb


const styles = (theme: Theme) => createStyles({
    container: {
        position: 'relative',
        outline: 'none'
    },
    overlay: {
        position: 'absolute',
        display: 'flex',
        flexDirection: 'column',
        alignItems: 'center',
        justifyContent: 'center',
        left: 0,
        top: 0,
        right: 0,
        bottom: 0,
        backgroundColor: 'rgba(0, 0, 0, .4)',
        color: theme.palette.common.white,
        textShadow: '0 1px 2px rgba(0, 0, 0, .4)',
        zIndex: 1024 * 1024
    },
    // https://caniuse.com/#feat=css-grid
    // https://developer.mozilla.org/en-US/docs/Web/CSS/CSS_Grid_Layout/Box_Alignment_in_CSS_Grid_Layout#Center_an_item_in_the_area
    dialogContainer: {
        [theme.breakpoints.up('sm')]: {
            minHeight: '100vh',
            maxHeight: '100vh',
            height: 'initial',
            display: 'grid',
            gridTemplateColumns: 'repeat(4, 1fr)',
            gridTemplateAreas: `
                ". a a ."
                ". a a ."
            `
        }
    },
    dialog: {
        borderRadius: 0,
        backgroundColor: 'transparent',
        [theme.breakpoints.up('sm')]: {
            width: 420,
            gridArea: 'a',
            alignSelf: 'center',
            justifySelf: 'center'
        }
    },
    dialogActions: {
        margin: 0,
        marginTop: theme.spacing.unit * 2
    },
    dialogAction: {
        '&:last-child': {
            marginRight: theme.spacing.unit
        }
    },
    cancel: {
        color: theme.palette.type === 'dark' ? theme.palette.text.primary : theme.palette.background.paper
    },
    gutterBottom: {
        marginBottom: theme.spacing.unit
    }
});

const style = withStyles(styles);


class DataPortal extends PureComponent<Props, State> {
    state: State = {
        models: []
    };

    dropzoneRef: RefObject<Dropzone> = createRef();
    get dropzone() {
        const dropzone = this.dropzoneRef.current;
        return dropzone;
    }

    onClose = () => {
        const {onClose} = this.props;
        if (onClose) {
            onClose();
        }
    }

    import = async (files: File[]) => {
        const {loaders} = this.props;

        // Notify file select close
        this.onClose();

        if (needsUserInput(files, loaders!)) {
            dispatchKeyboardLock(true);

            const models = {
                models: files.map(file => {
                    const name = findLoader(file, loaders!);
                    return {
                        file,
                        name,
                        properties: defaultProps(name, loaders!)
                    };
                })
            };
            this.setState({
                ...models,
                openLoaderProps: true
            });
        } else {
            const items = files.map(file => {
                const loaderName = findLoader(file, loaders!);
                return {
                    file,
                    loaderName,
                    loaderProperties: defaultProps(loaderName, loaders!),
                    chunkSize: Math.min(...[
                        file.size,
                        MAX_CHUNK_SIZE
                    ])
                };
            });

            this.upload(items);
        }
    }

    closeLoaderProps = () => {
        dispatchKeyboardLock(false);
        this.setState({openLoaderProps: false});
    }

    clearModels = () => {
        this.setState({
            models: []
        });
    }

    updateModelPropsList = (models: ModelWithLoader[]) => {
        this.setState({models});
    }

    uploadWithProps = () => {
        const {models} = this.state;

        const items = models.map(({file, name, properties}) => ({
            file,
            loaderName: name,
            loaderProperties: properties,
            chunkSize: Math.min(...[
                file.size,
                MAX_CHUNK_SIZE
            ])
        }));

        this.upload(items);
        this.closeLoaderProps();
    }

    upload = async (items: BinaryParams[]) => {
        const tasks: Map<PromiseLike<void>, Subscription> = new Map();

        for (const item of items) {
            const upload = brayns.upload(item);

            const sub = onRequestCancel(upload)
                .subscribe(() => {
                    upload.cancel();
                });

            dispatchRequest(upload);

            const p = upload.then(noop, noop);
            tasks.set(p, sub);
        }

        await Promise.all(tasks.keys());
        for (const sub of tasks.values()) {
            sub.unsubscribe();
        }
        tasks.clear();
    }

    componentDidUpdate(prevProps: Readonly<Props>) {
        // Open the file dialog
        const {open} = this.props;
        if (open !== prevProps.open && open && this.dropzone) {
            this.dropzone.open();
        }
    }

    render() {
        const {
            children,
            classes,
            className,
            online,
            width
        } = this.props;
        const {openLoaderProps, models} = this.state;

        const fullScreen = isWidthDown('xs', width);
        const canUpload = online && models.every(m => m.name.length > 0);

        const dropzone = ({getRootProps, getInputProps, isDragActive}: DropzoneRenderArgs) => (
            <div {...getRootProps({className: classNames(classes.container, className)})}>
                <input {...getInputProps()} />

                <Fade
                    in={isDragActive}
                    unmountOnExit
                    timeout={{enter: 150, exit: 50}}
                >
                    <div className={classes.overlay}>
                        <div className={classes.gutterBottom}>
                            <InsertDriveFileIcon style={{fontSize: 26}} />
                        </div>
                        <Typography variant="h6" color="inherit" gutterBottom>Drop files/folders here to upload</Typography>
                        <Typography variant="caption" color="inherit">Use an archive for meshes with materials</Typography>
                    </div>
                </Fade>

                {children}
            </div>
        );

        return (
            <Fragment>
                <Dropzone
                    key="data-portal"
                    ref={this.dropzoneRef}
                    getDataTransferItems={fromEvent}
                    onDrop={this.import}
                    onKeyDown={preventDefault}
                    onFileDialogCancel={this.onClose}
                    disabled={!online}
                    multiple
                    disableClick
                >
                    {dropzone}
                </Dropzone>

                <Dialog
                    open={!!openLoaderProps}
                    classes={{container: classes.dialogContainer, paper: classes.dialog}}
                    onClose={this.closeLoaderProps}
                    TransitionComponent={SlideUp}
                    TransitionProps={{onExited: this.clearModels}}
                    fullScreen={fullScreen}
                    PaperProps={{elevation: 0}}
                    scroll="body"
                >
                    <ModelList
                        models={models}
                        onChange={this.updateModelPropsList}
                    />
                    <DialogActions classes={{root: classes.dialogActions, action: classes.dialogAction}}>
                        <Button
                            className={classes.cancel}
                            color="inherit"
                            onClick={this.closeLoaderProps}
                        >
                            Cancel
                        </Button>
                        <Button
                            onClick={this.uploadWithProps}
                            variant="contained"
                            color="primary"
                            disabled={!canUpload}
                        >
                            Upload
                        </Button>
                    </DialogActions>
                </Dialog>
            </Fragment>
        );
    }
}

export default withWidth()(
    style(
        withConnectionStatus(
            withLoaders(DataPortal))));


function preventDefault(evt: KeyboardEvent) {
    if (evt.keyCode === KeyCode.Space || evt.keyCode === KeyCode.Enter) {
        evt.preventDefault();
    }
}


interface Props extends WithStyles<typeof styles>,
    WithWidth,
    WithConnectionStatus,
    WithLoaders {
    className?: string;
    open?: boolean;
    onClose?(): void;
}

interface State {
    models: ModelWithLoader[];
    openLoaderProps?: boolean;
}
