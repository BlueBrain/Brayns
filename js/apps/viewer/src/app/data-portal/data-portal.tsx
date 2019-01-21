// tslint:disable: object-literal-key-quotes
import React, {
    createRef,
    Fragment,
    PureComponent,
    RefObject
} from 'react';
import Dropzone from 'react-dropzone';

import {BinaryParams} from 'brayns';
import classNames from 'classnames';
import {fromEvent} from 'file-selector';
import {noop} from 'lodash';
import {Subscription} from 'rxjs';
import {mergeMap} from 'rxjs/operators';

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

import brayns, {onReady} from '../../common/client';
import {SlideUp} from '../../common/components';
import {
    dispatchKeyboardLock,
    dispatchNotification,
    dispatchRequest,
    onRequestCancel
} from '../../common/events';

import ModelList, {ModelWithLoader} from './model-list';
import {
    LoadersContext,
    Provider
} from './provider';
import {
    defaultProps,
    findLoader,
    getLoadersWithSchema,
    needsUserInput
} from './utils';


const MAX_CHUNK_SIZE = 1024 * 500; // 500kb


const styles = (theme: Theme) => createStyles({
    container: {
        position: 'relative'
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
    dialog: {
        borderRadius: 0,
        backgroundColor: 'transparent',
        [theme.breakpoints.up('sm')]: {
            minWidth: 400
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
    gutterBottom: {
        marginBottom: theme.spacing.unit
    }
});

const style = withStyles(styles);


class DataPortal extends PureComponent<Props, State> {
    state: State = {
        models: [],
        context: {
            loaders: []
        }
    };

    dropzoneRef: RefObject<Dropzone> = createRef();
    get dropzone() {
        const dropzone = this.dropzoneRef.current;
        return dropzone;
    }

    private subs: Subscription[] = [];

    onClose = () => {
        const {onClose} = this.props;
        if (onClose) {
            onClose();
        }
    }

    showDropHint = () => {
        this.setState({showDropHint: true});
    }
    hideDropHint = () => {
        this.setState({showDropHint: false});
    }

    import = async (files: File[]) => {
        const {context} = this.state;

        // Notify file select close
        this.onClose();

        if (needsUserInput(files, context)) {
            this.setState(state => {
                const models = {
                    models: files.map(file => {
                        const name = findLoader(file, context);
                        return {
                            file,
                            name,
                            properties: defaultProps(name, context.loaders!)
                        };
                    })
                };
                // If user added a file from native file select
                if (state.showDropHint) {
                    return {
                        ...models,
                        showDropHint: false
                    };
                } else {
                    return {
                        ...models,
                        openLoaderProps: true
                    };
                }
            });
        } else {
            const items = files.map(file => {
                const loaderName = findLoader(file, context);
                return {
                    file,
                    loaderName,
                    loaderProperties: defaultProps(loaderName, context.loaders!),
                    chunkSize: Math.min(...[
                        file.size,
                        MAX_CHUNK_SIZE
                    ])
                };
            });

            this.hideDropHint();
            this.upload(items);
        }
    }

    openLoaderProps = async () => {
        const {models} = this.state;
        if (!models.length) {
            return;
        }

        dispatchKeyboardLock(true);
        this.setState({openLoaderProps: true});
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

        // this.hideDropHint();
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

            const p = upload.then(noop, err => {
                dispatchNotification(err);
            });

            tasks.set(p, sub);
        }

        await Promise.all(tasks.keys());
        for (const sub of tasks.values()) {
            sub.unsubscribe();
        }
        tasks.clear();
    }

    componentDidMount() {
        this.subs.push(...[
            onReady()
                .pipe(mergeMap(() => getLoadersWithSchema()))
                .subscribe(loaders => {
                    this.setState({
                        context: {loaders}
                    });
                })
        ]);
    }

    componentDidUpdate(prevProps: Readonly<Props>) {
        // Open the file dialog
        const {open} = this.props;
        if (open !== prevProps.open && open && this.dropzone) {
            this.dropzone.open();
        }
    }

    componentWillUnmount() {
        while (this.subs.length) {
            const sub = this.subs.pop();
            sub!.unsubscribe();
        }
    }

    render() {
        const {
            children,
            classes,
            className,
            disabled,
            width
        } = this.props;
        const {
            showDropHint,
            openLoaderProps,
            models,
            context
        } = this.state;

        const fullScreen = isWidthDown('xs', width);
        const canUpload = !disabled && models.every(m => m.name.length > 0);

        return (
            <Fragment>
                <Dropzone
                    key="data-portal"
                    ref={this.dropzoneRef}
                    getDataTransferItems={fromEvent}
                    className={classNames(classes.container, className)}
                    onDragEnter={this.showDropHint}
                    onDragLeave={this.hideDropHint}
                    onDrop={this.import}
                    onFileDialogCancel={this.onClose}
                    disabled={disabled}
                    multiple
                    disableClick
                    disablePreview
                >
                    <Fade
                        in={showDropHint}
                        unmountOnExit
                        timeout={{enter: 150, exit: 50}}
                        onExited={this.openLoaderProps}
                    >
                        <div className={classes.overlay}>
                            <div className={classes.gutterBottom}>
                                <InsertDriveFileIcon style={{fontSize: 26}} />
                            </div>
                            <Typography variant="h6" color="inherit" gutterBottom>
                                Drop files/folders here to upload
                            </Typography>
                            <Typography variant="caption" color="inherit">
                                Use an archive for meshes with materials
                            </Typography>
                        </div>
                    </Fade>
                    {children}
                </Dropzone>

                <Dialog
                    open={!!openLoaderProps}
                    classes={{paper: classes.dialog}}
                    onClose={this.closeLoaderProps}
                    TransitionComponent={SlideUp}
                    TransitionProps={{onExited: this.clearModels}}
                    fullScreen={fullScreen}
                    PaperProps={{elevation: 0}}
                >
                    <Provider value={context}>
                        <ModelList
                            models={models}
                            onChange={this.updateModelPropsList}
                        />
                    </Provider>
                    <DialogActions classes={{root: classes.dialogActions, action: classes.dialogAction}}>
                        <Button onClick={this.closeLoaderProps}>
                            Cancel
                        </Button>
                        <Button
                            onClick={this.uploadWithProps}
                            variant="contained"
                            color="secondary"
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
    style(DataPortal)
);


interface Props extends WithStyles<typeof styles>, WithWidth {
    className?: string;
    open?: boolean;
    disabled?: boolean;
    onClose?(): void;
}

interface State {
    models: ModelWithLoader[];
    showDropHint?: boolean;
    openLoaderProps?: boolean;
    context: LoadersContext;
}
