import React, {
    ChangeEvent,
    PureComponent,
    ReactElement
} from 'react';

import {isFunction} from 'lodash';

import Button from '@material-ui/core/Button';
import Dialog from '@material-ui/core/Dialog';
import DialogActions from '@material-ui/core/DialogActions';
import DialogTitle from '@material-ui/core/DialogTitle';
import {IconProps} from '@material-ui/core/Icon';
import List from '@material-ui/core/List';
import ListItem from '@material-ui/core/ListItem';
import ListItemIcon from '@material-ui/core/ListItemIcon';
import ListItemSecondaryAction from '@material-ui/core/ListItemSecondaryAction';
import ListItemText from '@material-ui/core/ListItemText';
import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';
import Switch from '@material-ui/core/Switch';


const styles = (theme: Theme) => createStyles({
    paper: {
        [theme.breakpoints.up('xs')]: {
            minWidth: 400
        }
    }
});

const style = withStyles(styles);


export class AppSettings extends PureComponent<Props> {
    updatePreference = (preference: Preference) => (evt: ChangeEvent<HTMLInputElement>, checked: boolean) => {
        const {onUpdate} = this.props;
        if (onUpdate) {
            onUpdate(preference, checked);
        }
    }

    closeDialog = () => {
        this.props.onClose();
    }

    render() {
        const {classes, open, preferences} = this.props;

        const items = preferences.map(preference => (
            <ListItem key={preference.key}>
                <ListItemIcon>
                    {isFunction(preference.icon) ? preference.icon(preference.checked) : preference.icon}
                </ListItemIcon>
                <ListItemText primary={preference.label} />
                <ListItemSecondaryAction>
                    <Switch
                        onChange={this.updatePreference(preference)}
                        checked={preference.checked}
                    />
                </ListItemSecondaryAction>
            </ListItem>
        ));

        return (
            <Dialog
                open={!!open}
                onClose={this.closeDialog}
                aria-labelledby="app-settings-title"
                classes={{paper: classes.paper}}
            >
                <DialogTitle id="app-settings-title">App settings</DialogTitle>
                <List>
                    {items}
                </List>
                <DialogActions>
                    <Button onClick={this.closeDialog}>
                        Close
                    </Button>
                </DialogActions>
            </Dialog>
        );
    }
}

export default style(AppSettings);


interface Props extends WithStyles<typeof styles> {
    open?: boolean;
    preferences: Preference[];
    onUpdate?(preference: Preference, checked?: boolean): void;
    onClose(): void;
}

type PreferenceIconFn = (checked?: boolean) => ReactElement<IconProps>;

export interface Preference {
    key: string;
    label?: string;
    icon: ReactElement<IconProps> | PreferenceIconFn;
    checked?: boolean;
}
