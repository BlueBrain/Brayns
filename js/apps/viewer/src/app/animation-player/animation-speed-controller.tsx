import React, {
    Component,
    MouseEvent
} from 'react';

import Button from '@material-ui/core/Button';
import Menu from '@material-ui/core/Menu';
import MenuItem from '@material-ui/core/MenuItem';
import Tooltip from '@material-ui/core/Tooltip';

import {TOOLTIP_DELAY} from '../../common/constants';


const animationSpeedId = 'animation-speed-controller';


export default class AnimationSpeedController extends Component<Props, State> {
    state: State = {
        anchorEl: null
    };

    openMenu = (evt: MouseEvent<HTMLElement>) => this.setState({anchorEl: evt.currentTarget});

    closeMenu = () => this.setState({anchorEl: null});

    changeAnimationSpeed = (evt: MouseEvent<HTMLSelectElement>) => {
        const delta = Number(evt.currentTarget.value);
        this.closeMenu();
        this.props.onDeltaChange(delta);
    }

    render() {
        const {delta, disabled} = this.props;
        const {anchorEl} = this.state;

        const speedItems = [8, 4, 2, 1].map((speed, id) => (
            <MenuItem
                onClick={this.changeAnimationSpeed}
                selected={speed === delta}
                disabled={disabled}
                value={speed}
                key={id}
            >
                {speedLabel(speed)}
            </MenuItem>
        ));

        return (
            <Tooltip title={'Animation speed'} placement="left" {...TOOLTIP_DELAY}>
                <div>
                    <Button
                        aria-owns={anchorEl ? animationSpeedId : undefined}
                        aria-haspopup="true"
                        onClick={this.openMenu}
                    >
                        {speedLabel(delta ? delta : 1)}
                    </Button>
                    <Menu
                        id={animationSpeedId}
                        anchorEl={anchorEl}
                        open={Boolean(anchorEl)}
                        onClose={this.closeMenu}
                    >
                        {speedItems}
                    </Menu>
                </div>
            </Tooltip>
        );
    }
}

function speedLabel(value: number) {
    return `x${value}`;
}


interface Props {
    delta?: number;
    disabled?: boolean;
    onDeltaChange(delta: number): void;
}

interface State {
    anchorEl: null | HTMLElement;
}

