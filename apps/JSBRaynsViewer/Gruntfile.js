// Generated on 2015-03-12 using generator-hbp-angular 0.0.2
'use strict';
// # Globbing
// for performance reasons we're only matching one level down:
// 'test/spec/{,*/}*.js'
// use this if you want to recursively match all subfolders:
// 'test/spec/**/*.js'
module.exports = function (grunt) {
    /*
    var braynsHost = '128.178.97.241';
    var braynsPort = 5000;
    */
    
    grunt.loadNpmTasks('grunt-htmlhint');
    grunt.loadNpmTasks('grunt-contrib-sass');
    //grunt.loadNpmTasks('grunt-connect-proxy');
    
    var CI = process.env.CI || false;
    var jsHintOptions = function (prefix, options) {
        if (CI) {
            options.reporter = require('jshint-junit-reporter');
            options.reporterOutput = (prefix ? prefix + '-' : '') + 'jshint-unit.xml';
        } else {
            options.reporter = require('jshint-stylish');
        }
        return options;
    };

    // Load grunt tasks automatically
    require('load-grunt-tasks')(grunt);

    // Time how long tasks take. Can help when optimizing build times
    require('time-grunt')(grunt);

    // Configurable paths for the application
    var appConfig = {
        app: require('./bower.json').appPath || 'app',
        dist: 'dist'
    };

    var corsMiddleware = function (req, res, next) {
        //console.log('Adding cors headers to ' + req.url);
        res.setHeader('Access-Control-Allow-Origin', '*');
        res.setHeader('Access-Control-Allow-Methods', '*');
        res.setHeader('Access-Control-Allow-Headers', 'Content-Type');
        next();
    }

    // Define the configuration for all the tasks
    grunt.initConfig({
        // Project settings
        yeoman: appConfig,

        // Watches files for changes and runs tasks based on the changed files
        watch: {
            bower: {
                files: ['bower.json'],
                tasks: ['wiredep']
            },
            js: {
                files: ['<%= yeoman.app %>/scripts/**/*.js'],
                tasks: ['newer:jshint:all'],
                options: {
                    livereload: '<%= connect.options.livereload %>'
                }
            },
            gruntfile: {
                files: ['Gruntfile.js']
            },
            livereload: {
                options: {
                    livereload: '<%= connect.options.livereload %>'
                },
                files: ['<%= yeoman.app %>/{,*/}*.html', '.tmp/css/{,*/}*.css', '<%= yeoman.app %>/images/{,*/}*.{png,jpg,jpeg,gif,webp,svg}']
            }
        },

        // The actual grunt server settings
        connect: {
            options: {
                port: 8095,
                // Change this to '0.0.0.0' to access the server from outside.
                hostname: '0.0.0.0',
                livereload: 35890,
            },
            /*
            proxies: {
                context: '/zerobuf',
                host: braynsHost,
                port: braynsPort,
            },
            */
            livereload: {
                options: {
                    //open: true,
                    middleware: function (connect, options) {
                        var middlewares = [
                            //require('connect-modrewrite')(['^/zerobuf http://' + braynsHost + ':5000/zerobuf [P L]']),
                            //require("grunt-connect-proxy/lib/utils").proxyRequest,
                            connect.static('.tmp'),
                            connect().use('/bower_components', connect.static('./bower_components')),
                            connect.static(appConfig.app),
                        ];
                        //middlewares.unshift(corsMiddleware);
                        return middlewares;
                    }
                }
            },
            dist: {
                options: {
                    port: 8092,
                    open: true,
                    base: '<%= yeoman.dist %>'
                }
            }
        },

        // Make sure code styles are up to par and there are no obvious mistakes
        jshint: {
            all: {
                options: jsHintOptions('all', {
                    jshintrc: '.jshintrc'
                }),
                src: ['Gruntfile.js', '<%= yeoman.app %>/scripts/{,*/}*.js']
            },
            test: {
                options: jsHintOptions('test', {
                    jshintrc: 'test/.jshintrc'
                }),
                src: ['test/spec/{,*/}*.js']
            }
        },

        // Empties folders to start fresh
        clean: {
            dist: {
                files: [{
                    dot: true,
                    src: ['.tmp', '<%= yeoman.dist %>/{,*/}*', '!<%= yeoman.dist %>/.git*']
                }]
            },
            server: '.tmp'
        },

        // Add vendor prefixed styles
        autoprefixer: {
            options: {
                browsers: ['last 1 version']
            },
            dist: {
                files: [{
                    expand: true,
                    cwd: '.tmp/css/',
                    src: '{,*/}*.css',
                    dest: '.tmp/css/'
                }]
            }
        },
        // Renames files for browser caching purposes
        filerev: {
            dist: {
                src: ['<%= yeoman.dist %>/scripts/{,*/}*.js', '<%= yeoman.dist %>/css/{,*/}*.css', ]
            }
        },

        // Reads HTML for usemin blocks to enable smart builds that automatically
        // concat, minify and revision files. Creates configurations in memory so
        // additional tasks can operate on them
        useminPrepare: {
            html: ['<%= yeoman.app %>/index.html'],
            options: {
                dest: '<%= yeoman.dist %>',
                flow: {
                    html: {
                        steps: {
                            js: ['concat', 'uglifyjs'],
                            css: ['cssmin']
                        },
                        post: {}
                    }
                }
            }
        },

        // Performs rewrites based on filerev and the useminPrepare configuration
        usemin: {
            html: ['<%= yeoman.dist %>/{,*/}*.html'],
            css: ['<%= yeoman.dist %>/css/{,*/}*.css'],
        },
        htmlmin: {
            dist: {
                options: {
                    collapseWhitespace: true,
                    conservativeCollapse: true,
                    collapseBooleanAttributes: true,
                    removeCommentsFromCDATA: true,
                    removeOptionalTags: true
                },
                files: [{
                    expand: true,
                    cwd: '<%= yeoman.dist %>',
                    src: ['*.html'],
                    dest: '<%= yeoman.dist %>'
                }]
            }
        },

        // Replace Google CDN references
        cdnify: {
            dist: {
                html: ['<%= yeoman.dist %>/*.html']
            }
        },

        // Copies remaining files to places other tasks can use
        copy: {
            dist: {
                files: [
                    {
                        expand: true,
                        dot: true,
                        cwd: '<%= yeoman.app %>',
                        dest: '<%= yeoman.dist %>',
                        src: ['*.json', '*.html', 'views/**/*.html']
                    },
                    {
                        expand: true,
                        dot: true,
                        cwd: 'bower_components/hbp-collaboratory-theme/dist',
                        dest: '<%= yeoman.dist %>',
                        src: ['fonts/*']
                    },
                    {
                        expand: true,
                        dot: true,
                        cwd: 'bower_components/hbp-collaboratory-theme/dist/fonts',
                        dest: '<%= yeoman.dist %>/css/assets',
                        src: ['*']
                    }]
            },
            styles: {
                expand: true,
                cwd: '<%= yeoman.app %>/css',
                dest: '.tmp/css/',
                src: '{,*/}*.css'
            }
        },
        pkg: grunt.file.readJSON('package.json'),
        bump: {
            options: {
                files: ['package.json', 'bower.json'],
                updateConfigs: ['pkg'],
                commitFiles: ['package.json', 'bower.json'],
                pushTo: 'origin HEAD:master'
            }
        },
        exec: {
            npmPublish: 'npm publish',
            npmTag: 'npm tag <%= pkg.name %>@<%= pkg.version %> latest'
        },
        wiredep: {
            task: {
                // Point to the files that should be updated when
                // you run `grunt wiredep`
                src: ['app/index.html'],
                options: {
                    // See wiredep's configuration documentation for the options
                    // you may pass:
                    // https://github.com/taptapship/wiredep#configuration
                }
            }
        }
    });

    grunt.registerTask('serve', 'Compile then start a connect web server', function (target) {
        if (target === 'dist') {
            return grunt.task.run(['build', 'connect:dist:keepalive']);
        }
        grunt.task.run(['clean:server', 'connect:livereload', 'watch']);
    });

    grunt.registerTask('server', 'DEPRECATED TASK. Use the "serve" task instead', function (target) {
        grunt.log.warn('The `server` task has been deprecated. Use `grunt serve` to start a server.');
        grunt.task.run(['serve:' + target]);
    });

    grunt.registerTask('build', [
        'clean:dist', 'useminPrepare', 'concat', 'copy:dist', 'cdnify', 'cssmin', 'uglify', 'filerev', 'usemin', 'htmlmin'
    ]);

    grunt.registerTask('default', ['newer:jshint', 'build', 'htmlhint']);
    grunt.registerTask('ci', function (target) {
        grunt.config('jshint.all.options.reporter', require('jshint-junit-reporter'));
        grunt.config('jshint.all.options.reporterOutput', 'reports/jshint-all-unit.xml');
        grunt.config('jshint.test.options.reporter', require('jshint-junit-reporter'));
        grunt.config('jshint.test.options.reporterOutput', 'reports/jshint-test-unit.xml');
        var tasks = ['build'];
        if (target === 'patch' || target === 'minor' || target === 'major') {
            tasks.unshift('bump-only:' + target);
            tasks.push('bump-commit', 'exec:npmPublish', 'exec:npmTag');
        }
        grunt.task.run(tasks);
    });
};