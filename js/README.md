# Brayns JS

> A collection of web apps and JavaScript packages for the C++ rendering service [Brayns](https://github.com/BlueBrain/Brayns).


# Table of Contents

* [Prerequisites](#prerequisites)
* [Setup](#setup)
* [Development](#development)
* [Contribute](#contribute)
* [Release](#release)
* [Learning Material](#learning-material)


### Prerequisites
-----------------
In order to run or develop this project you need the following tools installed:

- [Node](http://nodejs.org) `>= 8.11`
- [Yarn](https://yarnpkg.com/en/docs/install) `>= 1.12`


### Setup
---------
Set up the app:

1. Clone this repository;
2. From the root of workspace (the `js` folder), install dependencies with `yarn install`;


### Development
---------------
The following commands are available at the root of the workspace:
* `yarn start` - builds the Brayns JS SDK and starts the UI dev server;
* `yarn build` - builds apps/packages;
* `yarn test` - builds the Brayns JS SDK and runs tests across apps/packages;
* `yarn lint` - lints apps/packages;
* `yarn clean:deps` - removes all `node_modules` folders from every app/package and root of the workspace.

For more specific commands, navigate to `apps` or `packages` and see the instructions for each app/package.


### Contribute
--------------
If you wish to contribute, please use the following guidelines:
* Use [Conventional Commits](https://www.conventionalcommits.org/) for commit messages;
* Use `[ci skip]` in commit messages to skip a build (e.g. when making docs changes).


### Release
-----------
If you'd like to make a release, use:
```shell
yarn release patch|minor|major
```

The above command will bump package/app versions using [lerna version](https://github.com/lerna/lerna/tree/master/commands/version).
Don't forget to commit or amend these changes before pushing.

**NOTE** If there is a change in package versions, the CI will publish these on NPM.


### Learning Material
---------------------
* [Workspaces](https://yarnpkg.com/lang/en/docs/workspaces)
* [CRA](https://github.com/wmonk/create-react-app-typescript)
* [TypeScript](https://www.typescriptlang.org)
* [React](https://reactjs.org)
* [Material UI](https://material-ui-next.com)
* [Jest](https://facebook.github.io/jest)
