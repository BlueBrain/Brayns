import React, {PureComponent} from 'react';

import Avatar from '@material-ui/core/Avatar';
import Button from '@material-ui/core/Button';
import Dialog from '@material-ui/core/Dialog';
import DialogActions from '@material-ui/core/DialogActions';
import DialogTitle from '@material-ui/core/DialogTitle';
import Grid from '@material-ui/core/Grid';
import List from '@material-ui/core/List';
import ListItem from '@material-ui/core/ListItem';
import ListItemAvatar from '@material-ui/core/ListItemAvatar';
import ListItemText from '@material-ui/core/ListItemText';
import ListSubheader from '@material-ui/core/ListSubheader';
import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';
import withWidth, {isWidthDown, WithWidth} from '@material-ui/core/withWidth';
import ArrowBackIcon from '@material-ui/icons/ArrowBack';
import CloudIcon from '@material-ui/icons/Cloud';
import InsertDriveFileIcon from '@material-ui/icons/InsertDriveFile';
import LayersIcon from '@material-ui/icons/Layers';
import LinkIcon from '@material-ui/icons/Link';
import LocationSearchingIcon from '@material-ui/icons/LocationSearching';
import PhonelinkSetupIcon from '@material-ui/icons/PhonelinkSetup';
import PhotoCameraIcon from '@material-ui/icons/PhotoCamera';
import PlayIcon from '@material-ui/icons/PlayArrow';
import RefreshIcon from '@material-ui/icons/Refresh';
import SkipNextIcon from '@material-ui/icons/SkipNext';
import SkipPreviousIcon from '@material-ui/icons/SkipPrevious';


const styles = (theme: Theme) => createStyles({
    dialog: {
        [theme.breakpoints.up('xs')]: {
            minWidth: 400
        }
    }
});

const style = withStyles(styles);

export class Shortcuts extends PureComponent<Props> {
    closeDialog = () => {
        this.props.onClose();
    }

    render() {
        const {
            classes,
            open,
            width
        } = this.props;

        const fullScreen = isWidthDown('xs', width);
        const direction = fullScreen ? 'column' : 'row';

        const playerShortcuts = (
            <List subheader={<ListSubheader component="div">Player</ListSubheader>}>
                <ListItem>
                    <ListItemAvatar>
                        <Avatar>
                            <PlayIcon />
                        </Avatar>
                    </ListItemAvatar>
                    <ListItemText
                        primary="SPACE"
                        secondary="Play/Pause simulation"
                    />
                </ListItem>
                <ListItem>
                    <ListItemAvatar>
                        <Avatar>
                            <SkipPreviousIcon />
                        </Avatar>
                    </ListItemAvatar>
                    <ListItemText
                        primary="SHIFT + P"
                        secondary="Jump to previous frame"
                    />
                </ListItem>
                <ListItem>
                    <ListItemAvatar>
                        <Avatar>
                            <SkipNextIcon />
                        </Avatar>
                    </ListItemAvatar>
                    <ListItemText
                        primary="SHIFT + N"
                        secondary="Jump to next frame"
                    />
                </ListItem>
            </List>
        );

        const sceneManagement = (
            <List subheader={<ListSubheader component="div">Scene Management</ListSubheader>}>
                <ListItem>
                    <ListItemAvatar>
                        <Avatar>
                            <InsertDriveFileIcon />
                        </Avatar>
                    </ListItemAvatar>
                    <ListItemText
                        primary="CTRL/CMD + U"
                        secondary="Upload a model"
                    />
                </ListItem>
                <ListItem>
                    <ListItemAvatar>
                        <Avatar>
                            <LinkIcon />
                        </Avatar>
                    </ListItemAvatar>
                    <ListItemText
                        primary="CTRL/CMD + L"
                        secondary="Load a model"
                    />
                </ListItem>
                <ListItem>
                    <ListItemAvatar>
                        <Avatar>
                            <LayersIcon />
                        </Avatar>
                    </ListItemAvatar>
                    <ListItemText
                        primary="SHIFT + D"
                        secondary="Delete selected models/clip planes"
                    />
                </ListItem>
                <ListItem>
                    <ListItemAvatar>
                        <Avatar>
                            <ArrowBackIcon />
                        </Avatar>
                    </ListItemAvatar>
                    <ListItemText
                        primary="ESC"
                        secondary="Clear model/clip planes selection"
                    />
                </ListItem>
            </List>
        );

        const config = (
            <List subheader={<ListSubheader component="div">Config</ListSubheader>}>
                <ListItem>
                    <ListItemAvatar>
                        <Avatar>
                            <PhonelinkSetupIcon />
                        </Avatar>
                    </ListItemAvatar>
                    <ListItemText
                        primary="ALT + O"
                        secondary="App preferences"
                    />
                </ListItem>
            </List>
        );

        const misc = (
            <List subheader={<ListSubheader component="div">Misc</ListSubheader>}>
                <ListItem>
                    <ListItemAvatar>
                        <Avatar>
                            <RefreshIcon />
                        </Avatar>
                    </ListItemAvatar>
                    <ListItemText
                        primary="SHIFT + R"
                        secondary="Fly to center of scene"
                    />
                </ListItem>
                <ListItem>
                    <ListItemAvatar>
                        <Avatar>
                            <PhotoCameraIcon />
                        </Avatar>
                    </ListItemAvatar>
                    <ListItemText
                        primary="SHIFT + S"
                        secondary="Take a snapshot"
                    />
                </ListItem>
                <ListItem>
                    <ListItemAvatar>
                        <Avatar>
                            <LocationSearchingIcon />
                        </Avatar>
                    </ListItemAvatar>
                    <ListItemText
                        primary="CTRL/CMD + CLICK"
                        secondary="Inspect object in the scene"
                    />
                </ListItem>
                <ListItem>
                    <ListItemAvatar>
                        <Avatar>
                            <CloudIcon />
                        </Avatar>
                    </ListItemAvatar>
                    <ListItemText
                        primary="CTRL + Q"
                        secondary="Quit the remote renderer"
                    />
                </ListItem>
            </List>
        );

        return (
            <Dialog
                open={!!open}
                onClose={this.closeDialog}
                aria-labelledby="shortcuts-dialog-title"
                classes={{paper: classes.dialog}}
                fullScreen={fullScreen}
            >
                <DialogTitle id="shortcuts-dialog-title">{'Shortcuts'}</DialogTitle>
                <Grid container justify="flex-start" direction={direction}>
                    <Grid item xs>
                        {sceneManagement}
                    </Grid>
                    <Grid item xs>
                        {misc}
                    </Grid>
                </Grid>
                <Grid container justify="flex-start" direction={direction}>
                    <Grid item xs>
                        {playerShortcuts}
                    </Grid>
                    <Grid item xs>
                        {config}
                    </Grid>
                </Grid>
                <DialogActions>
                    <Button onClick={this.closeDialog}>
                        Close
                    </Button>
                </DialogActions>
            </Dialog>
        );
    }
}

export default withWidth()(
    style(Shortcuts)
);


interface Props extends WithStyles<typeof styles>, WithWidth {
    open?: boolean;
    onClose(): void;
}
