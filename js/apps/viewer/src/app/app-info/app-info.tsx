import React, {PureComponent} from 'react';

import {GET_VERSION, Version} from 'brayns';
import {from, Subscription} from 'rxjs';
import {mergeMap} from 'rxjs/operators';

import Button from '@material-ui/core/Button';
import Dialog from '@material-ui/core/Dialog';
import DialogActions from '@material-ui/core/DialogActions';
import DialogTitle from '@material-ui/core/DialogTitle';
import List from '@material-ui/core/List';
import ListItem from '@material-ui/core/ListItem';
import ListItemText from '@material-ui/core/ListItemText';
import ListSubheader from '@material-ui/core/ListSubheader';
import {
    createStyles,
    Theme,
    withStyles,
    WithStyles
} from '@material-ui/core/styles';

import brayns, {onReady} from '../../common/client';

import {
    BRAYNS_GITHUB_REPO,
    BRAYNS_GITHUB_URL
} from './constants';
import {
    getTags,
} from './github';


const styles = (theme: Theme) => createStyles({
    paper: {
        [theme.breakpoints.up('xs')]: {
            minWidth: 400
        }
    }
});

const style = withStyles(styles);


export class AppInfo extends PureComponent<Props, State> {
    state: State = {
        authors: []
    };

    private subs: Subscription[] = [];

    closeDialog = () => {
        this.props.onClose();
    }

    componentDidMount() {
        this.subs.push(...[
            onReady().pipe(
                mergeMap(() => brayns.request(GET_VERSION)),
                mergeMap(toGitVersion))
                .subscribe(version => {
                    this.setState({version});
                }),
            from(getAuthors())
                .subscribe(authors => {
                    this.setState({authors});
                })
        ]);
    }

    componentWillUnmount() {
        while (this.subs.length > 0) {
            const sub = this.subs.pop()!;
            sub.unsubscribe();
        }
    }

    render() {
        const {classes, open} = this.props;
        const {authors, version} = this.state;

        const authorsHeader = (
            <ListSubheader component="div">
                Authors
            </ListSubheader>
        );
        const authorItems = authors.map(author => (
            <ListItem
                key={author.url}
                button
                component="a"
                href={author.url}
                target="_blank"
                rel="noopener"
                aria-label="Navigate to author github page"
            >
                <ListItemText primary={author.name} />
            </ListItem>
        ));

        return (
            <Dialog
                open={!!open}
                onClose={this.closeDialog}
                aria-labelledby="app-info-title"
                classes={{paper: classes.paper}}
            >
                <DialogTitle id="app-info-title">App info</DialogTitle>
                <List>
                    <ListItem
                        component="a"
                        href={version ? version.commit : ''}
                        target="_blank"
                        rel="noopener"
                        aria-label="Navigate to commit"
                        button
                    />
                </List>
                <List component="nav" subheader={authorsHeader}>
                    {authorItems}
                </List>
                <DialogActions>
                    <Button onClick={this.closeDialog}>
                        Close
                    </Button>
                </DialogActions>
            </Dialog>
        );
    }
}

export default style(AppInfo);


async function getAuthors(): Promise<Author[]> {

    const data = [
        {name: "Cyrille Favreau", url: "https://github.com/favreau", contributions: 8}, 
        {name: "Daniel Nachbaur", url: "https://github.com/tribal-tec", contributions: 7}, 
        {name: "Jonas Karlsson", url: "https://github.com/karjonas", contributions: 6}, 
        {name: "Roland Groza", url: "https://github.com/rolandjitsu", contributions: 5}, 
        {name: "Grigori Chevtchenko", url: "https://github.com/chevtche", contributions: 4}, 
        {name: "Juan Hernando Vieites", url: "https://github.com/BlueBrain", contributions: 3}, 
        {name: "Raphael Dumusc", url: "https://github.com/BlueBrain", contributions: 2}, 
        {name: "Pawel Podhajski", url: "https://github.com/ppodhajski", contributions: 1}, 
    ];

    return sortByContributions(data);
}

function sortByContributions(authors: Author[]) {
    return authors.sort((a, b) => b.contributions - a.contributions);
}

async function toGitVersion(version: Version): Promise<GitVersion> {
    const name = toPrettyVersion(version);
    const tags = await getTags(BRAYNS_GITHUB_REPO);
    const match = tags.find(tag => tag.name.indexOf(name) !== -1);
    const commit = gitCommitUrl(version.revision);
    const tag = match ? gitTagUrl(name) : undefined;
    return {
        name,
        commit,
        tag,
        sha: version.revision
    };
}

function gitTagUrl(version: string) {
    return `${BRAYNS_GITHUB_URL}/tags/${version}`;
}

function gitCommitUrl(sha: string) {
    return `${BRAYNS_GITHUB_URL}/commit/${sha}`;
}

function toPrettyVersion(version: Version) {
    return `v${version.major}.${version.minor}.${version.patch}`;
}


interface Props extends WithStyles<typeof styles> {
    open?: boolean;
    onClose(): void;
}

interface State {
    authors: Author[];
    version?: GitVersion;
}

interface GitVersion {
    name: string;
    commit: string;
    sha: string;
    tag?: string;
}

interface Author {
    name: string;
    url: string;
    contributions: number;
}
