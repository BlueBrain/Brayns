import React, {PureComponent} from 'react';

import Snackbar, {SnackbarOrigin} from '@material-ui/core/Snackbar';
import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';

import {Notification, onNotification} from '../../common/events';


const NOTIFICATION_ID = 'notification';
const SNACKBAR_POSITION: SnackbarOrigin = {
    vertical: 'bottom',
    horizontal: 'left'
};


const styles = (theme: Theme) => createStyles({
    notifications: {
        position: 'absolute',
        display: 'flex',
        flexDirection: 'column',
        alignItems: 'flex-end',
        right: theme.spacing.unit * 2,
        bottom: 0,
        zIndex: 1024 * 2
    },
    notification: {
        marginBottom: theme.spacing.unit * 2
    },
    close: {
        width: theme.spacing.unit * 4,
        height: theme.spacing.unit * 4
    }
});

// https://material-ui-next.com/guides/typescript/
const style = withStyles(styles);


// TODO: Dismiss current notification when a new one arrives
class Notifications extends PureComponent<Props> {
    state: State = {};

    closeSnackBar = () => {
        this.setState({
            showNotification: false
        });
    }

    componentDidMount() {
        onNotification()
            .subscribe(notification => {
                this.setState({
                    notification,
                    showNotification: true
                });
            });
    }

    render() {
        const {notification, showNotification} = this.state;
        const message = getNotificationMessage(notification);
        const messageNode = (<span id={NOTIFICATION_ID}>{message}</span>);
        return (
            <Snackbar
                anchorOrigin={SNACKBAR_POSITION}
                open={!!showNotification}
                onClose={this.closeSnackBar}
                ContentProps={{'aria-describedby': NOTIFICATION_ID}}
                message={messageNode}
            />
        );
    }
}


export default style(Notifications);


function getNotificationMessage(notification?: Notification) {
    if (notification) {
        return notification.message;
    }
    return '';
}

type Props = WithStyles<typeof styles>;

interface State {
    showNotification?: boolean;
    notification?: Notification;
}
