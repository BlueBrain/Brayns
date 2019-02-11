import React, {
    ChangeEvent,
    PureComponent
} from 'react';

import {Renderer as RendererType} from 'brayns';

import IconButton from '@material-ui/core/IconButton';
import InputAdornment from '@material-ui/core/InputAdornment';
import TextField from '@material-ui/core/TextField';
import CloseIcon from '@material-ui/icons/Close';

import {withRenderer, WithRenderer} from '../../common/client';
import {
    ColorField,
    NumericField,
    SchemaFields,
    SelectField,
    SwitchField
} from '../../common/components';


const MAX_ACC_FRAMES = 1000;


class Renderer extends PureComponent<Props> {
    changeRendererType = (evt: ChangeEvent<HTMLSelectElement>) => this.props.onRendererTypeChange!(evt.target.value);

    updateSamplesPerPixel = (samplesPerPixel: number) => this.props.onRendererPropsChange!({samplesPerPixel});

    updateSubsampling = (subsampling: number) => this.props.onRendererPropsChange!({subsampling});

    updateHeadLight = (evt: ChangeEvent<HTMLInputElement>, checked: boolean) => this.props.onRendererPropsChange!({
        headLight: checked
    })

    updateMaxAccumFrames = (value: number) => {
        const num = Math.round(value);
        this.props.onRendererPropsChange!({
            maxAccumFrames: num < MAX_ACC_FRAMES
                ? num
                : MAX_ACC_FRAMES
        });
    }

    updateBackgroundColor = (color: number[]) => {
        const rgb = fromRgb(color);
        this.props.onRendererPropsChange!({
            backgroundColor: rgb
        });
    }

    updateEnvironmentMap = (evt: ChangeEvent<HTMLInputElement>) => {
        this.props.onEnvMapChange!(evt.target.value);
    }

    clearEnvMap = () => {
        this.props.onEnvMapChange!('');
    }

    render() {
        const {
            disabled,
            envMap,
            isEnvMapValid,
            onRendererParamsChange: onSetRendererParams,
            params,
            renderer,
            schema
        } = this.props;
        const {
            backgroundColor,
            current = '',
            headLight = false,
            maxAccumFrames,
            samplesPerPixel,
            subsampling,
            types = []
        }: Partial<RendererType> = renderer || {};

        const rgbBg = toRgb(backgroundColor);

        const envMapFile = envMap ? envMap.filename : '';

        const fields = schema ? (
            <SchemaFields
                schema={schema}
                values={params!}
                onChange={onSetRendererParams!}
                disabled={disabled}
            />
        ) : null;

        const hasEnvMap = envMapFile && envMapFile.length;
        const envMapInputProps = {};
        if (hasEnvMap) {
            Object.assign(envMapInputProps, {
                endAdornment: (
                    <InputAdornment position="end">
                        <IconButton onClick={this.clearEnvMap} disabled={disabled}>
                            <CloseIcon fontSize="small" />
                        </IconButton>
                    </InputAdornment>
                )
            });
        }

        const disableBackgroundColor = disabled || (hasEnvMap && isEnvMapValid);

        return (
            <div>
                <SelectField
                    id="rendering-mode"
                    label="Rendering mode"
                    options={types}
                    value={current}
                    onChange={this.changeRendererType}
                    disabled={disabled}
                    margin="normal"
                    fullWidth
                />

                <NumericField
                    label="Samples/px"
                    value={samplesPerPixel}
                    onChange={this.updateSamplesPerPixel}
                    type="integer"
                    step={1}
                    min={1}
                    max={128}
                    disabled={disabled}
                    margin="normal"
                    fullWidth
                />

                <NumericField
                    label="Subsampling"
                    value={subsampling}
                    onChange={this.updateSubsampling}
                    type="integer"
                    step={1}
                    min={1}
                    max={16}
                    disabled={disabled}
                    margin="normal"
                    fullWidth
                />

                <NumericField
                    label="Accumulation frames"
                    value={maxAccumFrames}
                    onChange={this.updateMaxAccumFrames}
                    min={0}
                    max={MAX_ACC_FRAMES}
                    disabled={disabled}
                    margin="normal"
                    fullWidth
                />

                <ColorField
                    id="colorpicker"
                    label="Background"
                    ariaLabel="Open color picker"
                    value={rgbBg}
                    onChange={this.updateBackgroundColor}
                    margin="normal"
                    disabled={!!disableBackgroundColor}
                    fullWidth
                />

                <TextField
                    id="environmentmap"
                    label="Environment map"
                    value={envMapFile}
                    onChange={this.updateEnvironmentMap}
                    margin="normal"
                    disabled={disabled}
                    fullWidth
                    InputProps={envMapInputProps}
                    error={!disabled && !isEnvMapValid}
                    helperText={disabled || isEnvMapValid ? '' : 'Invalid path'}
                />

                <SwitchField
                    id="head-light"
                    label="Head light"
                    value={headLight}
                    onChange={this.updateHeadLight}
                    disabled={disabled}
                    fullWidth
                />

                {fields}
            </div>
        );
    }
}

export default withRenderer(Renderer);


// TODO: Add tests?
function toRgb(rgb?: number[]): number[] {
    if (Array.isArray(rgb)) {
        const color = rgb.map(v => Math.floor(v * 255));
        return color;
    }
    return [0, 0, 0];
}

// TODO: Add tests?
function fromRgb(rgb: number[]): number[] {
    return rgb.map(v => v / 255);
}


interface Props extends WithRenderer {
    disabled?: boolean;
}
