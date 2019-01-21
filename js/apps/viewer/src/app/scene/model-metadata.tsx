import React, {PureComponent} from 'react';

import {
    entries,
    isString,
    keys,
    startCase
} from 'lodash';

import Button from '@material-ui/core/Button';
import Dialog from '@material-ui/core/Dialog';
import DialogActions from '@material-ui/core/DialogActions';
import DialogContent from '@material-ui/core/DialogContent';
import DialogTitle from '@material-ui/core/DialogTitle';
import List from '@material-ui/core/List';
import ListItem from '@material-ui/core/ListItem';
import ListItemText from '@material-ui/core/ListItemText';
import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';
import Typography from '@material-ui/core/Typography';

import {ModelInfo} from './types';


const styles = (theme: Theme) => createStyles({
    dialogPaper: {
        [theme.breakpoints.up('xs')]: {
            minWidth: 400
        }
    },
    title: {
        display: 'block',
        maxWidth: 350,
        textOverflow: 'ellipsis',
        overflow: 'hidden'
    }
});

const style = withStyles(styles);

class ModelMetadata extends PureComponent<Props> {
    render() {
        const {
            classes,
            open,
            onClose,
            name,
            metadata
        } = this.props;
        const items = toList(metadata);
        return (
            <Dialog
                open={open}
                classes={{paper: classes.dialogPaper}}
                onClose={onClose}
            >
                <DialogTitle>
                    <span className={classes.title}>{name}</span>
                </DialogTitle>
                {items}
                <DialogActions>
                    <Button onClick={onClose}>
                        Close
                    </Button>
                </DialogActions>
            </Dialog>
        );
    }
}

export default style(ModelMetadata);


function toList(metadata?: object) {
    const hasMetadata = keys(metadata).length > 0;
    if (hasMetadata) {
        const items = entries(metadata)
            .map(toListItem);
        return (
            <List>
                {items}
            </List>
        );
    }
    return (
        <DialogContent>
            <Typography>
                This model has no metadata
            </Typography>
        </DialogContent>
    );
}

function toListItem([key, value]: Entry) {
    const v = getValue(value);
    const name = startCase(key);
    return (
        <ListItem key={key}>
            <ListItemText primary={name} secondary={v} />
        </ListItem>
    );
}

function getValue(value: any) {
    if (isString(value)) {
        return value.length > 0 ? value : (<span>&mdash;</span>);
    } else if (value === true) {
        return 'YES';
    } else if (value === false) {
        return 'NO';
    }
    return '';
}


interface Props extends Partial<ModelInfo>, WithStyles<typeof styles> {
    open: boolean;
    onClose?(): void;
}

interface Entry extends Array<any> {
    0: string;
    1: any;
    length: 2;
}
