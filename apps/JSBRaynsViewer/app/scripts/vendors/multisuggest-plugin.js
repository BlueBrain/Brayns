(function($) {
    
    "use strict";

    var _superconstr = $.fn.typeahead.Constructor, 
        _superproto  = $.fn.typeahead.Constructor.prototype;

    /*
     * MultiSuggest Plugin Constructor
     */
    var MultiSuggest = function(element, options) {

        _superconstr.call(this, element, options);
        this.init.call(this);

    };

    /*
     * MultiSuggest Plugin Prototype
     */
    MultiSuggest.prototype = $.extend(MultiSuggest, _superproto, {

        constructor : MultiSuggest,
        /* Widget initialization */
        init : function() {            
            var self = this;
            var nameAttr;
                        
            self.sources = self.options.sources;
            self.idSet = [];
            self.query = "";

            
            //replace the 'name' attribute with a temporary assignment, 
            //create hidden input to store value
            nameAttr = self.$element.attr("name");
            self.$element.attr("name", nameAttr + "_msuggest");
            self.$hiddenInput = $("<input>", {  name : nameAttr,
                                                type: 'hidden',
                                                "class" : "msuggest-hiddeninput"});
            self.$element.after(self.$hiddenInput);
            
            //display the default value, if provided
            self.setValue(self.$element.val(), self.$element.data('value'));

            self.$element.attr("autocomplete", "off");
            self.$menu.addClass("msuggest-menu");
            
            //focus/click listener to always select text when input has a value
            self.$element.on("mousedown", function(evt) {
                if (self.$element.hasClass("msuggest-selected")) {
                    //select the existing item on click/key focus
                    evt.stopPropagation();
                    evt.preventDefault();
                    self.$element.select();                
                }
                if (self.options.enableDropdownMode) {
                    //register click listener to expand dropdown
                    self.open.call(self);
                }
            });
            
            if (self.options.enableDropdownMode) {
                self.$element.addClass("msuggest-input-dropdown");
            }
            
            //provide default values for options that were not specified            
            $.each(self.sources, function(ind, dataSource) {
                self.sources[ind] = $.extend( [], $.fn.multisuggest.defaults.dataSource, dataSource);
            });

        },
        /* Selection listener to assign selected value to hidden input */
        select: function () {
            var active, display, val;
            
            active  = this.$menu.find('.active');
            display = active.attr('data-display');
            val = active.attr('data-value');
            this.setValue(display, val);
        },
        setValue : function(display, value) {
            if (display && display !== "" &&
                value && value !== "") {
                this.$element.val(this.updater(display)).change();
                this.$hiddenInput.val(value);
                this.$element.addClass("msuggest-selected");
            }
            return this.hide();
        },
        /* Entry point for search operation, delays the search if delay option is specified */
        lookup : function() {
            var self = this;
            
            if (self.options.delay) {
                
                clearTimeout(self.timeout);
                self.timeout = setTimeout(function() {
                    // only search if the value has changed
                    if ( self.query !== self.$element.val() ) {
                        self.search.call(self);
                    }
                }, self.options.delay );
            }
            else {
                //no delay!
                self.search();
            }
        },
        /* Performs the search for each registered data source, based on their type */
        search : function() {
            var self = this;
            
            self.query = self.$element.val();
            self.idSet = [];
            
            //empty query
            if (!self.query || self.query.length < self.options.minLength) {
                return self.shown ? self.hide() : self;
            }

            //iterate over data sources and perform search operation for each type
            $.each(self.sources, function(ind, dataSource) {
                                                
                //Backbone Collection type
                if ("backbone.collection" === dataSource.type) {
                    dataSource.results = dataSource.filter(dataSource.data.models, self.query);
                    self._normalize(dataSource);
                } 
                //URL type
                else if ("url" === dataSource.type) {

                    if (self.xhr) {
                        self.xhr.abort();
                    }
                    
                    //setup the url params
                    var urlData = {};
                    urlData[dataSource.queryParam] = self.query;
                    //give the datasource a unique id, so we can identify its 'loading' placeholder later
                    dataSource.sourceId = dataSource.type + "_" + ind;

                    self.xhr = $.ajax({
                        url : dataSource.data,
                        data : urlData,
                        dataType : "json",
                        success : function(data) {
                            
                            dataSource.results = dataSource.resultsProcessor(data);
                            self._urlResponse(dataSource);

                        },
                        error : function() {
                            
                            dataSource.results = [];
                            self._urlResponse(dataSource);

                        }
                    });
                    //wipe out previous request
                    dataSource.results = null;
                } 
                 //Array type
                else if ("array" === dataSource.type) {
                    dataSource.results = dataSource.filter(dataSource.data, self.query);
                    self._normalize(dataSource);
                }

            });
            
            //process and display data source results
            return self.render(false).show();

        },
        /* Dropdown mode: opens the widget as a dropdown to display synchronous dataSources only */
        open : function() {
            var self = this;
            self.idSet = [];
            self.query = "";

            $.each(self.sources, function(ind, dataSource) {

                if ("backbone.collection" === dataSource.type) {
                    dataSource.results = dataSource.data.models;
                    self._normalize(dataSource);
                }
                //ignore url sources for open flow
                else if ("url" === dataSource.type) {
                    //wipe out previous request
                    dataSource.results = null;
                }
                else {
                     dataSource.results = dataSource.data;
                     self._normalize(dataSource);
                 }
            });
            
            //render with dropdownMode to true (don't show remote data)
            return self.render(true).show();

        },
        /* Format the result data into label, value, and displayValue */
        _normalize : function(dataSource) {
            var self = this;
            var idAttr;

            //chop off tail end if we are beyond the maxEntries for this data source
            if (dataSource.maxEntries && 
                    (dataSource.maxEntries < dataSource.results.length)) {
                dataSource.results = dataSource.results.slice(0, dataSource.maxEntries);                    
                dataSource.moreEntries = true;
            } 
            else {
                dataSource.moreEntries = false;
            }

            //format the results
            dataSource.results = $.map(dataSource.results, function(item) {
                    
                    idAttr = dataSource.valueAttribute(item);
                    
                    if (self.idSet.indexOf(idAttr) < 0) {
                        self.idSet.push(idAttr);
                    
                        return {
                            label : dataSource.listFormatter(item, self.query),
                            displayValue : dataSource.inputFormatter(item, self.query),
                            value : idAttr
                         };                        
                    }
                });
            
            

            return dataSource;
        },
        /* Render the menu sections for each dataSource. 
         * @param dropdownMode - if true, will not render asynchronous dataSources 
         *                       (as in 'open' flow for dropdown mode)
         */
        render : function(dropdownMode) {
            var self = this;
            var menu, li, renderPlaceHolder, renderSource;

            menu = self.$menu.empty();

            $.each(self.sources, function(ind, dataSource) {
                
                //boolean which indicates whether or not to render a loading placehold icon for url data sources
                //for open flow, url types are ignored, only static data is displayed
                renderPlaceHolder = ('url' === dataSource.type && !dropdownMode);
                renderSource = ('url' !== dataSource.type || !dropdownMode);
                
                //render the dataSource (dropdownMode will not render url types for open flow)
                if (renderSource) {
                    //create the header text
                    li = $("<li>", {"class" : "msuggest-header"});
                    li.append($("<span>", {"html" : dataSource.header}));
                    menu.append(li);
                
                    //render the loading icon (if applicable)
                    if (renderPlaceHolder) {
                        li = $("<li>", {id : dataSource.sourceId});
                        li.append($("<a>", {"class" : "msuggest-loading"}));
                        menu.append(li);
                    }
    
                    if (dataSource.results) {
                        //render the menu items                    
                        self._renderMenu(menu, dataSource);                    
                    }
                    
                    //divider to separate the data source sections
                    menu.append($("<li>", {"class" : "divider"}));
                }

            });

            self._buildLink();

            // size and position menu
            menu.show();
            self._resizeMenu();

            return self;
        },
        /* Process the response from an asynchronous url type dataSource */
        _urlResponse : function(dataSource) {
            var self = this;
            var li, tempul;

            //insert the response from the ajax request into its spot in the menu
            li = $("#" + dataSource.sourceId, self.$menu).empty();
            tempul = $("<ul>");
            self._normalize(dataSource);
            
            self._renderMenu(tempul, dataSource);
            li.replaceWith(tempul.children());
            self._resizeMenu();

        },
        /* Helper method to render the menu section for a particular data source's results set */
        _renderMenu : function(ul, dataSource) {
            var self = this,
                items = dataSource.results,
                moreEntries = dataSource.moreEntries;
            

            items = $(items).map(function(i, item) {
                
                i = $(self.options.item).attr('data-value', item.value)
                                        .attr('data-display', item.displayValue)
                                        .addClass("menuitem");
                i.find('a').html(item.label);
                return i[0];

            });
            
            //no items found.. display the no results message instead
            if (!items.length) {
                var li = $("<li>", {"class" : "disabled"})
                                .append($("<a>", {"class" : "muted",
                                                   html : self.options.noResultsText}));
                ul.append(li);
            }
            else {
                ul.append(items);                
            }
            
            //if we found more items than are displayed, show a '...'
            if (moreEntries) {
                ul.append($("<li>", {"class" : "disabled msuggest-ellip",
                                      html : "<span>...</span>"}));
            }
            
            return ul;
        },
        /* Overridden next method to ignore headings on keyboard navigation*/
        next : function(event) {

            var active = this.$menu.find('.active').removeClass('active'),
                next = active.next();

            //select the first item in the menu if at the end of the list
            if (!next.length) {
                next = $(this.$menu.find('li.menuitem')[0]);
            }
            
            //otherwise, look for the next menuitem after the current 'active' one
            while (next.length && !next.hasClass('menuitem')) {
                next = next.next();
            }

            next.addClass('active');
        },
        /* Overridden prev method to ignore headings on keyboard navigation*/
        prev : function(event) {
            var active = this.$menu.find('.active').removeClass('active'), 
                prev = active.prev();

            //select the last item in the menu if at the beginning of the list
            if (!prev.length) {
                prev = this.$menu.find('li.menuitem').last();
            }
            
            //otherwise, look for the previous menuitem before the current 'active' one
            while (prev.length && !prev.hasClass('menuitem')) {
                prev = prev.prev();
            }

            prev.addClass('active');
        },
        blur : function(e) {
            this.focused = false;
            if (!this.mousedover && this.shown) {
                this.hide();
            }
            if (!this.$element.hasClass("msuggest-selected")) {
                this.$element.val("");
            }
        },
        /* Match menu width to input element */
        _resizeMenu : function() {
            var ul = this.$menu;
            ul.css("min-width", this.$element.outerWidth());
        },
        /* Overridden keyup functionality to support 'selected' input.  Pressing
         * any (not navigation-related) key will clear out the selection */
        keydown : function(e) {

            if (this.$element.hasClass("msuggest-selected")) {
                switch(e.which) {
                    case 9: // tab
                    case 13: //enter
                    case 16: // shift
                    case 17: // ctrl
                    case 18: // alt
                    case 20: //caps
                    case 27: // escape
                    case 37: // left arrow
                    case 38: // up arrow
                    case 39: // right arrow
                    case 40: // down arrow
                    case 91: //command
                       //ignore these keys for a selected input, force selection
                       break;
                    default:
                        this.$element.select();
                        this.$element.removeClass("msuggest-selected");
                        this.$hiddenInput.val("");
                        //otherwise start over with a new search based on the new key
                }
                
            }
            _superproto.keydown.call(this, e);
        },
         keyup: function (e) {
            switch(e.keyCode) {
                case 37: // left arrow
                case 39: // right arrow
                case 91: //command
                    e.stopPropagation();
                    e.preventDefault();
                    break;
                default:
                    _superproto.keyup.call(this, e);
            }
        },
        /* Appends the link text to the bottom of the menu, if specified */
        _buildLink : function() {
            var self = this;
            var link, li, span;
            
            //render the link as the last element
            if (self.options.link) {
                
                //create the link html
                link = $(self.options.link);
                li = $("<li>", {"class" : "disabled"});
                span = $("<span>", {"class" : "msuggest-link"});
                span.append(link);
                li.append(span);
                
                //TODO on select, not click
                link.on("click", function(evt) {
                        evt.stopPropagation();
                        evt.preventDefault();
                        //link onclick handler as provided by consumer
                        self.options.linkHandler.call(this, evt);
                    });
                    
                self.$menu.append(li);
            }
        }
    });
    
    

    /*
     * MultiSuggest Plugin Definition
     */
    
    var old = $.fn.multisuggest;

    $.fn.multisuggest = function(option) {
        return this.each(function() {
            var $this = $(this), data = $this.data('multisuggest'), options = typeof option === 'object' && option;
            if (!data) {
                $this.data('multisuggest', ( data = new MultiSuggest(this, options)));
            }
            if ( typeof option === 'string') {
                data[option]();
            }
        });
    };

    /*
     * Static utilities
     */
    
    $.extend( $.fn.multisuggest, {
        //esc
        escapeRegex : function( value ) {
            return value.replace(/[\-\[\]{}()*+?.,\\\^$|#\s]/g, "\\$&");
        },
        //default filter function
        filter : function(array, term) {
            var matcher = new RegExp( $.fn.multisuggest.escapeRegex(term), "i" );
            return $.grep( array, function(value) {
                return matcher.test( value.label || value.value || value );
            });
        },
        //default highlighter function
        highlighter : function(query, item) {
            if (query === "") {
                return item;
            }
           
            query = $.fn.multisuggest.escapeRegex(query);
            return item.replace(new RegExp('(' + query + ')', 'ig'), 
                      function ($1, match) {
                                return '<strong>' + match + '</strong>';
                       });
          }
    });


    $.fn.multisuggest.defaults = {
        sources : [],
        menu : '<ul class="typeahead dropdown-menu"></ul>',
        item : '<li><a href="#"></a></li>',
        loadingIconClass : 'msuggest-loading',
        noResultsText : 'No search results found.',
        minLength : 1
    };
    
    $.fn.multisuggest.defaults.dataSource = {
        data : [],
        filter : $.fn.multisuggest.filter,
        listFormatter : function(item, query) { return item; },
        inputFormatter : function(item) { return item; },
        valueAttribute : function(item) { return item; },
        header : "",
        queryParam : "term",
        resultsProcessor : function(data) { return data; }
    };

    $.fn.multisuggest.Constructor = MultiSuggest;
    
    

})(window.jQuery);
