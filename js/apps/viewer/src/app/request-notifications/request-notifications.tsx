import React, {PureComponent} from 'react';

import {
    LOAD_MODEL,
    SNAPSHOT,
    UPLOAD_MODEL
} from 'brayns';
import {Request} from 'rockets-client';
import {animationFrameScheduler, Subscription} from 'rxjs';
import {observeOn} from 'rxjs/operators';

import {PropTypes} from '@material-ui/core';
import Avatar from '@material-ui/core/Avatar';
import Badge from '@material-ui/core/Badge';
import IconButton from '@material-ui/core/IconButton';
import LinearProgress from '@material-ui/core/LinearProgress';
import List from '@material-ui/core/List';
import ListItem from '@material-ui/core/ListItem';
import ListItemSecondaryAction from '@material-ui/core/ListItemSecondaryAction';
import ListItemText from '@material-ui/core/ListItemText';
import Popover from '@material-ui/core/Popover';
import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';
import Tooltip from '@material-ui/core/Tooltip';
import Typography from '@material-ui/core/Typography';
import CloudUploadIcon from '@material-ui/icons/CloudUpload';
import DeleteIcon from '@material-ui/icons/DeleteForever';
import NotificationsIcon from '@material-ui/icons/Notifications';
import NotificationsNoneIcon from '@material-ui/icons/NotificationsNone';
import PhotoCameraIcon from '@material-ui/icons/PhotoCamera';
import RadioButtonUncheckedIcon from '@material-ui/icons/RadioButtonUnchecked';

import {TOOLTIP_DELAY} from '../../common/constants';
import {
    dispatchRequestCancel,
    onRequestComplete,
    onRequestProgress,
    onRequestStart,
    ProgressEvent
} from '../../common/events';


const styles = (theme: Theme) => createStyles({
    notifications: {
        width: 360
    },
    cancelAction: {
        paddingRight: 40
    },
    progress: {
        marginTop: theme.spacing.unit / 1.5,
        maxWidth: '90%',
        height: 4
    }
});


// https://material-ui-next.com/guides/typescript/
const style = withStyles(styles);


class Notifications extends PureComponent<Props, State> {
    state: State = {
        requests: [],
        active: [],
        canceled: []
    };

    anchor: any;

    private subs: Subscription[] = [];

    setAnchor = (node: any) => {
        this.anchor = node;
    }

    showNotifications = () => {
        if (this.state.active.length > 0) {
            this.setState({
                showNotifications: true
            });
        }
    }
    hideNotifications = () => {
        this.setState({
            showNotifications: false
        });
    }

    cancelRequest = (request: Request) => () => {
        dispatchRequestCancel(request);

        this.setState(state => {
            const index = state.canceled.findIndex(r => r.id === request.id);
            if (index !== -1) {
                return {} as any;
            }

            return {
                canceled: [
                    ...this.state.canceled,
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
                        requests.push(request);
                        return {requests};
                    });
                }),
            onRequestProgress()
                .pipe(observeOn(animationFrameScheduler))
                .subscribe(progress => {
                    this.setState(state => {
                        const active = [...state.active];

                        const index = active.findIndex(p => p.id === progress.id);
                        if (index !== -1) {
                            active.splice(index, 1, progress);
                        } else {
                            active.push(progress);
                        }

                        return {active};
                    });
                }),
            onRequestComplete()
                .subscribe(request => {
                    this.setState(state => {
                        const active = removeMatch(request, state.active);
                        const canceled = removeMatch(request, state.canceled);
                        const requests = removeMatch(request, state.requests);
                        const showNotifications = active.length === 0 && canceled.length === 0
                            ? false
                            : state.showNotifications;
                        return {
                            requests,
                            active,
                            canceled,
                            showNotifications
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
        const {classes, /* color,  */disabled} = this.props;
        const {
            requests,
            active,
            canceled,
            showNotifications
        } = this.state;

        const notifications = renderNotifications({
            active,
            requests,
            canceled,
            classes,
            cancelRequest: this.cancelRequest
        });
        const count = notifications.length;
        const btnColor = count ? 'inherit' : 'default';
        const badge = count ? (
            <Badge badgeContent={count} color="secondary">
                <NotificationsIcon />
            </Badge>
        ) : (<NotificationsNoneIcon />);

        const canShowNotifications = !!showNotifications && count > 0;

        return (
            <div>
                <Tooltip title="Notifications" placement="bottom" {...TOOLTIP_DELAY}>
                    <div>
                        <IconButton
                            buttonRef={this.setAnchor}
                            onClick={this.showNotifications}
                            color={btnColor}
                            disabled={disabled}
                            aria-label="Show pending requests"
                        >
                            {badge}
                        </IconButton>
                    </div>
                </Tooltip>

                <Popover
                    open={canShowNotifications}
                    anchorEl={this.anchor}
                    onClose={this.hideNotifications}
                    classes={{paper: classes.notifications}}
                    anchorOrigin={{vertical: 'top', horizontal: 'right'}}
                    transformOrigin={{vertical: 'top', horizontal: 'right'}}
                >
                    <List>
                        {notifications}
                    </List>
                </Popover>
            </div>
        );
    }
}


export default style(Notifications);


function renderNotifications({
    requests, active, canceled,
    classes,
    cancelRequest
}: {
    active: ProgressEvent[];
    requests: Request[];
    canceled: Request[];
    classes: Record<any, string>;
    cancelRequest(request: Request): () => void;
}) {
    const items = requests.map(request => {
        const key = `${request.id}`;
        const requestFinder = createRequestFinder(request);

        const icon = getIcon(request);
        const disabled = !!canceled.find(requestFinder);

        const progress = active.find(requestFinder);
        const amount = getProgressAmount(progress);
        const operation = getProgressMessage(progress, disabled);

        const variant = amount >= 100 || disabled ? 'indeterminate' : 'determinate';
        const progressItem = (
            <LinearProgress
                className={classes.progress}
                variant={variant}
                color="primary"
                value={amount}
            />
        );

        const message = (
            <Typography
                id={key}
                variant="subtitle1"
                noWrap
            >
                {operation}
            </Typography>
        );

        return (
            <ListItem key={key} classes={{secondaryAction: classes.cancelAction}}>
                <Avatar>{icon}</Avatar>
                <ListItemText primary={message} secondary={progressItem} disableTypography/>
                <ListItemSecondaryAction>
                    <IconButton
                        color="secondary"
                        onClick={cancelRequest(request)}
                        disabled={disabled}
                        aria-label="Cancel request"
                    >
                        <DeleteIcon />
                    </IconButton>
                </ListItemSecondaryAction>
            </ListItem>
        );
    });

    return items;
}

function removeMatch<T extends {id: string | number}>(request: Request, items: T[]) {
    const index = items.findIndex(item => item.id === request.id);
    if (index !== -1) {
        const copy = [...items];
        copy.splice(index, 1);
        return copy;
    }
    return items;
}

function createRequestFinder(request: Request) {
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

function getProgressMessage(progress?: ProgressEvent, disabled?: boolean) {
    if (disabled) {
        return 'Canceling ...';
    }

    if (progress) {
        const {amount, operation} = progress;
        if (amount >= 1) {
            return 'Downloading ...';
        }
        return operation;
    }

    return 'Pending ...';
}

function getIcon(request?: Request) {
    const empty = (<RadioButtonUncheckedIcon />);
    if (!request) {
        return empty;
    }
    const {method} = request;
    switch (method) {
        case UPLOAD_MODEL:
        case LOAD_MODEL:
            return (
                <CloudUploadIcon />
            );
        case SNAPSHOT:
            return (
                <PhotoCameraIcon />
            );
        default:
            return empty;
    }
}


interface Props extends WithStyles<typeof styles> {
    color?: PropTypes.Color;
    disabled?: boolean;
}

interface State {
    requests: Request[];
    active: ProgressEvent[];
    canceled: Request[];
    showNotifications?: boolean;
}
