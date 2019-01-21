const camelCase = require('camelcase');
const babel = require('rollup-plugin-babel');
const commonjs = require('rollup-plugin-commonjs');
const resolve = require('rollup-plugin-node-resolve');
const sourcemaps = require('rollup-plugin-sourcemaps');
const {uglify} = require('rollup-plugin-uglify');

const pckg = require('./package.json');
const name = pckg.name;
const input = pckg.module;

const plugins = [
    resolve(),
    commonjs(),
    // Transform Lodash imports from:
    // import {isString} from 'lodash';
    // to:
    // import isString from 'lodash';
    // https://github.com/rollup/rollup/issues/691#issue-158687701
    babel({
        babelrc: false,
        presets: [['@babel/preset-env', {
            // Disable modules transform
            // https://github.com/rollup/rollup-plugin-babel#modules
            modules: false
        }]],
        plugins: [
            "lodash"
        ]
    }),
    sourcemaps()
];

const output = {
    format: 'umd',
    name: camelCase(name),
    // The key is library name and the value is the name of the global variable name on the window object.
    // See https://github.com/rollup/rollup/wiki/JavaScript-API#globals for more.
    globals: {
        // RxJS
        // https://github.com/ReactiveX/rxjs#cdn
        'rxjs': 'rxjs',
        'rxjs/operators': 'rxjs.operators',
        'rxjs/webSocket': 'rxjs.webSocket',
        // Rockets
        'rockets-client': 'rocketsClient'
    },
    sourcemap: true
};

// List of dependencies
// See https://github.com/rollup/rollup/wiki/JavaScript-API#external for more.
const external = [
    // RxJS
    'rxjs',
    'rxjs/operators',
    'rxjs/webSocket',
    'rockets-client'
];

export default [{
    input,
    plugins,
    external,
    output: {
        ...output,
        file: distPath(`${name}.umd.js`)
    }
},
{
    input,
    plugins: [
        ...plugins,
        uglify()
    ],
    external,
    output: {
        ...output,
        file: distPath(`${name}.umd.min.js`)
    }
}];

function distPath(file) {
    return `./dist/bundles/${file}`;
}
