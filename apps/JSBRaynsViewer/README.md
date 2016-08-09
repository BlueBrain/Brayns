# Brayns Visualisation UI

## Development

Once you downloaded the code, you are ready to setup your environment using
npm and bower. First, ensure node is installed on your system. You can then
install bower as a global dependency using `npm install -g bower`:

```shell
npm install -g bower
```

And of course, the NPM dependencies used to run the development server and
build the static assets using `npm install`.

```shell
npm install
```

The client-side dependencies are installed using `bower install`

```shell
bower install
```

The development server can be started using `gulp serve`

```shell
gulp serve
```

It's time to use Gulp tasks:
- `$ gulp` to build an optimized version of your application in folder dist
- `$ gulp serve` to start BrowserSync server on your source files with live reload
- `$ gulp serve:dist` to start BrowserSync server on your optimized application without live reload
- `$ gulp test` to run your unit tests with Karma
- `$ gulp test:auto` to run your unit tests with Karma in watch mode
- `$ gulp protractor` to launch your e2e tests with Protractor
- `$ gulp protractor:dist` to launch your e2e tests with Protractor on the dist files
