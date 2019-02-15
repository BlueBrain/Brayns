import React, {PureComponent} from 'react';

import {kebabCase, startCase} from 'lodash';
import {Autocomplete, AutocompleteInputProps} from '../autocomplete';
import {findColormapsByName, getColormapsList} from './api';

export default class ColormapAutocomplete extends PureComponent<ColormapAutocompleteProps, State> {
    state: State = {
        suggestions: []
    };

    private colormapList = getColormapsList();
    private controller?: AbortController;

    search = async (inputValue: string) => {
        if (this.controller) {
            this.controller.abort();
        }

        this.controller = new AbortController();

        if (inputValue.length) {
            try {
                const suggestions = await findColormapsByName(inputValue, this.controller.signal);
                this.setState({
                    suggestions: suggestions.map(startCase)
                });
            } catch {} // tslint:disable-line: no-empty
        } else {
            const suggestions = await this.colormapList;
            this.setState({
                suggestions: suggestions.map(startCase)
            });
        }
    }

    onChange = (suggestion: string) => {
        const {onChange} = this.props;
        if (onChange) {
            onChange(kebabCase(suggestion));
        }
    }

    render() {
        const {value, ...inputProps} = this.props;
        const {suggestions} = this.state;

        return (
            <Autocomplete
                {...inputProps}
                suggestions={suggestions}
                label="Colormap"
                helperText="Type a colormap name to see available options"
                onInputValueChange={this.search}
                value={startCase(value)}
                onChange={this.onChange}
            />
        );
    }
}

export interface ColormapAutocompleteProps extends AutocompleteInputProps {
    value?: string;
    onChange?(value: string): void;
}

interface State {
    suggestions: string[];
}
