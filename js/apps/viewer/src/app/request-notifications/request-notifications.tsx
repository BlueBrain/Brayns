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
import ErrorIcon from '@material-ui/icons/ErrorOutline';
import ExpandMoreIcon from '@material-ui/icons/ExpandMore';
import PhotoCameraIcon from '@material-ui/icons/PhotoCamera';
import WarningIcon from '@material-ui/icons/WarningRounded';

import {withAnimation, WithAnimation} from '../../common/client';
import {
    dispatchRequestCancel,
    onRequestDone,
    onRequestProgress,
    onRequestStart,
    ProgressEvent,
    RequestDoneEvent
} from '../../common/events';


const REQUEST_PENDING = 0;
const REQUEST_IN_PROGRESS = 1;
const REQUEST_DONE = 2;
const REQUEST_IS_CANCELING = 3;
const REQUEST_CANCELED = 4;
const REQUEST_ERRORED = 5;


const styles = (theme: Theme) => createStyles({
    root: {
        position: 'absolute',
        bottom: theme.spacing.unit * 2,
        right: theme.spacing.unit * 2,
        width: 360
    },
    rootShift: {
        marginBottom: 94
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
    requestName: {
        textOverflow: 'ellipsis',
        whiteSpace: 'nowrap',
        overflow: 'hidden'
    },
    progressContainer: {
        position: 'relative'
    },
    progress: {
        position: 'absolute',
        top: 12,
        left: 12,
        zIndex: -2
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
        progressEvents: [],
        requestDoneEvents: [],
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
            progressEvents: [],
            requestDoneEvents: [],
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
                        const active = [...state.progressEvents];

                        const index = active.findIndex(p => p.id === progress.id);
                        if (index !== -1) {
                            active.splice(index, 1, progress);
                        } else {
                            active.unshift(progress);
                        }

                        return {progressEvents: active};
                    });
                }),
            onRequestDone()
                .subscribe(evt => {
                    this.setState(state => {
                        const copy = [...state.requestDoneEvents];
                        const index = copy.findIndex(e => e.request.id === evt.request.id);
                        if (index !== -1) {
                            return null;
                        }
                        return {
                            requestDoneEvents: [
                                ...copy,
                                evt
                            ]
                        };
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
        const {classes, hasAnimation} = this.props;
        const {
            requests,
            progressEvents,
            canceledRequests,
            requestDoneEvents,
            expandNotifications,
            closeNotifications
        } = this.state;

        const notificationsHeaderText = getNotificationsCount(progressEvents.length, requestDoneEvents.length);
        const notifications = renderNotifications({
            requests,
            progressEvents,
            requestDoneEvents,
            canceledRequests,
            classes,
            cancelRequest: this.cancelRequest
        });

        return (
            <Fade
                in={!closeNotifications}
                onExited={this.clearRequests}
                unmountOnExit
            >
                <div className={classNames(classes.root, {[classes.rootShift]: hasAnimation})}>
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


export default style(
    withAnimation(Notifications));


function renderNotifications({
    requests, progressEvents, requestDoneEvents, canceledRequests,
    classes,
    cancelRequest
}: {
    requests: AnyRequest[];
    progressEvents: ProgressEvent[];
    requestDoneEvents: RequestDoneEvent[];
    canceledRequests: AnyRequest[];
    classes: Record<any, string>;
    cancelRequest(request: AnyRequest): () => void;
}) {
    const items = requests.map(request => {
        const key = `${request.id}`;
        const name = getRequestName(request);
        const icon = getRequestTypeIcon(request);

        const progressEvt = progressEvents.find(evt => evt.id === request.id);
        const requestDoneEvt = requestDoneEvents.find(evt => evt.request.id === request.id);
        const canceledRequest = canceledRequests.find(evt => evt.id === request.id);
        const state = getRequestState(progressEvt, canceledRequest, requestDoneEvt);
        const message = getRequestStateMessage(state, progressEvt, requestDoneEvt);

        let action;
        const requestActionCls = state !== REQUEST_IN_PROGRESS ? classes.requestAction : '';
        switch (state) {
            case REQUEST_PENDING:
                action = (
                    <CircularProgress
                        variant="indeterminate"
                        color="primary"
                        thickness={4}
                        size={24}
                    />
                );
                break;
            case REQUEST_IN_PROGRESS:
                const amount = getProgressAmount(progressEvt);
                action = (
                    <div className={classes.progressContainer}>
                        <IconButton
                            color="inherit"
                            onClick={cancelRequest(request)}
                            aria-label="Cancel request"
                        >
                            <CancelIcon />
                        </IconButton>
                        <CircularProgress
                            className={classes.progress}
                            variant="static"
                            color="primary"
                            value={amount}
                            thickness={4}
                            size={24}
                        />
                    </div>
                );
                break;
            case REQUEST_IS_CANCELING:
                action = (
                    <CircularProgress
                        variant="indeterminate"
                        color="secondary"
                        thickness={4}
                        size={24}
                    />
                );
                break;
            case REQUEST_CANCELED:
                action = <WarningIcon color="disabled" />;
                break;
            case REQUEST_ERRORED:
                action = <ErrorIcon color="error" />;
                break;
            case REQUEST_DONE:
                action = <CheckIcon className={classes.successColor} />;
                break;
        }

        return (
            <ListItem key={key}>
                <Avatar>{icon}</Avatar>
                <ListItemText
                    primary={name}
                    secondary={message}
                    classes={{primary: classes.requestName}}
                />
                <ListItemSecondaryAction className={requestActionCls}>
                    {action}
                </ListItemSecondaryAction>
            </ListItem>
        );
    });

    return items;
}

function getNotificationsCount(progressEventsCount: number, requestDoneEventsCount: number) {
    const pendingCount = progressEventsCount - requestDoneEventsCount;
    const hasPending = pendingCount > 0;
    const requestsStr = hasPending ? '' : ` ${pluralizeReqStr(requestDoneEventsCount)}`;
    const requestsDoneStr = `${requestDoneEventsCount}${requestsStr} complete`;

    if (hasPending) {
        return `${pendingCount} pending ${pluralizeReqStr(pendingCount)}${requestDoneEventsCount > 0 ? `, ${requestsDoneStr}` : ''}`;
    }

    return requestsDoneStr;
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

function getRequestState(progress?: ProgressEvent, canceledRequest?: AnyRequest, requestDoneEvt?: RequestDoneEvent) {
    const isDone = !!requestDoneEvt;
    const wasCanceled = !!canceledRequest;
    const isCanceling = wasCanceled && !isDone;
    const hasError = requestDoneEvt && !!requestDoneEvt.error;

    if (isCanceling) {
        return REQUEST_IS_CANCELING;
    }

    if (progress && progress.amount < 1) {
        return REQUEST_IN_PROGRESS;
    }

    if (wasCanceled) {
        return REQUEST_CANCELED;
    }

    if (hasError) {
        return REQUEST_ERRORED;
    }

    if (isDone) {
        return REQUEST_DONE;
    }

    return REQUEST_PENDING;
}

function getRequestStateMessage(state: number, progress?: ProgressEvent, requestDoneEvt?: RequestDoneEvent) {
    switch (state) {
        case REQUEST_PENDING:
            return 'Pending ...';
        case REQUEST_IN_PROGRESS:
            return progress!.operation;
        case REQUEST_IS_CANCELING:
            return 'Canceling ...';
        case REQUEST_CANCELED:
            return 'Canceled';
        case REQUEST_ERRORED:
            return requestDoneEvt && requestDoneEvt.error
                ? requestDoneEvt.error.message
                : 'Failed';
        case REQUEST_DONE:
        default:
            return 'Done';
    }
}

function getRequestName(request: AnyRequest) {
    return isSnapshot(request)
        ? request.params!.name
        : isLoadFromPath(request)
            ? request.params!.path
            : isUpload(request)
                ? `${request.params!.name}.${request.params!.type}`
                : '';
}

function getRequestTypeIcon(request: AnyRequest) {
    if (isUpload(request) || isLoadFromPath(request)) {
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

function isLoadFromPath(request: AnyRequest): request is Request<PathParams> {
    return request.method === LOAD_MODEL;
}

function pluralizeReqStr(count: number) {
    return `request${count > 1 ? 's' : ''}`;
}


type Props = WithStyles<typeof styles>
    & WithAnimation;

interface State {
    requests: AnyRequest[];
    progressEvents: ProgressEvent[];
    requestDoneEvents: RequestDoneEvent[];
    canceledRequests: AnyRequest[];
    expandNotifications?: boolean;
    closeNotifications?: boolean;
}

type AnyRequest = Request<SnapshotParams
    | UploadParams
    | PathParams
>;
