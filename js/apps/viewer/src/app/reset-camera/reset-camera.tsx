import React, {PureComponent} from 'react';

import {GET_SCENE} from 'brayns';

import IconButton from '@material-ui/core/IconButton';
import Tooltip from '@material-ui/core/Tooltip';
import RefreshIcon from '@material-ui/icons/Refresh';

import brayns, {
    withCamera,
    WithCamera,
    withConnectionStatus,
    WithConnectionStatus
} from '../../common/client';
import {KeyCode, TOOLTIP_DELAY} from '../../common/constants';
import {isCmdKey} from '../../common/utils';

class ResetCamera extends PureComponent<Props> {
    resetCamera = async () => {
        const scene = await brayns.request(GET_SCENE);
        await this.props.onReset!(scene.bounds);
    }

    resetCameraOnKeydown = async (evt: KeyboardEvent) => {
        if (this.props.online && evt.keyCode === KeyCode.R && evt.shiftKey && !isCmdKey(evt)) {
            evt.preventDefault();
            await this.resetCamera();
        }
    }

    componentDidMount() {
        window.addEventListener('keydown', this.resetCameraOnKeydown, false);
    }
    componentWillUnmount() {
        window.removeEventListener('keydown', this.resetCameraOnKeydown);
    }

    render() {
        return (
            <div>
                <Tooltip title={'Reset camera'} placement="bottom" {...TOOLTIP_DELAY}>
                    <div>
                        <IconButton
                            onClick={this.resetCamera}
                            aria-label="Reset camera"
                            disabled={!this.props.online}
                        >
                            <RefreshIcon />
                        </IconButton>
                    </div>
                </Tooltip>
            </div>
        );
    }
}

export default withConnectionStatus(
    withCamera(ResetCamera));

type Props = WithConnectionStatus
    & WithCamera;
