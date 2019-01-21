import React, {PureComponent} from 'react';

import IconButton from '@material-ui/core/IconButton';
import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';
import Tooltip from '@material-ui/core/Tooltip';
import PauseIcon from '@material-ui/icons/Pause';
import PlayIcon from '@material-ui/icons/PlayArrow';
import SkipNextIcon from '@material-ui/icons/SkipNext';
import SkipPreviousIcon from '@material-ui/icons/SkipPrevious';

import {TOOLTIP_DELAY} from '../../common/constants';


const styles = (theme: Theme) => createStyles({
    controls: {
        display: 'flex',
        flexDirection: 'row',
        alignItems: 'center',
        height: 40
    }
});

const style = withStyles(styles);


export class Controls extends PureComponent<Props> {
    togglePlay = () => {
        const {onPlayToggle} = this.props;
        if (onPlayToggle) {
            onPlayToggle();
        }
    }

    render() {
        const {
            classes,
            play,
            onPrev,
            onNext,
            disablePrev,
            disableNext,
            disabled
        } = this.props;

        const icon = play ? (<PauseIcon />) : (<PlayIcon />);
        const label = play ? 'Pause' : 'Play';
        const playPauseBtn = (
            <IconButton
                onClick={this.togglePlay}
                aria-label={label}
                disabled={disabled}
            >
                {icon}
            </IconButton>
        );

        return (
            <div className={classes.controls}>
                <Tooltip title={'Previous frame'} placement="top" {...TOOLTIP_DELAY}>
                    <div>
                        <IconButton
                            onClick={onPrev}
                            disabled={disabled || disablePrev}
                            aria-label="Previous frame"
                        >
                            <SkipPreviousIcon />
                        </IconButton>
                    </div>
                </Tooltip>
                <Tooltip title={'Play'} placement="top" {...TOOLTIP_DELAY}>
                    <div>{playPauseBtn}</div>
                </Tooltip>
                <Tooltip title={'Next frame'} placement="top" {...TOOLTIP_DELAY}>
                    <div>
                        <IconButton
                            onClick={onNext}
                            disabled={disabled || disableNext}
                            aria-label="Next frame"
                        >
                            <SkipNextIcon />
                        </IconButton>
                    </div>
                </Tooltip>
            </div>
        );
    }
}

export default style(Controls);


interface Props extends WithStyles<typeof styles> {
    play: boolean;
    disablePrev?: boolean;
    disableNext?: boolean;
    disabled?: boolean;
    onPrev(): void;
    onPlayToggle(): void;
    onNext(): void;
}
