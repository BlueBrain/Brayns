// tslint:disable: member-ordering
import React, {
    MouseEvent,
    PureComponent,
    ReactElement
} from 'react';

import classNames from 'classnames';
import {memoize} from 'lodash';

import {PropTypes} from '@material-ui/core';
import Checkbox from '@material-ui/core/Checkbox';
import Fade from '@material-ui/core/Fade';
import {IconProps} from '@material-ui/core/Icon';
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
import DeleteForeverIcon from '@material-ui/icons/DeleteForever';
import InfoIcon from '@material-ui/icons/Info';
import MyLocationIcon from '@material-ui/icons/MyLocation';
import VisibilityOnIcon from '@material-ui/icons/Visibility';
import VisibilityOffIcon from '@material-ui/icons/VisibilityOff';

import {TOOLTIP_DELAY} from '../../common/constants';
import {ModelId} from './types';
import {composeEvtHandler} from './utils';


const styles = (theme: Theme) => createStyles({
    model: {
        transition: theme.transitions.create('padding', {
            easing: theme.transitions.easing.easeOut,
            duration: theme.transitions.duration.enteringScreen
        })
    },
    modelShift: {
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

const style = withStyles(styles);


export class Model extends PureComponent<Props, State> {
    state: State = {
        showActions: false
    };

    getListItemStyle = memoize((showActions, hasMetadata) => (showActions ? {
        paddingRight: 48 * (hasMetadata ? 4 : 3)
    } : undefined));

    showActions = () => this.setState({showActions: true});
    hideActions = () => this.setState({showActions: false});

    enableRipple = () => this.setState({disableRipple: false});
    disableRipple = () => this.setState({disableRipple: true});

    toggleChecked = composeEvtHandler(() => {
        const {id, onSelectChange} = this.props;
        if (onSelectChange) {
            onSelectChange(id);
        }
    });

    showEditor = composeEvtHandler(() => {
        const {id, onEdit} = this.props;
        if (onEdit) {
            onEdit(id);
        }
    });

    showInfo = composeEvtHandler(() => {
        const {id, onShowInfo} = this.props;
        if (onShowInfo) {
            onShowInfo(id);
        }
    });

    focus = composeEvtHandler(() => {
        const {id, onFocus} = this.props;
        if (onFocus) {
            onFocus(id);
        }
    });

    hide = composeEvtHandler(() => {
        const {id, onVisibilityChange} = this.props;
        if (onVisibilityChange) {
            onVisibilityChange(id, false);
        }
    });

    show = composeEvtHandler(() => {
        const {id, onVisibilityChange} = this.props;
        if (onVisibilityChange) {
            onVisibilityChange(id, true);
        }
    });

    delete = () => {
        const {id, onDelete} = this.props;
        if (onDelete) {
            onDelete(id);
        }
    }

    createAction = memoize(
        ({title, ariaLabel, cb, icon, ...rest}: ActionProps) => {
            return (
                <Tooltip title={title} {...TOOLTIP_DELAY}>
                    <div>
                        <IconButton
                            onClick={cb}
                            onMouseOver={this.disableRipple}
                            onMouseOut={this.enableRipple}
                            aria-label={ariaLabel}
                            {...rest}
                        >
                            {icon}
                        </IconButton>
                    </div>
                </Tooltip>
            );
        }
    );

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
        const {showActions, disableRipple} = this.state;

        const itemStyle = this.getListItemStyle(showActions, hasMetadata);
        const itemClasses = {[classes.modelShift]: showActions};

        const infoAction = hasMetadata ? this.createAction({
            disabled,
            title: 'Show info',
            ariaLabel: 'Show model info',
            icon: <InfoIcon />,
            cb: this.showInfo
        }) : null;

        const focusAction = this.createAction({
            title: 'Focus',
            ariaLabel: 'Focus model',
            icon: <MyLocationIcon />,
            cb: this.focus,
            disabled: disabled || (!visible && !boundingBox)
        });

        const visibilityAction = visible ? this.createAction({
            disabled,
            title: 'Hide',
            ariaLabel: 'Hide model',
            icon: <VisibilityOffIcon />,
            cb: this.hide
        }) : this.createAction({
            disabled,
            title: 'Show',
            ariaLabel: 'Show model',
            icon: <VisibilityOnIcon />,
            cb: this.show
        });

        const deleteAction = this.createAction({
            disabled,
            title: 'Delete',
            ariaLabel: 'Delete model',
            icon: <DeleteForeverIcon />,
            cb: this.delete,
            color: 'secondary'
        });

        return (
            <ListItem
                onClick={this.showEditor}
                onMouseOver={this.showActions}
                onMouseOut={this.hideActions}
                className={classNames(classes.model, itemClasses)}
                style={itemStyle}
                disabled={disabled}
                disableRipple={disableRipple}
                disableTouchRipple={disableRipple}
                button
            >
                <Checkbox
                    onMouseOver={this.disableRipple}
                    onMouseOut={this.enableRipple}
                    onClick={this.toggleChecked}
                    checked={checked}
                />
                <ListItemText
                    primary={name}
                    secondary={path}
                    classes={{primary: classes.ellipsis, secondary: classes.ellipsis}}
                />
                <Fade in={showActions} unmountOnExit>
                    <ListItemSecondaryAction className={classes.modelActions}>
                        {focusAction}
                        {infoAction}
                        {visibilityAction}
                        {deleteAction}
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
    onVisibilityChange?(id: ModelId, visible: boolean): void;
    onDelete?(id: ModelId): void;
}

interface State {
    showActions?: boolean;
    disableRipple?: boolean;
}

interface ActionProps {
    title: string;
    ariaLabel: string;
    icon: ReactElement<IconProps>;
    disabled?: boolean;
    color?: PropTypes.Color;
    cb(evt: MouseEvent): void;
}
