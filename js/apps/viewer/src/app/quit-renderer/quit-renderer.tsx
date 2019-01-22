import React, {PureComponent} from 'react';

import {QUIT} from 'brayns';
import {Subscription} from 'rxjs';

import Button from '@material-ui/core/Button';
import Dialog from '@material-ui/core/Dialog';
import DialogActions from '@material-ui/core/DialogActions';
import DialogContent from '@material-ui/core/DialogContent';
import DialogContentText from '@material-ui/core/DialogContentText';
import DialogTitle from '@material-ui/core/DialogTitle';
import IconButton from '@material-ui/core/IconButton';
import Tooltip from '@material-ui/core/Tooltip';
import CloudOffIcon from '@material-ui/icons/CloudOff';

import brayns from '../../common/client';
import {KeyCode, TOOLTIP_DELAY} from '../../common/constants';
import {dispatchKeyboardLock, onKeyboardLockChange} from '../../common/events';


export default class QuitRenderer extends PureComponent<{}, State> {
    state: State = {
        showDialog: false
    };

    private keyboardLocked = false;
    private subs: Subscription[] = [];

    quitRenderer = () => {
        brayns.notify(QUIT);
        this.closeDialog();
    }

    quitOnKeydown = (evt: KeyboardEvent) => {
        if (this.state.isConnected
            && evt.keyCode === KeyCode.Q
            && evt.ctrlKey
            && !this.keyboardLocked) {
            evt.preventDefault();
            evt.stopPropagation();
            this.openDialog();
        }
    }

    openDialog = () => {
        dispatchKeyboardLock(true);
        this.setState({
            showDialog: true
        });
    }
    closeDialog = () => {
        dispatchKeyboardLock(false);
        this.setState({
            showDialog: false
        });
    }

    componentDidMount() {
        window.addEventListener('keydown', this.quitOnKeydown, false);
        this.subs.push(...[
            brayns.ready.subscribe(ready => {
                this.setState({
                    isConnected: ready
                });
            }),
            onKeyboardLockChange()
                .subscribe(locked => {
                    this.keyboardLocked = locked;
                })
        ]);
    }
    componentWillUnmount() {
        window.removeEventListener('keydown', this.quitOnKeydown);
        while (this.subs.length) {
            const sub = this.subs.pop();
            sub!.unsubscribe();
        }
    }

    render() {
        const {isConnected, showDialog} = this.state;
        const hasError = !isConnected;
        const title = hasError ? 'Renderer is offline' : 'Quit renderer';

        return (
            <div>
                <Tooltip title={title} placement="bottom" {...TOOLTIP_DELAY}>
                    <div>
                        <IconButton
                            onClick={this.openDialog}
                            color="secondary"
                            aria-label="Quit renderer"
                            disabled={hasError}
                        >
                            <CloudOffIcon />
                        </IconButton>
                    </div>
                </Tooltip>
                <Dialog
                    open={showDialog}
                    onClose={this.closeDialog}
                    aria-labelledby="quit-dialog-title"
                    aria-describedby="quit-dialog-description"
                >
                    <DialogTitle id="quit-dialog-title">{'Quit the renderer?'}</DialogTitle>
                    <DialogContent>
                        <DialogContentText id="quit-dialog-description">
                            You're about to quit the remote renderer. This action cannot be undone.
                            Are you sure about this?
                        </DialogContentText>
                    </DialogContent>
                    <DialogActions>
                        <Button onClick={this.closeDialog}>
                            Cancel
                        </Button>
                        <Button onClick={this.quitRenderer} color="primary" autoFocus>
                            Ok
                        </Button>
                    </DialogActions>
                </Dialog>
            </div>
        );
    }
}


interface State {
    isConnected?: boolean;
    showDialog: boolean;
}
