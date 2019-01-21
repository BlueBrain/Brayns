// tslint:disable: object-literal-key-quotes
import classNames from 'classnames';
import React, {MouseEvent, PureComponent} from 'react';

import {
    createStyles,
    Theme,
    WithStyles,
    withStyles
} from '@material-ui/core/styles';
import Typography from '@material-ui/core/Typography';


const styles = (theme: Theme) => createStyles({
    root: {
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'center',
        textTransform: 'uppercase',
        transition: 'background-color .1s ease-in-out',
        border: '2px solid #fff',
        backgroundColor: 'rgba(33, 33, 33, .95)',
        color: '#fff',
        cursor: 'pointer',
        '&:hover': {
            backgroundColor: '#fff',
            color: '#222'
        }
    },
    label: {
        transition: 'color .1s ease-in-out',
        fontSize: '0.7rem'
    }
});

const style = withStyles(styles);

export class Face extends PureComponent<Props> {
    render() {
        const {classes, className, label, size, onClick} = this.props;
        const style = {width: size, height: size};
        return (
            <div
                onClick={onClick}
                className={classNames(classes.root, className)}
                style={style}
            >
                <Typography
                    variant="body1"
                    className={classes.label}
                    color="inherit"
                >
                    {label}
                </Typography>
            </div>
        );
    }
}

export default style(Face);


interface Props extends WithStyles<typeof styles> {
    className?: string;
    label?: string;
    size: number;
    onClick?(evt: MouseEvent<HTMLDivElement>): void;
}
