import React, {memo, Ref} from 'react';

import Downshift, {ChildrenFunction} from 'downshift';

import {InputBaseProps} from '@material-ui/core/InputBase';
import MenuItem from '@material-ui/core/MenuItem';
import Paper from '@material-ui/core/Paper';
import Popper from '@material-ui/core/Popper';
import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';
import TextField, {TextFieldProps} from '@material-ui/core/TextField';


const styles = (theme: Theme) => createStyles({
    root: {
        flexGrow: 1
    },
    container: {
        flexGrow: 1,
        position: 'relative'
    },
    popper: {
        zIndex: theme.zIndex.drawer + 1
    },
    paper: {
        marginTop: theme.spacing.unit,
        maxHeight: 230,
        overflowY: 'scroll'
    },
    inputRoot: {
        flexWrap: 'wrap'
    },
    inputInput: {
        width: 'auto',
        flexGrow: 1
    }
});

const style = withStyles(styles);


let popperNode: HTMLElement | null;

function Autocomplete(props: AutocompleteProps) {
    const {
        classes,
        label,
        onInputValueChange,
        onChange,
        suggestions,
        value,
        helperText,
        disabled,
        fullWidth,
        margin
    } = props;

    const children = createDownshiftChildren({
        classes,
        label,
        suggestions,
        helperText,
        disabled,
        fullWidth,
        margin
    });

    return (
        <div className={classes.root}>
            <Downshift
                onInputValueChange={onInputValueChange}
                onChange={onChange}
                selectedItem={value}
            >
                {children}
            </Downshift>
        </div>
    );
}

export default style(memo(Autocomplete));

export interface AutocompleteProps extends WithStyles<typeof styles>, AutocompleteInputProps {
    suggestions: string[];
    value?: string;
    onInputValueChange?(value: string): void;
    onChange?(item: string): void;
}


function createDownshiftChildren({classes, suggestions, ...inputProps}: DownshiftRenderArgs): ChildrenFunction<string> {
    return ({
        getInputProps,
        getItemProps,
        getMenuProps,
        highlightedIndex,
        isOpen,
        selectedItem
    }) => {
        const input = renderInput({
            ...inputProps,
            classes,
            InputProps: getInputProps()
        });
        const menuProps = getMenuProps({}, {suppressRefError: true});
        const paperStyle = popperNode ? {width: popperNode.clientWidth} : {};
        const items = suggestions.map((suggestion, index) => renderSuggestion({
            suggestion,
            index,
            itemProps: getItemProps({item: suggestion}),
            highlightedIndex,
            selectedItem
        }));

        const paper = isOpen ? (
            <Paper
                className={classes.paper}
                style={{...paperStyle}}
                square
            >
                {items}
            </Paper>
        ) : null;

        return (
            <div className={classes.container}>
                {input}
                <Popper open={isOpen} anchorEl={popperNode} className={classes.popper}>
                    <div {...(isOpen ? menuProps : {})}>
                        {paper}
                    </div>
                </Popper>
            </div>
        );
    };
}

function renderSuggestion({suggestion, index, itemProps, highlightedIndex, selectedItem}: SuggestionRenderArgs) {
    const isHighlighted = highlightedIndex === index;
    const isSelected = selectedItem === suggestion;
    const style = {
        fontWeight: isSelected ? 500 : 400
    };

    return (
        <MenuItem
            {...itemProps}
            key={suggestion}
            selected={isHighlighted}
            component="div"
            style={style}
        >
            {suggestion}
        </MenuItem>
    );
}

function renderInput(inputProps: InputRenderArgs) {
    const {InputProps, classes, ref, ...other} = inputProps;
    const props = {
        inputRef: (node: HTMLElement | null) => {
            popperNode = node;
        },
        classes: {
            root: classes.inputRoot,
            input: classes.inputInput
        },
        ...InputProps
    };

    return (
        <TextField
            InputProps={props}
            {...other}
        />
    );
}


export type AutocompleteInputProps = Pick<InputRenderArgs, 'label' | 'helperText' | 'disabled' | 'margin' | 'fullWidth'>;

interface DownshiftRenderArgs extends AutocompleteInputProps {
    classes: Record<'container' | 'paper' | 'popper' | 'inputRoot' | 'inputInput', string>;
    suggestions: string[];
}

interface SuggestionRenderArgs {
    suggestion: string;
    index: number;
    itemProps: any;
    highlightedIndex: number | null;
    selectedItem: string | null;
}

interface InputRenderArgs {
    classes: Record<'inputRoot' | 'inputInput', string>;
    InputProps: InputBaseProps;
    label?: TextFieldProps['label'];
    helperText?: TextFieldProps['helperText'];
    disabled?: TextFieldProps['disabled'];
    fullWidth?: TextFieldProps['fullWidth'];
    margin?: TextFieldProps['margin'];
    ref?: Ref<any>;
}
