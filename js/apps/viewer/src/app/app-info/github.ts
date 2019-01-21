// tslint:disable: no-empty

/**
 * Get list of tags for a repo
 * @see https://developer.github.com/v3/repos/#list-tags
 */
export async function getTags(repo: string) {
    try {
        const uri = apiUrl(`repos/${repo}/tags`);
        const response = await fetch(uri);
        if (response.ok) {
            const json = await response.json();
            const tags: Tag[] = Array.isArray(json) ? json : [];
            return tags;
        }
    } catch {}
    return [];
}

/**
 * Get list of contributors
 * @see https://developer.github.com/v3/repos/statistics/#get-contributors-list-with-additions-deletions-and-commit-counts
 */
export async function getContributors(repo: string) {
    try {
        const uri = apiUrl(`repos/${repo}/contributors`);
        const response = await fetch(uri);
        if (response.ok) {
            const json = await response.json();
            const contributors: Contributor[] = Array.isArray(json) ? json : [];
            return contributors;
        }
    } catch {}
    return [];
}

/**
 * Get user info
 * @see https://developer.github.com/v3/users/#get-a-single-user
 */
export async function getUser(username: string) {
    try {
        const uri = apiUrl(`users/${username}`);
        const response = await fetch(uri);
        if (response.ok) {
            const json = await response.json();
            return json as UserProfile;
        }
    } catch {}
    return;
}


function apiUrl(api: string) {
    return `https://api.github.com/${api}`;
}


export interface Tag {
    name: string;
    zipball_url: string;
    tarball_url: string;
    commit: Commit;
    node_id: string;
}

export interface Commit {
    sha: string;
    url: string;
}

export interface Contributor extends User {
    contributions: number;
}

export interface UserProfile extends User {
    name: string;
    company: string;
    blog: string;
    location: string;
    email: string;
    hireable: boolean;
    bio: string;
    public_repos: number;
    public_gists: number;
    followers: number;
    following: number;
    created_at: string;
    updated_at: string;
}

export interface User {
    login: string;
    id: number;
    node_id: string;
    avatar_url: string;
    gravatar_id: string;
    url: string;
    html_url: string;
    followers_url: string;
    following_url: string;
    gists_url: string;
    starred_url: string;
    subscriptions_url: string;
    organizations_url: string;
    repos_url: string;
    events_url: string;
    received_events_url: string;
    type: string;
    site_admin: boolean;
}
