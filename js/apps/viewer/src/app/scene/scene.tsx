import React, {
    ChangeEvent,
    Fragment,
    PureComponent
} from 'react';
import SwipeableViews from 'react-swipeable-views';

import {
    ADD_CLIP_PLANE,
    BoundingBox,
    ClipPlane,
    GET_CLIP_PLANES,
    GET_SCENE,
    Model,
    Plane,
    REMOVE_CLIP_PLANES,
    REMOVE_MODEL,
    SET_SCENE,
    UPDATE_CLIP_PLANE,
    UPDATE_MODEL
} from 'brayns';

import classNames from 'classnames';
import {pick} from 'lodash';
import {Subscription} from 'rxjs';
import {filter, mergeMap} from 'rxjs/operators';

import AppBar from '@material-ui/core/AppBar';
import Drawer, {DrawerClassKey, DrawerProps} from '@material-ui/core/Drawer';
import Fab from '@material-ui/core/Fab';
import Fade from '@material-ui/core/Fade';
import IconButton from '@material-ui/core/IconButton';
import Slide, {SlideProps} from '@material-ui/core/Slide';
import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';
import {ClassNameMap} from '@material-ui/core/styles/withStyles';
import Tab from '@material-ui/core/Tab';
import Tabs from '@material-ui/core/Tabs';
import Tooltip from '@material-ui/core/Tooltip';
import Typography from '@material-ui/core/Typography';
import Zoom from '@material-ui/core/Zoom';
import AddIcon from '@material-ui/icons/Add';
import ArrowBackIcon from '@material-ui/icons/ArrowBack';
import ChevronLeftIcon from '@material-ui/icons/ChevronLeft';

import brayns, {
    onReady,
    WithCamera,
    withCamera
} from '../../common/client';
import {KeyCode, TOOLTIP_DELAY} from '../../common/constants';
import {
    dispatchKeyboardLock,
    dispatchNotification,
    onKeyboardLockChange
} from '../../common/events';
import storage from '../../common/storage';

import ClipPlaneActions from './clip-plane-actions';
import ClipPlaneEditor from './clip-plane-editor';
import ClipPlanes from './clip-planes';
import ModelActions from './model-actions';
import ModelEditor from './model-editor';
import ModelMetadata from './model-metadata';
import Models from './models';
import {ModelId, ModelProps} from './types';


const SELECTED_TAB_KEY = 'sceneTab';

const TOOLBAR_HEIGHT = 48;

const MODELS_TAB_IDX = 0;
const CLIP_PLANES_TAB_IDX = 1;


const styles = (theme: Theme) => {
    const toolbar = {
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'flex-end',
        padding: `0 ${theme.spacing.unit * 2}px`,
        ...theme.mixins.toolbar
    };

    return createStyles({
        mainToolbar: {
            position: 'sticky',
            top: 0,
            left: 0,
            zIndex: 2,
            ...toolbar
        },
        mainToolbarActions: {
            position: 'absolute',
            top: 0,
            right: 0,
            left: 0,
            bottom: 0,
            ...toolbar
        },
        mainToolbarText: {
            maxWidth: 250,
            textOverflow: 'ellipsis',
            overflow: 'hidden',
            whiteSpace: 'nowrap'
        },
        secondaryToolbar: {
            height: TOOLBAR_HEIGHT
        },
        back: {
            marginRight: theme.spacing.unit * 2
        },
        editToolbar: {
            backgroundColor: theme.palette.type === 'dark'
                ? theme.palette.grey['900']
                : theme.palette.grey['100']
        },
        content: {
            position: 'relative',
            flex: 1
        },
        editor: {
            position: 'absolute',
            top: 0,
            right: 0,
            bottom: 0,
            left: 0
        },
        clipPlaneEditor: {
            display: 'flex'
        },
        addClipPlane: {
            position: 'absolute',
            bottom: theme.spacing.unit * 2,
            right: theme.spacing.unit * 2
        },
        spacer: {
            flex: 1
        }
    });
};

const style = withStyles(styles, {
    withTheme: true
});


// TODO: Remove keyboard interaction logic as it affects perf
// Maybe just listen to keyboard enable/disable and do not react to keyboard events if disabled
export class SceneModels extends PureComponent<Props, State> {
    state: State = {
        tab: storage.get<number>(SELECTED_TAB_KEY) || MODELS_TAB_IDX,
        models: [],
        selectedModels: [],
        showModelEditor: false,
        showModelInfo: false,
        clipPlanes: [],
        selectedClipPlanes: [],
        showClipPlaneEditor: false
    };

    private keyboardLocked = false;
    private subs: Subscription[] = [];


    onTabChange = (evt: ChangeEvent<{}>, tab: number) => {
        storage.set(SELECTED_TAB_KEY, tab);
        this.setState({tab});
    }
    onSwipe = (index: number) => {
        storage.set(SELECTED_TAB_KEY, index);
        this.setState({
            tab: index
        });
    }


    /**
     * Models
     */

    focusModel = async (box: BoundingBox) => {
        this.props.onReset!(box);
    }

    updateModel = async (updates: Partial<ModelProps>) => {
        const model = this.state.model;
        if (model) {
            try {
                await brayns.request(UPDATE_MODEL, {
                    ...updates,
                    id: model.id
                });
            } catch (err) {
                dispatchNotification(err);
            }
        }
    }

    deleteModel = async (id: ModelId) => {
        try {
            await brayns.request(REMOVE_MODEL, [id]);
        } catch (err) {
            dispatchNotification(err);
        }
    }

    updateModelSelection = (id: ModelId) => {
        const clone = this.state.selectedModels.slice(0);

        const index = clone.indexOf(id);
        if (index !== -1) {
            clone.splice(index, 1);
        } else {
            clone.push(id);
        }

        this.setState({
            selectedModels: clone
        });
    }

    removeModels = async () => {
        const {selectedModels} = this.state;
        if (selectedModels.length) {
            try {
                await brayns.request(REMOVE_MODEL, selectedModels);
                this.setState({
                    selectedModels: []
                });
            } catch (err) {
                dispatchNotification(err);
            }
        }
    }

    setModelVisibility = async (id: ModelId, visible: boolean) => {
        try {
            await brayns.request(UPDATE_MODEL, {id, visible});
        } catch (err) {
            dispatchNotification(err);
        }
    }

    setSelectedModelsVisibility = async (visible: boolean) => {
        const {selectedModels} = this.state;
        if (selectedModels.length) {
            try {
                await Promise.all(selectedModels.map(id => brayns.request(UPDATE_MODEL, {
                    id,
                    visible
                })));
            } catch (err) {
                dispatchNotification(err);
            }
        }
    }

    showModelEditor = (id: ModelId) => {
        const {models} = this.state;
        const model = models.find(model => model.id === id);

        this.setState({
            model,
            showModelEditor: true
        });
    }

    closeModelEditor = () => {
        this.setState({
            showModelEditor: false
        });
    }

    showModelInfo = (id: ModelId) => {
        const {models} = this.state;
        const model = models.find(model => model.id === id);

        dispatchKeyboardLock(true);

        this.setState({
            model,
            showModelInfo: true
        });
    }

    closeModelInfo = () => {
        dispatchKeyboardLock(false);
        this.setState({
            showModelInfo: false
        });
    }

    clearModel = () => {
        this.setState({
            model: undefined
        });
    }

    clearModelSelection = () => {
        this.setState({
            selectedModels: []
        });
    }


    /**
     * Clip planes
     */

    addClipPlane = async () => {
        try {
            const plane: Plane = [0, 0, 0, 0];
            const cp = await brayns.request(ADD_CLIP_PLANE, plane);

            this.setState(state => {
                const copy = [...state.clipPlanes];
                copy.push(cp);
                return {
                    clipPlanes: copy
                };
            });
        } catch (err) {
            dispatchNotification(err);
        }
    }

    updateClipPlane = async (clipPlane: ClipPlane) => this.setState(state => {
        const copy = [...state.clipPlanes];
        const index = copy.findIndex(item => item.id === clipPlane.id);
        if (index !== -1) {
            copy.splice(index, 1, clipPlane);
        }
        return {
            clipPlane,
            clipPlanes: copy
        };
    }, async () => {
        try {
            await brayns.request(UPDATE_CLIP_PLANE, clipPlane);
        } catch (err) {
            dispatchNotification(err);
        }
    })

    updateClipPlaneSelection = (cp: ClipPlane) => {
        const clone = this.state.selectedClipPlanes.slice(0);

        const idx = clone.indexOf(cp);
        if (idx !== -1) {
            clone.splice(idx, 1);
        } else {
            clone.push(cp);
        }

        this.setState({
            selectedClipPlanes: clone
        });
    }

    removeClipPlanes = async () => {
        const {selectedClipPlanes} = this.state;

        if (selectedClipPlanes.length) {
            const ids = selectedClipPlanes.map(cp => cp.id);
            this.setState(state => {
                const clipPlanes = state.clipPlanes.filter(cp => !ids.includes(cp.id));
                return {
                    clipPlanes,
                    selectedClipPlanes: []
                };
            }, async () => {
                try {
                    await brayns.request(REMOVE_CLIP_PLANES, ids);
                } catch (err) {
                    dispatchNotification(err);
                }
            });
        }
    }

    showClipPlaneEditor = (cp: ClipPlane) => {
        this.setState({
            clipPlane: {...cp},
            showClipPlaneEditor: true
        });
    }

    closeClipPlaneEditor = () => {
        this.setState({
            showClipPlaneEditor: false
        });
    }

    cleanupClipPlane = () => {
        this.setState({
            clipPlane: undefined
        });
    }

    clearClipPlaneSelection = () => {
        this.setState({
            selectedClipPlanes: []
        });
    }


    handleKeyDown = (evt: KeyboardEvent) => {
        if (this.keyboardLocked) {
            return;
        }
        switch (evt.keyCode) {
            case KeyCode.Esc:
                if (this.state.showModelEditor) {
                    this.setState({
                        showModelEditor: false
                    });
                } else if (this.state.showClipPlaneEditor) {
                    this.setState({
                        showClipPlaneEditor: false
                    });
                } else {
                    this.setState({
                        selectedModels: [],
                        selectedClipPlanes: []
                    });
                }
                break;
            case KeyCode.D:
                if (evt.shiftKey) {
                    if (this.state.selectedModels.length) {
                        this.removeModels();
                    } else if (this.state.selectedClipPlanes.length) {
                        this.removeClipPlanes();
                    }
                }
                break;
            default:
                break;
        }
    }


    closeDrawer = () => {
        const {onClose} = this.props;
        if (onClose) {
            onClose();
        }
    }

    subscribeToScene = async () => {
        document.addEventListener('keydown', this.handleKeyDown, false);

        this.subs.push(...[
            onKeyboardLockChange()
                .subscribe(locked => {
                    this.keyboardLocked = locked;
                }),
            // Listen to scene updates
            brayns.observe(SET_SCENE)
                .subscribe(scene => {
                    const {showModelEditor, model} = this.state;

                    const {models} = scene;
                    const state = {models};

                    if (showModelEditor && model) {
                        Object.assign(state, {
                            model: models.find(m => m.id === model.id)
                        });
                    }

                    this.setState(state);
                }),
            brayns.observe(UPDATE_CLIP_PLANE)
                .subscribe(clipPlane => {
                    this.setState(state => {
                        const copy = state.clipPlanes.slice(0);
                        const index = copy.findIndex(cp => cp.id === clipPlane.id);

                        if (index !== -1) {
                            copy.splice(index, 1, clipPlane);

                            if (state.showClipPlaneEditor && state.clipPlane) {
                                return {
                                    clipPlane,
                                    clipPlanes: copy
                                } as any;
                            }
                        } else {
                            copy.push(clipPlane);
                        }

                        return {
                            clipPlanes: copy
                        };
                    });
                }),
            brayns.observe(REMOVE_CLIP_PLANES)
                .subscribe(items => {
                    this.setState(state => {
                        const clipPlanes = state.clipPlanes.filter(cp => !items.includes(cp.id));
                        return {clipPlanes};
                    });
                }),
            // TODO: Remove this one!
            brayns.ready.pipe(
                filter(ready => !ready))
                .subscribe(() => {
                    this.setState({
                        selectedModels: [],
                        selectedClipPlanes: [],
                        showModelEditor: false,
                        model: undefined,
                        showClipPlaneEditor: false,
                        clipPlane: undefined,
                        disabled: true
                    });
                }),
            // Get current scene when renderer connects
            onReady()
                .pipe(
                    mergeMap(() => Promise.all([
                        brayns.request(GET_SCENE),
                        brayns.request(GET_CLIP_PLANES)
                    ])))
                .subscribe(([scene, clipPlanes]) => {
                    const {models} = scene;
                    this.setState({
                        models,
                        clipPlanes,
                        disabled: false
                    });
                })
        ]);
    }
    unsubscribe = () => {
        document.removeEventListener('keydown', this.handleKeyDown);

        while (this.subs.length) {
            const sub = this.subs.pop();
            sub!.unsubscribe();
        }
    }

    componentWillUnmount() {
        this.unsubscribe();
    }

    componentDidUpdate() {
        const {open} = this.props;
        if (open && !this.subs.length) {
            this.subscribeToScene();
        } else if (!open) {
            this.unsubscribe();
        }
    }

    render() {
        const {
            tab,
            disabled,
            // Models
            models,
            selectedModels,
            showModelEditor,
            showModelInfo,
            model,
            // Clip planes
            clipPlanes,
            selectedClipPlanes,
            showClipPlaneEditor,
            clipPlane
        } = this.state;
        const {
            classes,
            SlideProps,
            theme
        } = this.props;
        const props = pick(this.props, ['anchor', 'open', 'variant']);

        const modelCount = selectedModels.length;
        const clipPlaneCount = selectedClipPlanes.length;

        const selectionNextVisibility = getSelectionNextVisibility(models, selectedModels);

        const hasModel = model !== undefined && !disabled;
        const canShowModelEditor = showModelEditor && hasModel;
        const canShowModelInfo = !!showModelInfo && hasModel;

        const canShowClipPlaneEditor = showClipPlaneEditor && clipPlane !== undefined && !disabled;
        const clipPlaneEditor = clipPlane ? (
            <ClipPlaneEditor
                value={clipPlane}
                disabled={disabled}
                onUpdate={this.updateClipPlane}
            />
        ) : null;

        const hideMainToolbar = canShowModelEditor || canShowClipPlaneEditor;
        const hideSecondaryToolbar = (modelCount > 0 || clipPlaneCount > 0) && !disabled;

        const showSelectedModelsActions = modelCount > 0 && !disabled;
        const showSelectedClipPlanesActions = clipPlaneCount > 0 && !disabled;

        const showAddClipPlane = tab === CLIP_PLANES_TAB_IDX && !showClipPlaneEditor && !disabled;

        const slideProps = {
            ...SlideProps,
            unmountOnExit: true
        };

        const transitionDuration = {
            enter: theme!.transitions.duration.enteringScreen,
            exit: theme!.transitions.duration.leavingScreen
        };


        // TODO: Fade transition timing should be 0 when first rendering everything
        return (
            <Fragment>
                <Drawer
                    {...props}
                    classes={this.props.DrawerClasses}
                    SlideProps={slideProps}
                >
                    <div className={classes.mainToolbar}>
                        <Fade in={!hideMainToolbar} unmountOnExit>
                            <div>
                                <IconButton onClick={this.closeDrawer} aria-label="Close models panel">
                                    <ChevronLeftIcon />
                                </IconButton>
                            </div>
                        </Fade>

                        <Fade in={canShowModelEditor} unmountOnExit>
                            <div className={classNames(classes.mainToolbarActions, classes.editToolbar)}>
                                <IconButton
                                    onClick={this.closeModelEditor}
                                    className={classes.back}
                                    aria-label="Close model editor"
                                >
                                    <ArrowBackIcon />
                                </IconButton>
                                <Typography className={classes.mainToolbarText} variant="subtitle1">
                                    {model ? model.name : ''}
                                </Typography>
                                <span className={classes.spacer} />
                            </div>
                        </Fade>

                        <Fade in={canShowClipPlaneEditor} unmountOnExit>
                            <div className={classNames(classes.mainToolbarActions, classes.editToolbar)}>
                                <IconButton
                                    onClick={this.closeClipPlaneEditor}
                                    className={classes.back}
                                    aria-label="Close clip plane editor"
                                >
                                    <ArrowBackIcon />
                                </IconButton>
                                <Typography variant="subtitle1">
                                    Clip Plane {clipPlane ? clipPlane.id : ''}
                                </Typography>
                                <span className={classes.spacer} />
                            </div>
                        </Fade>
                    </div>

                    <div className={classes.content}>
                        <Fade in={!hideMainToolbar} timeout={{enter: 350, exit: 250}}>
                            <div>
                                <div className={classes.secondaryToolbar}>
                                    <Fade in={!hideSecondaryToolbar} unmountOnExit>
                                        <AppBar position="absolute" elevation={0} color="default">
                                            <Tabs
                                                value={tab}
                                                onChange={this.onTabChange}
                                                indicatorColor="primary"
                                                textColor="primary"
                                                variant="fullWidth"
                                            >
                                                <Tab label="Models" />
                                                <Tab label="Clip Planes" />
                                            </Tabs>
                                        </AppBar>
                                    </Fade>

                                    <ModelActions
                                        open={showSelectedModelsActions}
                                        count={modelCount}
                                        nextVisibility={selectionNextVisibility}
                                        onVisibilityChange={this.setSelectedModelsVisibility}
                                        onClose={this.clearModelSelection}
                                        onClear={this.removeModels}
                                    />

                                    <ClipPlaneActions
                                        open={showSelectedClipPlanesActions}
                                        count={clipPlaneCount}
                                        onClose={this.clearClipPlaneSelection}
                                        onClear={this.removeClipPlanes}
                                    />
                                </div>

                                <SwipeableViews axis={'x'} index={tab} onChangeIndex={this.onSwipe}>
                                    <div>
                                        <Models
                                            models={models}
                                            selected={selectedModels}
                                            disabled={disabled}
                                            onSelectChange={this.updateModelSelection}
                                            onFocus={this.focusModel}
                                            onShowInfo={this.showModelInfo}
                                            onEdit={this.showModelEditor}
                                            onVisibilityChange={this.setModelVisibility}
                                            onDelete={this.deleteModel}
                                        />
                                    </div>

                                    <div>
                                        <ClipPlanes
                                            clipPlanes={clipPlanes}
                                            selected={selectedClipPlanes}
                                            disabled={disabled}
                                            onEdit={this.showClipPlaneEditor}
                                            onSelectChange={this.updateClipPlaneSelection}
                                        />
                                    </div>
                                </SwipeableViews>
                            </div>
                        </Fade>

                        <Slide
                            in={canShowModelEditor}
                            direction="up"
                            unmountOnExit
                            onExited={this.clearModel}
                        >
                            <div className={classes.editor}>
                                <ModelEditor
                                    bounds={model ? model.bounds : undefined}
                                    boundingBox={model ? model.boundingBox : undefined}
                                    visible={model ? model.visible : undefined}
                                    transformation={model ? model.transformation : undefined}
                                    id={model ? model.id : undefined}
                                    metadata={model ? model.metadata : undefined}
                                    onUpdate={this.updateModel}
                                    disabled={disabled}
                                />
                            </div>
                        </Slide>

                        <Slide
                            in={canShowClipPlaneEditor}
                            direction="up"
                            unmountOnExit
                            onExited={this.cleanupClipPlane}
                        >
                            <div className={classNames(classes.editor, classes.clipPlaneEditor)}>
                                {clipPlaneEditor}
                            </div>
                        </Slide>

                        <Zoom
                            in={showAddClipPlane}
                            timeout={transitionDuration}
                            unmountOnExit
                        >
                            <Tooltip title={'Add clip plane'} placement="left" {...TOOLTIP_DELAY}>
                                <Fab
                                    className={classes.addClipPlane}
                                    onClick={this.addClipPlane}
                                >
                                    <AddIcon />
                                </Fab>
                            </Tooltip>
                        </Zoom>
                    </div>
                </Drawer>

                <ModelMetadata
                    open={canShowModelInfo}
                    name={model ? model.name : undefined}
                    metadata={model ? model.metadata : undefined}
                    onClose={this.closeModelInfo}
                />
            </Fragment>
        );
    }
}


export default style(
        withCamera(SceneModels));


function getSelectionNextVisibility(models: Model[], selectedModels: ModelId[]) {
    const matches = selectedModels.map(id => models.find(model => model.id === id));
    if (matches.every(model => model ? !model.visible : false)
        || matches.some(model => model ? !model.visible : false)) {
        return true;
    }
    return false;
}


interface Props extends Pick<DrawerProps, 'anchor' | 'open' | 'variant'>,
    WithStyles<typeof styles, true>,
    WithCamera {
    DrawerClasses?: Partial<ClassNameMap<DrawerClassKey>>;
    SlideProps?: Partial<SlideProps>;
    onClose?(): void;
}

interface State {
    tab?: number;
    disabled?: boolean;

    // Models
    models: Model[];
    selectedModels: ModelId[];
    showModelEditor?: boolean;
    showModelInfo?: boolean;
    model?: Model;

    // Clip planes
    clipPlanes: ClipPlane[];
    selectedClipPlanes: ClipPlane[];
    showClipPlaneEditor?: boolean;
    clipPlane?: ClipPlane;
}
