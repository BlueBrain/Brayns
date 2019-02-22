// tslint:disable: no-string-literal
import React, {
    createRef,
    Fragment,
    MouseEvent,
    PureComponent,
    RefObject
} from 'react';

import {LOAD_MODEL, ModelParams} from 'brayns';
import classNames from 'classnames';
import {Subscription} from 'rxjs';

import AppBar from '@material-ui/core/AppBar';
import lightBlue from '@material-ui/core/colors/lightBlue';
import red from '@material-ui/core/colors/red';
import CssBaseline from '@material-ui/core/CssBaseline';
import Drawer from '@material-ui/core/Drawer';
import Fade from '@material-ui/core/Fade';
import IconButton from '@material-ui/core/IconButton';
import Menu from '@material-ui/core/Menu';
import MenuItem from '@material-ui/core/MenuItem';
import {
    createMuiTheme,
    createStyles,
    MuiThemeProvider,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';
import Toolbar from '@material-ui/core/Toolbar';
import Tooltip from '@material-ui/core/Tooltip';
import ChevronRightIcon from '@material-ui/icons/ChevronRight';
import CloudUploadIcon from '@material-ui/icons/CloudUpload';
import ComputerIcon from '@material-ui/icons/Computer';
import DeviceUnknownIcon from '@material-ui/icons/DeviceUnknown';
import KeyboardIcon from '@material-ui/icons/Keyboard';
import LayersIcon from '@material-ui/icons/Layers';
import LayersClearIcon from '@material-ui/icons/LayersClear';
import LinkIcon from '@material-ui/icons/Link';
import PhonelinkSetupIcon from '@material-ui/icons/PhonelinkSetup';
import PhotoCameraIcon from '@material-ui/icons/PhotoCamera';
import ThreeSixtyIcon from '@material-ui/icons/ThreeSixty';
import TimelineIcon from '@material-ui/icons/Timeline';
import TuneIcon from '@material-ui/icons/Tune';

import brayns, {
    AnimationProvider,
    AppParamsProvider,
    CameraProvider,
    LoadersProvider,
    RendererProvider,
    withConnectionStatus,
    WithConnectionStatus
} from '../common/client';
import {
    GithubIcon,
    LightbulbFull,
    LightbulbOutline
} from '../common/components';
import {
    APP_BAR_HEIGHT,
    APP_BAR_HEIGHT_XS,
    KeyCode,
    TOOLTIP_DELAY
} from '../common/constants';
import {
    dispatchKeyboardLock,
    dispatchRequest,
    onKeyboardLockChange,
    onRequestCancel
} from '../common/events';
import storage from '../common/storage';

import AnimationPlayer from './animation-player';
import AppInfo from './app-info';
import AppSettings, {Preference} from './app-settings';
import Camera from './camera';
import ConnectionStatus from './connection-status';
import Cube from './cube';
import {DataPortal, LoadModel} from './data-portal';
import ImageStream from './image-stream';
import Notifications from './notifications';
import QuitRenderer from './quit-renderer';
import RendererSettings from './renderer-settings';
import RequestNotifications from './request-notifications';
import ResetCamera from './reset-camera';
import Scene from './scene';
import Shortcuts from './shortcuts';
import Snapshot from './snapshot';
import Statistics from './statistics';


const SETTINGS_PANEL_WIDTH = 380;
const MODELS_PANEL_WIDTH = 380;
const PANELS_WIDTH = MODELS_PANEL_WIDTH + SETTINGS_PANEL_WIDTH;
const BRAYNS_GITHUB_URL = 'https://github.com/BlueBrain/Brayns';

const MODELS_PANEL_KEY = 'models';
const SETTINGS_PANEL_KEY = 'settings';
const STATISTICS_KEY = 'statistics';
const NAV_CUBE_KEY = 'cube';
const THEME_KEY = 'theme';


// https://material-ui-next.com/customization/themes/#palette
const LIGHT_THEME = createMuiTheme({
    palette: {
        primary: {
            light: lightBlue['A400'],
            main: lightBlue['500'],
            dark: lightBlue['A700']
        },
        error: {
            main: red['400']
        }
    },
    typography: {
        useNextVariants: true
    }
});

const DARK_THEME = createMuiTheme({
    palette: {
        primary: {
            light: lightBlue['A400'],
            main: lightBlue['500'],
            dark: lightBlue['A700']
        },
        error: {
            main: red['400']
        },
        type: 'dark'
    },
    typography: {
        useNextVariants: true
    }
});

const APP_PREFERENCES = [
    {
        key: THEME_KEY,
        label: 'Dark Theme',
        icon: (checked?: boolean) => checked ? <LightbulbOutline /> : <LightbulbFull />,
        checked: !!storage.get(THEME_KEY)
    },
    {
        key: NAV_CUBE_KEY,
        label: 'Navigation Cube',
        icon: <ThreeSixtyIcon />,
        checked: !!storage.get(NAV_CUBE_KEY)
    },
    {
        key: STATISTICS_KEY,
        label: 'Statistics',
        icon: <TimelineIcon />,
        checked: !!storage.get(STATISTICS_KEY)
    }
];


// https://material-ui-next.com/demos/drawers/#mini-variant-drawer
const styles = (theme: Theme) => createStyles({
    root: {
        width: '100vw',
        overflow: 'hidden',
        zIndex: 1
    },
    appFrame: {
        position: 'relative',
        display: 'flex',
        width: '100vw',
        height: '100vh'
    },
    appBar: {
        position: 'absolute',
        willChange: 'width, margin',
        transition: theme.transitions.create(['width', 'margin'], {
            easing: theme.transitions.easing.sharp,
            duration: theme.transitions.duration.leavingScreen
        })
    },
    appBarToolbar: {
        padding: `0 ${theme.spacing.unit * 2}px`
    },
    appBarShift: {
        transition: theme.transitions.create(['width', 'margin'], {
            easing: theme.transitions.easing.easeOut,
            duration: theme.transitions.duration.enteringScreen
        })
    },
    appBarShiftRight: {
        width: `calc(100% - ${SETTINGS_PANEL_WIDTH}px)`,
        marginRight: SETTINGS_PANEL_WIDTH
    },
    appBarShiftLeft: {
        width: `calc(100% - ${MODELS_PANEL_WIDTH}px)`,
        marginLeft: MODELS_PANEL_WIDTH
    },
    appBarShiftBoth: {
        width: `calc(100% - ${PANELS_WIDTH}px)`,
        marginLeft: MODELS_PANEL_WIDTH,
        marginRight: SETTINGS_PANEL_WIDTH
    },
    drawerPaper: {
        position: 'absolute',
        height: '100vh',
        overflowX: 'hidden',
        opacity: .98
    },
    drawerInner: {
        display: 'flex',
        flexDirection: 'column',
        flex: 1
    },
    drawerHeader: {
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'flex-start',
        padding: `0 ${theme.spacing.unit * 2}px`,
        // backgroundColor: theme.palette.background.default,
        ...theme.mixins.toolbar
    },
    modelsDrawer: {
        width: MODELS_PANEL_WIDTH
    },
    settingsDrawer: {
        width: SETTINGS_PANEL_WIDTH
    },
    viewport: {
        position: 'absolute',
        display: 'flex',
        flexDirection: 'column',
        width: '100%',
        backgroundColor: theme.palette.background.default,
        height: 'calc(100% - 56px)',
        marginTop: APP_BAR_HEIGHT_XS,
        [theme.breakpoints.up('sm')]: {
            height: 'calc(100% - 64px)',
            marginTop: APP_BAR_HEIGHT
        }
    },
    canvas: {
        position: 'absolute',
        top: 0,
        right: 0,
        bottom: 0,
        left: 0
    },
    content: {
        position: 'relative',
        display: 'flex',
        flex: 1,
        willChange: 'margin',
        transition: theme.transitions.create('margin', {
            easing: theme.transitions.easing.sharp,
            duration: theme.transitions.duration.leavingScreen
        })
    },
    contentShift: {
        transition: theme.transitions.create('margin', {
            easing: theme.transitions.easing.easeOut,
            duration: theme.transitions.duration.enteringScreen
        })
    },
    contentShiftLeft: {
        marginLeft: MODELS_PANEL_WIDTH
    },
    contentShiftRight: {
        marginRight: SETTINGS_PANEL_WIDTH
    },
    contentShiftBoth: {
        marginLeft: MODELS_PANEL_WIDTH,
        marginRight: SETTINGS_PANEL_WIDTH
    },
    dataPortal: {
        display: 'flex',
        flexDirection: 'column',
        flex: 1
    },
    statistics: {
        position: 'absolute',
        top: theme.spacing.unit * 2,
        left: theme.spacing.unit * 2
    },
    cube: {
        position: 'absolute',
        top: 50,
        right: 50
    },
    fullWidth: {
        flex: 1
    },
    gutterBottom: {
        marginBottom: theme.spacing.unit * 2
    },
    gutterRight: {
        marginRight: theme.spacing.unit * 2
    },
    spacer: {
        flex: 1
    }
});

// https://material-ui-next.com/guides/typescript/
const style = withStyles(styles);


class App extends PureComponent<Props, State> {
    state: State = {
        showFileDialog: false,
        showLoadModelDialog: false,
        showModelsPanel: !!storage.get(MODELS_PANEL_KEY),
        showSettingsPanel: !!storage.get(SETTINGS_PANEL_KEY),
        showStatistics: !!storage.get(STATISTICS_KEY),
        showNavCube: !!storage.get(NAV_CUBE_KEY),
        appPreferences: APP_PREFERENCES,
        isDarkTheme: !!storage.get(THEME_KEY),
        theme: getTheme()
    };

    viewportRef: RefObject<HTMLDivElement> = createRef();
    get viewport() {
        const {current} = this.viewportRef;
        return current;
    }

    private keyboardLocked = false;
    private subs: Subscription[] = [];

    // Models panel
    closeModelsPanel = () => {
        storage.set(MODELS_PANEL_KEY, false);
        this.setState({
            showModelsPanel: false
        });
    }
    toggleModelsPanel = () => {
        const value = !this.state.showModelsPanel;
        storage.set(MODELS_PANEL_KEY, value);
        this.setState({
            showModelsPanel: value
        });
    }

    // Renderer settings panel
    closeSettingsPanel = () => {
        storage.set(SETTINGS_PANEL_KEY, false);
        this.setState({
            showSettingsPanel: false
        });
    }
    toggleSettingsPanel = () => {
        const value = !this.state.showSettingsPanel;
        storage.set(SETTINGS_PANEL_KEY, value);
        this.setState({
            showSettingsPanel: value
        });
    }

    // Add model menu
    showAddModelMenu = (evt: MouseEvent<HTMLElement>) => {
        this.setState({
            addModelMenuAnchor: evt.currentTarget
        });
    }
    hideAddModelMenu = () => {
        this.setState({
            addModelMenuAnchor: undefined
        });
    }

    openFileDialog = () => {
        this.setState({
            addModelMenuAnchor: undefined,
            showFileDialog: true
        });
    }
    closeFileDialog = () => {
        this.setState({
            addModelMenuAnchor: undefined,
            showFileDialog: false
        });
    }

    // Load model from path
    openLoadModelDialog = () => {
        dispatchKeyboardLock(true);
        this.setState({
            addModelMenuAnchor: undefined,
            showLoadModelDialog: true
        });
    }
    closeLoadModelDialog = () => {
        dispatchKeyboardLock(false);
        this.setState({
            showLoadModelDialog: false
        });
    }

    loadPath = async (params: Partial<ModelParams>) => {
        const {path} = params;
        if (path && path.length) {
            const load = brayns.request(LOAD_MODEL, params);

            const sub = onRequestCancel(load)
                .subscribe(() => {
                    load.cancel();
                });

            dispatchRequest(load);

            try {
                await load;
            } catch {} // tslint:disable-line: no-empty

            sub.unsubscribe();
        }
    }

    // App settings
    openAppInfo = () => {
        dispatchKeyboardLock(true);
        this.setState({
            showAppInfo: true
        });
    }
    closeAppInfo = () => {
        dispatchKeyboardLock(false);
        this.setState({
            showAppInfo: false
        });
    }

    // App settings
    openAppSettings = () => {
        dispatchKeyboardLock(true);
        this.setState({
            showAppSettings: true
        });
    }
    closeAppSettings = () => {
        dispatchKeyboardLock(false);
        this.setState({
            showAppSettings: false
        });
    }
    updateAppPreference = (preference: Preference, checked: boolean) => {
        const {appPreferences} = this.state;
        const index = appPreferences.indexOf(preference);
        const preferences = [...appPreferences];
        preferences.splice(index, 1, {
            ...preference,
            checked
        });

        switch (preference.key) {
            case STATISTICS_KEY:
                this.toggleStatistics();
                break;
            case NAV_CUBE_KEY:
                this.toggleNavCube();
                break;
            case THEME_KEY:
                this.toggleTheme();
                break;
        }

        this.setState({
            appPreferences: preferences
        });
    }

    // Shortcuts
    openShortcuts = () => {
        dispatchKeyboardLock(true);
        this.setState({
            showShortcuts: true
        });
    }
    closeShortcuts = () => {
        dispatchKeyboardLock(false);
        this.setState({
            showShortcuts: false
        });
    }

    // Snapshot
    openSnapshot = () => {
        dispatchKeyboardLock(true);
        this.setState({
            // ...defaultSize(this.props.width),
            showSnapshot: true
        });
    }

    closeSnapshot = () => {
        dispatchKeyboardLock(false);
        this.setState({
            showSnapshot: false
        });
    }

    // Statistics
    toggleStatistics = () => {
        const enabled = !this.state.showStatistics;
        storage.set(STATISTICS_KEY, enabled);
        this.setState({
            showStatistics: enabled
        });
    }

    // Nav cube
    toggleNavCube = () => {
        const enabled = !this.state.showNavCube;
        storage.set(NAV_CUBE_KEY, enabled);
        this.setState({
            showNavCube: enabled
        });
    }

    // Theme
    toggleTheme = () => {
        const {isDarkTheme} = this.state;
        const value = !isDarkTheme;
        storage.set(THEME_KEY, value);
        const theme = getTheme();
        this.setState({
            theme,
            isDarkTheme: value
        });
    }

    // Keyboard interaction
    handleKeydown = (evt: KeyboardEvent) => {
        const {online} = this.props;

        if (!this.keyboardLocked) {
            switch (evt.keyCode) {
                case KeyCode.L:
                    if (evt.metaKey || evt.ctrlKey && online) {
                        evt.preventDefault();
                        evt.stopPropagation();
                        this.openLoadModelDialog();
                    }
                    break;
                case KeyCode.ForwardSlash:
                    // Show shortcuts
                    if (evt.shiftKey) {
                        this.openShortcuts();
                    }
                    break;
                case KeyCode.O:
                    // Show app settings
                    if (evt.altKey) {
                        this.openAppSettings();
                    }
                    break;
                case KeyCode.U:
                    if (evt.metaKey || evt.ctrlKey && online) {
                        this.openFileDialog();
                    }
                    break;
                case KeyCode.S:
                    if (evt.shiftKey && online) {
                        this.openSnapshot();
                    }
                    break;
                default:
                    break;
            }
        }
    }

    componentDidMount() {
        window.addEventListener('keydown', this.handleKeydown, false);
        this.subs.push(...[
            onKeyboardLockChange()
                .subscribe(locked => {
                    this.keyboardLocked = locked;
                })
        ]);
    }
    componentWillUnmount() {
        window.removeEventListener('keydown', this.handleKeydown);
        while (this.subs.length) {
            const sub = this.subs.pop();
            sub!.unsubscribe();
        }
    }

    render() {
        const {classes, online} = this.props;
        const {
            addModelMenuAnchor,
            showFileDialog,
            showLoadModelDialog,
            showModelsPanel,
            showSettingsPanel,
            showStatistics,
            showNavCube,
            showAppSettings,
            showAppInfo,
            showShortcuts,
            showSnapshot,
            appPreferences,
            theme
        } = this.state;

        const offline = !online;
        const canShowStatistics = showStatistics && online;
        const canShowNavCube = showNavCube && online;

        const modelsBtnTitle = showModelsPanel ? 'Hide models' : 'Show models';
        const modelsBtnColor = showModelsPanel ? 'inherit' : 'default';
        const modelsBtnIcon = showModelsPanel ? (<LayersClearIcon />) : (<LayersIcon />);
        const settingsBtnTitle = showSettingsPanel ? 'Hide renderer settings' : 'Show renderer settings';
        const settingsBtnColor = showSettingsPanel ? 'inherit' : 'default';

        const showBothDrawers = showSettingsPanel || showModelsPanel;
        const appBarShift = {
            [classes.appBarShift]: showBothDrawers,
            [classes.appBarShiftLeft]: showModelsPanel && !showSettingsPanel,
            [classes.appBarShiftRight]: showSettingsPanel && !showModelsPanel,
            [classes.appBarShiftBoth]: showSettingsPanel && showModelsPanel
        };
        const contentShift = {
            [classes.contentShift]: showBothDrawers,
            [classes.contentShiftLeft]: showModelsPanel && !showSettingsPanel,
            [classes.contentShiftRight]: showSettingsPanel && !showModelsPanel,
            [classes.contentShiftBoth]: showSettingsPanel && showModelsPanel
        };

        const slideProps = {
            unmountOnExit: true
        };

        return (
            <Fragment>
                <MuiThemeProvider theme={theme}>
                    <CssBaseline />
                    {/* TODO: Maybe have a single provider for all the Brayns API stuff?! */}
                    <LoadersProvider>
                        <AppParamsProvider>
                            <CameraProvider>
                                <RendererProvider>
                                    <div className={classes.root}>
                                        <div className={classes.appFrame}>
                                            <AppBar color="default" elevation={0} className={classNames(classes.appBar, appBarShift)}>
                                                <Toolbar className={classes.appBarToolbar} disableGutters>
                                                    <Tooltip title={modelsBtnTitle} {...TOOLTIP_DELAY}>
                                                        <IconButton
                                                            color={modelsBtnColor}
                                                            onClick={this.toggleModelsPanel}
                                                            aria-label="Open models panel"
                                                        >
                                                            {modelsBtnIcon}
                                                        </IconButton>
                                                    </Tooltip>
                                                    <Tooltip title={'Add model'} placement="bottom" {...TOOLTIP_DELAY}>
                                                        <div>
                                                            <IconButton
                                                                onClick={this.showAddModelMenu}
                                                                aria-label="Open add model menu"
                                                                aria-owns={addModelMenuAnchor ? 'add-model-menu' : ''}
                                                                aria-haspopup="true"
                                                                disabled={offline}
                                                            >
                                                                <CloudUploadIcon />
                                                            </IconButton>
                                                        </div>
                                                    </Tooltip>
                                                    <Menu
                                                        id="add-model-menu"
                                                        anchorEl={addModelMenuAnchor}
                                                        open={Boolean(addModelMenuAnchor)}
                                                        onClose={this.hideAddModelMenu}
                                                    >
                                                        <MenuItem onClick={this.openFileDialog}>
                                                            <ComputerIcon className={classes.gutterRight} />
                                                            From your computer
                                                        </MenuItem>
                                                        <MenuItem onClick={this.openLoadModelDialog}>
                                                            <LinkIcon className={classes.gutterRight} />
                                                            From path
                                                        </MenuItem>
                                                    </Menu>

                                                    <Tooltip title={'Take a snapshot'} placement="bottom" {...TOOLTIP_DELAY}>
                                                        <div>
                                                            <IconButton
                                                                onClick={this.openSnapshot}
                                                                aria-label={'Open the snapshot dialog'}
                                                                disabled={offline}
                                                            >
                                                                <PhotoCameraIcon />
                                                            </IconButton>
                                                        </div>
                                                    </Tooltip>

                                                    <span className={classes.spacer} />

                                                    <ResetCamera />

                                                    <QuitRenderer />

                                                    <Tooltip title={'App info'} {...TOOLTIP_DELAY}>
                                                        <IconButton
                                                            onClick={this.openAppInfo}
                                                            aria-label="Show app info"
                                                        >
                                                            <DeviceUnknownIcon />
                                                        </IconButton>
                                                    </Tooltip>
                                                    <Tooltip title={'Shortcuts'} {...TOOLTIP_DELAY}>
                                                        <IconButton
                                                            onClick={this.openShortcuts}
                                                            aria-label="Show shortcuts"
                                                        >
                                                            <KeyboardIcon />
                                                        </IconButton>
                                                    </Tooltip>
                                                    <Tooltip title={'Report an issue'} {...TOOLTIP_DELAY}>
                                                        <IconButton
                                                            href={BRAYNS_GITHUB_URL}
                                                            target="_blank"
                                                            rel="noopener"
                                                            aria-label="Navigate to Brayns github issues"
                                                        >
                                                            <GithubIcon />
                                                        </IconButton>
                                                    </Tooltip>
                                                    <Tooltip title={'App settings'} {...TOOLTIP_DELAY}>
                                                        <IconButton
                                                            onClick={this.openAppSettings}
                                                            aria-label="Open application settings"
                                                        >
                                                            <PhonelinkSetupIcon />
                                                        </IconButton>
                                                    </Tooltip>
                                                    <Tooltip title={settingsBtnTitle} {...TOOLTIP_DELAY}>
                                                        <IconButton
                                                            color={settingsBtnColor}
                                                            onClick={this.toggleSettingsPanel}
                                                            aria-label="Open settings panel"
                                                        >
                                                            <TuneIcon />
                                                        </IconButton>
                                                    </Tooltip>
                                                </Toolbar>
                                            </AppBar>
                                            {/* https://material-ui-next.com/demos/drawers/#persistent-drawer */}
                                            <Scene
                                                variant="persistent"
                                                DrawerClasses={{paper: classNames(classes.drawerPaper, classes.modelsDrawer)}}
                                                SlideProps={slideProps}
                                                anchor={'left'}
                                                open={showModelsPanel}
                                                onClose={this.closeModelsPanel}
                                            />
                                            <main ref={this.viewportRef} className={classes.viewport}>
                                                <div className={classes.canvas}>
                                                    <ImageStream />
                                                </div>
                                                <div className={classNames(classes.content, contentShift)}>
                                                    <DataPortal
                                                        className={classes.dataPortal}
                                                        open={showFileDialog}
                                                        onClose={this.closeFileDialog}
                                                    >
                                                        <Camera />
                                                        <Fade in={canShowStatistics} unmountOnExit>
                                                            <div className={classes.statistics}>
                                                                <Statistics />
                                                            </div>
                                                        </Fade>
                                                        <Fade in={canShowNavCube} unmountOnExit>
                                                            <div className={classes.cube}>
                                                                <Cube />
                                                            </div>
                                                        </Fade>

                                                        <AnimationProvider>
                                                            <RequestNotifications />
                                                            <AnimationPlayer />
                                                            <Snapshot
                                                                open={!!showSnapshot}
                                                                onClose={this.closeSnapshot}
                                                            />
                                                        </AnimationProvider>
                                                        <ConnectionStatus open={offline} />
                                                    </DataPortal>
                                                </div>
                                            </main>
                                            <Drawer
                                                variant="persistent"
                                                classes={{paper: classNames(classes.drawerPaper, classes.settingsDrawer)}}
                                                anchor={'right'}
                                                open={showSettingsPanel}
                                                SlideProps={slideProps}
                                            >
                                                <div className={classes.drawerInner}>
                                                    <div className={classes.drawerHeader}>
                                                        <IconButton
                                                            onClick={this.closeSettingsPanel}
                                                            aria-label="Close settings"
                                                        >
                                                            <ChevronRightIcon />
                                                        </IconButton>
                                                        <span className={classes.spacer} />
                                                    </div>
                                                    <RendererSettings />
                                                </div>
                                            </Drawer>
                                        </div>

                                        <LoadModel
                                            open={showLoadModelDialog as boolean}
                                            onPathLoad={this.loadPath}
                                            onClose={this.closeLoadModelDialog}
                                        />

                                        <AppInfo
                                            open={showAppInfo}
                                            onClose={this.closeAppInfo}
                                        />

                                        <AppSettings
                                            open={showAppSettings}
                                            preferences={appPreferences}
                                            onUpdate={this.updateAppPreference}
                                            onClose={this.closeAppSettings}
                                        />

                                        <Shortcuts
                                            open={!!showShortcuts}
                                            onClose={this.closeShortcuts}
                                        />

                                        <Notifications />
                                    </div>
                                </RendererProvider>
                            </CameraProvider>
                        </AppParamsProvider>
                    </LoadersProvider>
                </MuiThemeProvider>
            </Fragment>
        );
    }
}


export default style(
    withConnectionStatus(App));


function getTheme() {
    const isDarkTheme = !!storage.get<boolean>(THEME_KEY);
    if (isDarkTheme) {
        return DARK_THEME;
    }
    return LIGHT_THEME;
}


type Props = WithStyles<typeof styles> & WithConnectionStatus;

interface State {
    addModelMenuAnchor?: HTMLElement;
    showLoadModelDialog?: boolean;
    showFileDialog?: boolean;
    showModelsPanel?: boolean;
    showSettingsPanel?: boolean;

    showStatistics?: boolean;
    showNavCube?: boolean;

    showShortcuts?: boolean;
    showSnapshot?: boolean;

    showAppInfo?: boolean;
    showAppSettings?: boolean;
    appPreferences: Preference[];

    isDarkTheme?: boolean;
    theme: Theme;
}
