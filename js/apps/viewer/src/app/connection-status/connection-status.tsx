import React, {PureComponent} from 'react';

import {hex as fromHex} from 'chroma-js';

import Fade from '@material-ui/core/Fade';
import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';
import Typography from '@material-ui/core/Typography';
import CloudOffIcon from '@material-ui/icons/CloudOff';


const styles = (theme: Theme) => {
    const [r, g, b] = fromHex(theme.palette.background.paper)
        .rgb();
    const rgba = `rgba(${r}, ${g}, ${b}, .98)`;

    return createStyles({
        overlay: {
            position: 'absolute',
            display: 'flex',
            flexDirection: 'column',
            flex: 1,
            top: 0,
            right: 0,
            bottom: 0,
            left: 0,
            backgroundColor: rgba,
            alignItems: 'center',
            justifyContent: 'center',
            zIndex: 1024
        },
        content: {
            display: 'flex',
            flexDirection: 'column',
            alignItems: 'center'
        },
        icon: {
            color: theme.palette.text.secondary
        },
        text: {
            paddingTop: theme.spacing.unit,
            color: theme.palette.text.secondary
        }
    });
};


// https://material-ui-next.com/guides/typescript/
const style = withStyles(styles);


class ConnectionStatus extends PureComponent<Props> {
    render() {
        const {classes, open} = this.props;
        const overlay = open ? (
            <div className={classes.content}>
                <CloudOffIcon className={classes.icon} />
                <Typography align="center" className={classes.text}>
                    Unable to connect to remote renderer
                </Typography>
            </div>
        ) : null;

        return (
            <Fade in={open} unmountOnExit>
                <div className={classes.overlay}>
                    {overlay}
                </div>
            </Fade>
        );
    }
}


export default style(ConnectionStatus);


interface Props extends WithStyles<typeof styles> {
    open?: boolean;
}
