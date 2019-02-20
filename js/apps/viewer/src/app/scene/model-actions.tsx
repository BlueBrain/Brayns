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
import VisibilityIcon from '@material-ui/icons/Visibility';
import VisibilityOffIcon from '@material-ui/icons/VisibilityOff';

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


export class ModelActions extends PureComponent<Props> {
    toggleVisibility = () => {
        const {onVisibilityChange} = this.props;
        if (onVisibilityChange) {
            onVisibilityChange(!!this.props.nextVisibility);
        }
    }

    render() {
        const {
            classes,
            open,
            count,
            nextVisibility,
            onClear,
            onClose
        } = this.props;

        const visibilityIcon = nextVisibility ? (<VisibilityIcon />) : (<VisibilityOffIcon />);
        const visibilityActionText = nextVisibility ? 'Show models' : 'Hide models';

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
                        <Tooltip title={visibilityActionText} {...TOOLTIP_DELAY}>
                            <IconButton
                                onClick={this.toggleVisibility}
                                aria-label="Toggle model visibility"
                            >
                                {visibilityIcon}
                            </IconButton>
                        </Tooltip>
                        <Tooltip title={'Delete models'} {...TOOLTIP_DELAY}>
                            <IconButton
                                color="secondary"
                                onClick={onClear}
                                aria-label="Delete models"
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


export default style(ModelActions);


interface Props extends WithStyles<typeof styles> {
    count?: number;
    open?: boolean;
    nextVisibility?: boolean;
    onVisibilityChange?(visibility: boolean): void;
    onClear?(): void;
    onClose?(): void;
}
