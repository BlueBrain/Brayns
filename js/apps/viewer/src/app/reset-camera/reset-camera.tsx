import React, {PureComponent} from 'react';

import {GET_CAMERA, GET_SCENE} from 'brayns';

import brayns from '../../common/client';

import IconButton from '@material-ui/core/IconButton';
import Tooltip from '@material-ui/core/Tooltip';
import FilterCenterFocusIcon from '@material-ui/icons/FilterCenterFocus';

import {KeyCode, TOOLTIP_DELAY} from '../../common/constants';
import {dispatchCamera, dispatchCameraSettings} from '../../common/events';
import {rotateToBoundingBox} from '../../common/math';
import {isCmdKey} from '../../common/utils';


export default class ResetCamera extends PureComponent<Props> {
    resetCamera = async () => {
        const camera = await brayns.request(GET_CAMERA);
        const scene = await brayns.request(GET_SCENE);
        const res = rotateToBoundingBox(camera, scene.bounds);
        dispatchCamera(res);
        dispatchCameraSettings({
            sensitivity: 1
        });
    }

    resetCameraOnKeydown = (evt: KeyboardEvent) => {
        const {disabled} = this.props;
        if (!disabled && evt.keyCode === KeyCode.R && evt.shiftKey && !isCmdKey(evt)) {
            evt.preventDefault();
            this.resetCamera();
        }
    }

    componentDidMount() {
        window.addEventListener('keydown', this.resetCameraOnKeydown, false);
    }
    componentWillUnmount() {
        window.removeEventListener('keydown', this.resetCameraOnKeydown);
    }

    render() {
        const {disabled} = this.props;
        return (
            <div>
                <Tooltip title={'Reset camera'} placement="bottom" {...TOOLTIP_DELAY}>
                    <div>
                        <IconButton
                            onClick={this.resetCamera}
                            aria-label="Reset camera"
                            disabled={disabled}
                        >
                            <FilterCenterFocusIcon />
                        </IconButton>
                    </div>
                </Tooltip>
            </div>
        );
    }
}

interface Props {
    disabled?: boolean;
}
