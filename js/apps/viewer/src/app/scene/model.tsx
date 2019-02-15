import React, {PureComponent} from 'react';

import classNames from 'classnames';

import Checkbox from '@material-ui/core/Checkbox';
import Fade from '@material-ui/core/Fade';
import IconButton from '@material-ui/core/IconButton';
import ListItem from '@material-ui/core/ListItem';
import ListItemSecondaryAction from '@material-ui/core/ListItemSecondaryAction';
import ListItemText from '@material-ui/core/ListItemText';
import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';
import Tooltip from '@material-ui/core/Tooltip';
import EditIcon from '@material-ui/icons/Edit';
import MyLocationIcon from '@material-ui/icons/MyLocation';
import InfoIcon from '@material-ui/icons/Info';

import {TOOLTIP_DELAY} from '../../common/constants';
import {ModelId} from './types';


const styles = (theme: Theme) => createStyles({
    model: {
        transition: theme.transitions.create('padding', {
            easing: theme.transitions.easing.easeOut,
            duration: theme.transitions.duration.enteringScreen
        })
    },
    modelShift: {
        paddingRight: 148,
        transition: theme.transitions.create('padding', {
            easing: theme.transitions.easing.sharp,
            duration: 100
        })
    },
    modelActions: {
        display: 'flex'
    },
    hideFocus: {
        backgroundColor: 'transparent !important'
    },
    ellipsis: {
        textOverflow: 'ellipsis',
        whiteSpace: 'nowrap',
        overflow: 'hidden'
    }
});

// https://material-ui-next.com/guides/typescript/
const style = withStyles(styles);


export class Model extends PureComponent<Props, State> {
    state: State = {
        showActions: false,
        showButtonFocus: true
    };

    toggleChecked = () => {
        const {id, onSelectChange} = this.props;
        const {showButtonFocus} = this.state;
        if (showButtonFocus && onSelectChange) {
            onSelectChange(id);
        }
    }

    showEditor = () => {
        const {id, onEdit} = this.props;
        if (onEdit) {
            onEdit(id);
        }
    }

    showInfo = () => {
        const {id, onShowInfo} = this.props;
        if (onShowInfo) {
            onShowInfo(id);
        }
    }

    focus = () => {
        const {id, onFocus} = this.props;
        if (onFocus) {
            onFocus(id);
        }
    }

    toggleActionsVisibility = () => {
        this.setState(state => ({
            showActions: !state.showActions
        }));
    }

    toggleButtonFocus = () => {
        this.setState(state => ({
            showButtonFocus: !state.showButtonFocus
        }));
    }

    render() {
        const {
            classes,
            disabled,
            name,
            path,
            checked,
            boundingBox,
            hasMetadata,
            visible
        } = this.props;
        const {showActions, showButtonFocus} = this.state;

        const itemClasses = {
            [classes.modelShift]: showActions,
            [classes.hideFocus]: !showButtonFocus
        };

        const editAction = (
            <Tooltip title={'Edit'} placement="left" {...TOOLTIP_DELAY}>
                <div>
                    <IconButton
                        onClick={this.showEditor}
                        onMouseEnter={this.toggleButtonFocus}
                        onMouseLeave={this.toggleButtonFocus}
                        disabled={disabled}
                        aria-label="Edit model"
                    >
                        <EditIcon />
                    </IconButton>
                </div>
            </Tooltip>
        );

        const infoAction = hasMetadata ? (
            <Tooltip title={'Show info'} placement="bottom" {...TOOLTIP_DELAY}>
                <div>
                    <IconButton
                        onClick={this.showInfo}
                        onMouseEnter={this.toggleButtonFocus}
                        onMouseLeave={this.toggleButtonFocus}
                        disabled={disabled}
                        aria-label="Show model info"
                    >
                        <InfoIcon />
                    </IconButton>
                </div>
            </Tooltip>
        ) : null;

        const focusAction = (
            <Tooltip title={'Focus'} placement="bottom" {...TOOLTIP_DELAY}>
                <div>
                    <IconButton
                        onClick={this.focus}
                        onMouseEnter={this.toggleButtonFocus}
                        onMouseLeave={this.toggleButtonFocus}
                        disabled={disabled || (!visible && !boundingBox)}
                        aria-label="Focus model"
                    >
                        <MyLocationIcon />
                    </IconButton>
                </div>
            </Tooltip>
        );

        return (
            <ListItem
                onClick={this.toggleChecked}
                onMouseOver={this.toggleActionsVisibility}
                onMouseOut={this.toggleActionsVisibility}
                className={classNames(classes.model, itemClasses)}
                disabled={disabled}
                disableRipple={!showButtonFocus}
                disableTouchRipple={!showButtonFocus}
                button
            >
                <Checkbox
                    onChange={this.toggleChecked}
                    checked={checked}
                    tabIndex={-1}
                    disableRipple
                />
                <ListItemText
                    primary={name}
                    secondary={path}
                    classes={{primary: classes.ellipsis, secondary: classes.ellipsis}}
                />
                <Fade in={showActions} unmountOnExit>
                    <ListItemSecondaryAction className={classes.modelActions}>
                        {editAction}
                        {focusAction}
                        {infoAction}
                    </ListItemSecondaryAction>
                </Fade>
            </ListItem>
        );
    }
}


export default style(Model);


interface Props extends WithStyles<typeof styles> {
    id: ModelId;
    name: string;
    path: string;
    checked?: boolean;
    boundingBox?: boolean;
    visible?: boolean;
    hasMetadata?: boolean;
    disabled?: boolean;
    onSelectChange?(id: ModelId): void;
    onFocus?(id: ModelId): void;
    onShowInfo?(id: ModelId): void;
    onEdit?(id: ModelId): void;
}

interface State {
    showActions?: boolean;
    showButtonFocus?: boolean;
}
