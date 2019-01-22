// tslint:disable: member-ordering
import React, {PureComponent} from 'react';

import {ApplicationParameters} from 'brayns';

import {NumericField} from '../../common/components';


export default class ImageQuality extends PureComponent<Props> {
    updateProp = (key: keyof CommonProps) => (value: number) => {
        this.saveChanges(key, value);
    }

    updateJpegCompression = this.updateProp('jpegCompression');
    updateImageStreamFps = this.updateProp('imageStreamFps');

    saveChanges = (key: string, value?: any) => {
        const {onChange} = this.props;
        if (onChange) {
            onChange({
                [key]: value
            });
        }
    }

    render() {
        const {
            imageStreamFps,
            jpegCompression,
            disabled
        } = this.props;

        return (
            <div>
                <NumericField
                    label="Image quality"
                    value={jpegCompression}
                    onChange={this.updateJpegCompression}
                    min={1}
                    max={100}
                    disabled={disabled}
                    margin="normal"
                    fullWidth
                />

                <NumericField
                    label="Image stream fps"
                    value={imageStreamFps}
                    onChange={this.updateImageStreamFps}
                    min={1}
                    max={60}
                    disabled={disabled}
                    margin="normal"
                    fullWidth
                />
            </div>
        );
    }
}


type CommonProps = Pick<ApplicationParameters, 'jpegCompression' | 'imageStreamFps'>;

interface Props extends Partial<CommonProps> {
    disabled?: boolean;
    onChange?(value: Partial<CommonProps>): void;
}
