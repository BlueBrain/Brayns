import React, {PureComponent} from 'react';

import {GET_VERSION, Version} from 'brayns';
import {isObject} from 'lodash';
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
import storage from '../../common/storage';

import {
    BRAYNS_GITHUB_REPO,
    BRAYNS_GITHUB_URL
} from './constants';
import {
    Contributor,
    getContributors,
    getTags,
    getUser
} from './github';


const AUTHORS_KEY = 'authors';
const EXCLUDED_CONTRIBUTORS = [
    'eile',
    'adevress'
];


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

        const versionName = version
            ? version.tag
                ? version.name : version.sha
            : (<span>&mdash;</span>);

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
                    >
                        <ListItemText primary="Version" secondary={versionName} />
                    </ListItem>
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
    const today = Date.now();
    const cache = storage.get<AuthorsList>(AUTHORS_KEY);
    if (cache) {
        const diff = today - cache.updatedOn;
        if (diff <= (60 * 60 * 1000)) {
            return sortByContributions(cache.authors);
        }
    }

    const contributors = await getContributors(BRAYNS_GITHUB_REPO);
    const authors = await Promise.all(contributors.filter(
            contributor => contributor.contributions > 2 && !EXCLUDED_CONTRIBUTORS.includes(contributor.login))
        .map(toAuthor));
    const data = authors.filter(isObject) as Author[];

    // Cache results
    // https://developer.github.com/v3/#rate-limiting
    storage.set(AUTHORS_KEY, {
        authors: data,
        updatedOn: today
    });

    return sortByContributions(data);
}

function sortByContributions(authors: Author[]) {
    return authors.sort((a, b) => b.contributions - a.contributions);
}

async function toAuthor(contributor: Contributor) {
    const user = await getUser(contributor.login);
    if (user) {
        const {name} = user;
        return {
            name: name && name.length > 0 ? name : user.login,
            url: user.html_url,
            contributions: contributor.contributions
        };
    }
    return;
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

interface AuthorsList {
    authors: Author[];
    updatedOn: number;
}
