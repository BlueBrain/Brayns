import React, {
    Component,
    MouseEvent
} from 'react';

import Button from '@material-ui/core/Button';
import Menu from '@material-ui/core/Menu';
import MenuItem from '@material-ui/core/MenuItem';
import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';
import CheckIcon from '@material-ui/icons/Check';


const frameStepId = 'frame-step';

const styles = (theme: Theme) => createStyles({
    menuItem: {
        position: 'relative'
    },
    icon: {
        position: 'absolute'
    },
    text: {
        paddingLeft: 26,
        paddingRight: 16
    }
});

const style = withStyles(styles);


class FrameStep extends Component<Props, State> {
    state: State = {
        anchorEl: null
    };

    openMenu = (evt: MouseEvent<HTMLElement>) => this.setState({anchorEl: evt.currentTarget});

    closeMenu = () => this.setState({anchorEl: null});

    changeFrameStep = (evt: MouseEvent<HTMLSelectElement>) => {
        const delta = Number(evt.currentTarget.value);
        this.closeMenu();
        this.props.onDeltaChange(delta);
    }

    render() {
        const {classes, delta, disabled} = this.props;
        const {anchorEl} = this.state;

        const steps = [16, 8, 4, 2, 1].map((step, id) => {
            const isCurrent = step === delta;
            const icon = isCurrent ? (<CheckIcon className={classes.icon} fontSize="small" />) : null;
            const text = (<span className={classes.text}>{stepLabel(step)}</span>);

            return (
                <MenuItem
                    className={classes.menuItem}
                    onClick={this.changeFrameStep}
                    selected={isCurrent}
                    disabled={disabled}
                    value={step}
                    key={id}
                >
                    {icon}
                    {text}
                </MenuItem>
            );
        });

        return (
            <div>
                <Button
                    aria-owns={anchorEl ? frameStepId : undefined}
                    aria-haspopup="true"
                    onClick={this.openMenu}
                >
                    {`Frame Step: ${stepLabel(delta ? delta : 1)}`}
                </Button>
                <Menu
                    id={frameStepId}
                    anchorEl={anchorEl}
                    open={Boolean(anchorEl)}
                    onClose={this.closeMenu}
                >
                    {steps}
                </Menu>
            </div>
        );
    }
}

export default style(FrameStep);


function stepLabel(value: number) {
    if (value === 1) {
        return 'Normal';
    }
    return value;
}


interface Props extends WithStyles<typeof styles> {
    delta?: number;
    disabled?: boolean;
    onDeltaChange(delta: number): void;
}

interface State {
    anchorEl: null | HTMLElement;
}

