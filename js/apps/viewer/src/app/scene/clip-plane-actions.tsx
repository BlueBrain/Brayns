import React, {PureComponent} from 'react';

import AppBar from '@material-ui/core/AppBar';
import Fade from '@material-ui/core/Fade';
import IconButton from '@material-ui/core/IconButton';
import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';
import Toolbar from '@material-ui/core/Toolbar';
import Tooltip from '@material-ui/core/Tooltip';
import Typography from '@material-ui/core/Typography';
import ArrowBackIcon from '@material-ui/icons/ArrowBack';
import DeleteIcon from '@material-ui/icons/DeleteForever';

import {TOOLTIP_DELAY} from '../../common/constants';


const styles = (theme: Theme) => createStyles({
    root: {
        backgroundColor: theme.palette.background.paper,
        padding: theme.spacing.unit * 2,
        flex: 1
    },
    back: {
        marginRight: theme.spacing.unit * 2
    },
    spacer: {
        flex: 1
    }
});
const style = withStyles(styles);


export class ClipPlaneActions extends PureComponent<Props> {
    render() {
        const {
            classes,
            open,
            count,
            onClear,
            onClose
        } = this.props;

        return (
            <Fade in={open} unmountOnExit>
                <AppBar position="absolute" elevation={0} color="default">
                    <Toolbar variant="dense">
                        <IconButton
                            onClick={onClose}
                            className={classes.back}
                            aria-label="Dismiss edit mode"
                        >
                            <ArrowBackIcon />
                        </IconButton>
                        <Typography variant="subtitle1">{count}</Typography>
                        <span className={classes.spacer} />
                        <Tooltip title={'Delete clip planes'} placement="left" {...TOOLTIP_DELAY}>
                            <IconButton
                                color="secondary"
                                onClick={onClear}
                                aria-label="Delete clip planes"
                            >
                                <DeleteIcon />
                            </IconButton>
                        </Tooltip>
                    </Toolbar>
                </AppBar>
            </Fade>
        );
    }
}


export default style(ClipPlaneActions);


interface Props extends WithStyles<typeof styles> {
    count?: number;
    open?: boolean;
    onClear?(): void;
    onClose?(): void;
}
