# Brayns Viewer

> A web interface for interacting with the C++ rendering service [Brayns](https://github.com/BlueBrain/Brayns) built with [TypeScript](http://www.typescriptlang.org), [React](https://reactjs.org) and [Material UI](https://material-ui-next.com/).


# Table of Contents

* [Run](#run)
* [Test](#test)
* [Analyze](#analyze)
* [Build](#build)
* [Clean](#clean)


### Run
-------
Run `yarn start` to start a dev web server and run the app in the browser.
This should open the app in your default browser.

This process will watch files and re-compile on changes.


### Test
--------
There are two ways tests can be ran:

* `yarn test`: runs unit tests on files that have changed (uses git to detect changed files) or have been affected by the changed files;
* `yarn test:cov`: runs unit tests with coverage on all files;
* `yarn lint`: runs tslint on all files.


### Analyze
-----------
Show stats about the dependency tree byte size with `yarn analyze`.


### Build
---------
Make a production build with `yarn build`.


### Clean
---------
Remove build artifacts with `yarn build`.
