import React, {PureComponent} from 'react';

import {
    LOAD_MODEL,
    PathParams,
    SNAPSHOT,
    SnapshotParams,
    UPLOAD_MODEL,
    UploadParams
} from 'brayns';
import classNames from 'classnames';
import {Request} from 'rockets-client';
import {animationFrameScheduler, Subscription} from 'rxjs';
import {observeOn} from 'rxjs/operators';

import Avatar from '@material-ui/core/Avatar';
import CircularProgress from '@material-ui/core/CircularProgress';
import Collapse from '@material-ui/core/Collapse';
import green from '@material-ui/core/colors/green';
import Fade from '@material-ui/core/Fade';
import IconButton from '@material-ui/core/IconButton';
import List from '@material-ui/core/List';
import ListItem from '@material-ui/core/ListItem';
import ListItemSecondaryAction from '@material-ui/core/ListItemSecondaryAction';
import ListItemText from '@material-ui/core/ListItemText';
import Paper from '@material-ui/core/Paper';
import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';
import Typography from '@material-ui/core/Typography';
import CancelIcon from '@material-ui/icons/CancelRounded';
import CheckIcon from '@material-ui/icons/Check';
import CloseIcon from '@material-ui/icons/Close';
import CloudUploadIcon from '@material-ui/icons/CloudUpload';
import ExpandMoreIcon from '@material-ui/icons/ExpandMore';
import PhotoCameraIcon from '@material-ui/icons/PhotoCamera';
import WarningIcon from '@material-ui/icons/WarningRounded';

import {
    dispatchRequestCancel,
    onRequestComplete,
    onRequestProgress,
    onRequestStart,
    ProgressEvent
} from '../../common/events';


const styles = (theme: Theme) => createStyles({
    root: {
        position: 'absolute',
        bottom: theme.spacing.unit * 2,
        right: theme.spacing.unit * 2,
        width: 360
    },
    header: {
        position: 'relative',
        display: 'flex',
        minHeight: 48,
        borderTopLeftRadius: theme.shape.borderRadius,
        borderTopRightRadius: theme.shape.borderRadius,
        backgroundColor: theme.palette.background.default,
        transition: theme.transitions.create('border', {
            easing: theme.transitions.easing.easeInOut,
            duration: theme.transitions.duration.standard
        })
    },
    headerShift: {
        borderBottomLeftRadius: theme.shape.borderRadius,
        borderBottomRightRadius: theme.shape.borderRadius
    },
    headerText: {
        display: 'block',
        flex: 1,
        paddingLeft: theme.spacing.unit * 3,
        lineHeight: '48px',
        textOverflow: 'ellipsis',
        whiteSpace: 'nowrap',
        overflow: 'hidden',
        maxWidth: 248
    },
    expandBtn: {
        position: 'absolute',
        top: '50%',
        right: 56,
        transform: 'translateY(-50%) rotate(0deg)',
        transition: theme.transitions.create('transform', {
            easing: theme.transitions.easing.easeInOut,
            duration: theme.transitions.duration.standard
        })
    },
    expandBtnShift: {
        transform: 'translateY(-50%) rotate(180deg)'
    },
    closeBtn: {
        position: 'absolute',
        top: 0,
        right: theme.spacing.unit,
        paddingRight: '12px !important'
    },
    notifications: {
        maxHeight: 217,
        overflowY: 'auto'
    },
    requestAction: {
        padding: '10px 12px'
    },
    progressContainer: {
        position: 'relative'
    },
    progress: {
        position: 'absolute',
        top: 12,
        left: 12,
        zIndex: -2,
        color: theme.palette.text.primary
    },
    successColor: {
        color: green[500]
    }
});


// https://material-ui-next.com/guides/typescript/
const style = withStyles(styles);


class Notifications extends PureComponent<Props, State> {
    state: State = {
        requests: [],
        activeRequests: [],
        doneRequests: [],
        canceledRequests: [],
        closeNotifications: true
    };

    private subs: Subscription[] = [];

    toggleNotifications = () => {
        this.setState(state => ({expandNotifications: !state.expandNotifications}));
    }
    closeNotifications = () => {
        this.setState({closeNotifications: true});
    }

    clearRequests = () => {
        this.setState({
            requests: [],
            activeRequests: [],
            doneRequests: [],
            canceledRequests: []
        });
    }

    cancelRequest = (request: AnyRequest) => () => {
        dispatchRequestCancel(request);

        this.setState(state => {
            const index = state.canceledRequests.findIndex(r => r.id === request.id);
            if (index !== -1) {
                return {} as any;
            }

            return {
                canceledRequests: [
                    ...this.state.canceledRequests,
                    request
                ]
            };
        });
    }

    componentDidMount() {
        this.subs.push(...[
            onRequestStart()
                .subscribe(request => {
                    this.setState(state => {
                        const requests = [...state.requests];
                        requests.unshift(request);
                        return {
                            requests,
                            closeNotifications: false,
                            expandNotifications: true
                        };
                    });
                }),
            onRequestProgress()
                .pipe(observeOn(animationFrameScheduler))
                .subscribe(progress => {
                    this.setState(state => {
                        const active = [...state.activeRequests];

                        const index = active.findIndex(p => p.id === progress.id);
                        if (index !== -1) {
                            active.splice(index, 1, progress);
                        } else {
                            active.unshift(progress);
                        }

                        return {activeRequests: active};
                    });
                }),
            onRequestComplete()
                .subscribe(request => {
                    this.setState(state => {
                        const done = pushDone(request, state.doneRequests);
                        return {doneRequests: done};
                    });
                })
        ]);
    }
    componentWillUnmount() {
        while (this.subs.length) {
            const sub = this.subs.pop();
            sub!.unsubscribe();
        }
    }

    render() {
        const {classes} = this.props;
        const {
            requests,
            activeRequests,
            canceledRequests,
            doneRequests,
            expandNotifications,
            closeNotifications
        } = this.state;

        const notifications = renderNotifications({
            requests,
            activeRequests,
            doneRequests,
            canceledRequests,
            classes,
            cancelRequest: this.cancelRequest
        });

        const activeCount = activeRequests.length;
        const doneCount = doneRequests.length;
        const pendingCount = activeCount - doneCount;

        const notificationsHeaderText = pendingCount > 0
            ? `${pendingCount} pending ${pluralizeReqStr(pendingCount)}, ${doneCount} complete`
            : `${doneCount} ${pluralizeReqStr(doneCount)} complete`;

        return (
            <Fade
                in={!closeNotifications}
                onExited={this.clearRequests}
                unmountOnExit
            >
                <div className={classes.root}>
                    <Paper>
                        <div className={classNames(classes.header, {[classes.headerShift]: !expandNotifications})}>
                            <Typography className={classes.headerText} variant="subtitle1">
                                {notificationsHeaderText}
                            </Typography>
                            <IconButton
                                onClick={this.toggleNotifications}
                                className={classNames(classes.expandBtn, {[classes.expandBtnShift]: expandNotifications})}
                            >
                                <ExpandMoreIcon />
                            </IconButton>
                            <IconButton
                                onClick={this.closeNotifications}
                                className={classes.closeBtn}
                            >
                                <CloseIcon />
                            </IconButton>
                        </div>
                        <Collapse in={expandNotifications}>
                            <div className={classes.notifications}>
                                <List>
                                    {notifications}
                                </List>
                            </div>
                        </Collapse>
                    </Paper>
                </div>
            </Fade>
        );
    }
}


export default style(Notifications);


function renderNotifications({
    requests, activeRequests, doneRequests, canceledRequests,
    classes,
    cancelRequest
}: {
    requests: AnyRequest[];
    activeRequests: ProgressEvent[];
    doneRequests: AnyRequest[];
    canceledRequests: AnyRequest[];
    classes: Record<any, string>;
    cancelRequest(request: AnyRequest): () => void;
}) {
    const items = requests.map(request => {
        const key = `${request.id}`;
        const name = getName(request);
        const requestFinder = createRequestFinder(request);

        const icon = getIcon(request);
        const canceled = !!canceledRequests.find(requestFinder);
        const done = !!doneRequests.find(requestFinder);

        const progress = activeRequests.find(requestFinder);
        const amount = getProgressAmount(progress);
        const message = getMessage(progress, done, canceled);

        let action;
        if (done && canceled) {
            action = <WarningIcon color="error" />;
        } else if (done && !canceled) {
            action = <CheckIcon className={classes.successColor} />;
        } else if (!done && canceled)  {
            action = (
                <CircularProgress
                    variant="indeterminate"
                    color="secondary"
                    thickness={4}
                    size={24}
                />
            );
        } else {
            action = (
                <div className={classes.progressContainer}>
                    <IconButton
                        color="inherit"
                        onClick={cancelRequest(request)}
                        disabled={canceled}
                        aria-label="Cancel request"
                    >
                        <CancelIcon />
                    </IconButton>
                    <CircularProgress
                        className={classes.progress}
                        variant="static"
                        color="inherit"
                        value={amount}
                        thickness={4}
                        size={24}
                    />
                </div>
            );
        }

        return (
            <ListItem key={key}>
                <Avatar>{icon}</Avatar>
                <ListItemText primary={name} secondary={message}/>
                <ListItemSecondaryAction className={classNames((done || canceled) && classes.requestAction)}>
                    {action}
                </ListItemSecondaryAction>
            </ListItem>
        );
    });

    return items;
}

function pushDone<T extends {id: string | number}>(request: AnyRequest, items: T[]) {
    const index = items.findIndex(item => item.id === request.id);
    if (index !== -1) {
        return [...items];
    }
    return [
        ...items,
        request
    ];
}

function createRequestFinder(request: AnyRequest) {
    return <T extends {id: string | number}>(item: T) => item.id === request.id;
}

function getProgressAmount(progress?: ProgressEvent) {
    if (progress) {
        const {amount} = progress;
        const max = 1;
        const min = 0;
        const value = (amount - min) / (max - min) * 100;
        return value;
    }
    return 0;
}

function getMessage(progress?: ProgressEvent, done?: boolean, canceled?: boolean) {
    if (canceled && !done) {
        return 'Canceling ...';
    }

    if (progress && progress.amount < 1) {
        return progress.operation;
    }

    if (done && !canceled) {
        return 'Done';
    }

    if (done && canceled) {
        return 'Canceled';
    }

    return 'Pending ...';
}

function getName(request: AnyRequest) {
    return isSnapshot(request)
        ? request.params!.name
        : isLoad(request)
            ? request.params!.path
            : isUpload(request)
                ? `${request.params!.name}.${request.params!.type}`
                : '';
}

function getIcon(request: AnyRequest) {
    if (isUpload(request) || isLoad(request)) {
        return <CloudUploadIcon />;
    } else if (isSnapshot(request)) {
        return <PhotoCameraIcon />;
    }
    return null;
}

function isSnapshot(request: AnyRequest): request is Request<SnapshotParams> {
    return request.method === SNAPSHOT;
}

function isUpload(request: AnyRequest): request is Request<UploadParams> {
    return request.method === UPLOAD_MODEL;
}

function isLoad(request: AnyRequest): request is Request<PathParams> {
    return request.method === LOAD_MODEL;
}

function pluralizeReqStr(count: number) {
    return `request${count > 1 ? 's' : ''}`;
}


interface Props extends WithStyles<typeof styles> {
    disabled?: boolean;
}

interface State {
    requests: AnyRequest[];
    activeRequests: ProgressEvent[];
    doneRequests: AnyRequest[];
    canceledRequests: AnyRequest[];
    expandNotifications?: boolean;
    closeNotifications?: boolean;
}

type AnyRequest = Request<SnapshotParams
    | UploadParams
    | PathParams
>;
