#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (c) 2016-2018, Blue Brain Project
#                          Raphael Dumusc <raphael.dumusc@epfl.ch>
#                          Daniel Nachbaur <daniel.nachbaur@epfl.ch>
#                          Cyrille Favreau <cyrille.favreau@epfl.ch>
#
# This file is part of Brayns <https://github.com/BlueBrain/Brayns>
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License version 3.0 as published
# by the Free Software Foundation.
#
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
# All rights reserved. Do not distribute without further notice.

"""Client that connects to a remote running Brayns instance which provides the supported API."""

import asyncio
import inspect
import rockets

from .api_generator import build_api
from .utils import base64decode, set_http_protocol, underscorize
from .utils import HTTP_METHOD_GET, HTTP_STATUS_OK
from .version import MINIMAL_VERSION
from . import utils


class BaseClient:
    """Client that connects to a remote running Brayns instance which provides the supported API."""

    def __init__(self, url):
        """
        Create a new client instance by connecting to the given URL.

        :param str url: a string 'hostname:port' to connect to a running Brayns instance
        """
        self.http_url = set_http_protocol(url) + '/'
        """The HTTP URL"""

        self._check_version()
        self.rockets_client = None

        if utils.in_notebook():
            self._add_widgets()  # pragma: no cover

    # pylint: disable=E1101
    def __str__(self):
        """Return a pretty-print on the currently connected Brayns instance."""
        version = 'unknown'
        if self.version:
            version = '.'.join(str(x) for x in [self.version.major, self.version.minor,
                                                self.version.patch])
        return "Brayns version {0} running on {1}".format(version, self.http_url)

    def set_colormap(self, palette, intensity=1, opacity=1, data_range=(0, 255)):
        """
        Set a colormap to Brayns.

        :param list palette: color palette to use
        :param int intensity: value to amplify the color values
        :param int opacity: opacity for colormap values
        :param typle data_range: data range on which values the colormap should be applied
        """
        palette_size = len(palette)
        contributions = []
        diffuses = []
        # pylint: disable=E1101
        tf = self.transfer_function
        for i in range(0, palette_size):
            color = palette[i]
            diffuses.append([intensity * color[0], intensity * color[1], intensity * color[2],
                             opacity])
            contributions.append(0)
        tf.diffuse = diffuses
        tf.contribution = contributions
        tf.range = data_range
        tf.commit()

    def open_ui(self):
        """Open the Brayns UI in a new page of the default system browser."""
        import webbrowser
        url = 'https://bbp-brayns.epfl.ch?host=' + self.http_url
        webbrowser.open(url)

    def _check_version(self):
        """Check if the Brayns' version is sufficient enough."""
        status = utils.http_request(HTTP_METHOD_GET, self.http_url, 'version')
        if status.code != HTTP_STATUS_OK:
            raise Exception('Cannot obtain version from Brayns')

        version = '.'.join(str(x) for x in [status.contents['major'], status.contents['minor'],
                                            status.contents['patch']])

        import semver
        if semver.match(version, '<{0}'.format(MINIMAL_VERSION)):
            raise Exception('Brayns does not satisfy minimal required version; '
                            'needed {0}, got {1}'.format(MINIMAL_VERSION, version))

    def _build_api(self, registry, requests, schemas):
        """Use the schemas from the remote running Brayns to build the API."""
        schemas_dict = dict()
        for request in requests:
            # pylint: disable=protected-access,cell-var-from-loop
            response = list(filter(lambda x, request_id=request.request_id(): x._id == request_id,
                                   schemas))
            # pylint: enable=protected-access,cell-var-from-loop
            schemas_dict[request.params['endpoint']] = response[0].result
        build_api(self, registry, schemas_dict)

        self._setup_notifications()

    def _setup_notifications(self):
        def _notification_filter(data):
            return data.method.startswith('set-')

        def _on_notification(data):
            # remove the 'set-' part of the data method to find the property to update its content
            prop_name = underscorize(data.method[4:])
            prop = getattr(self, '_' + prop_name, None)
            if prop:
                prop.update(data.params)

        self.rockets_client.notifications.filter(_notification_filter).subscribe(_on_notification)

    def _add_widgets(self):  # pragma: no cover
        """Add functions to the Brayns object to provide widgets for appropriate properties."""
        self._add_show_function()
        self._add_animation_slider()

    def _add_show_function(self):  # pragma: no cover
        """Add show() function for live streaming."""
        def function_builder():
            """Wrapper for returning the show() function."""
            def show():
                """Show the live rendering of Brayns."""
                from IPython.display import display
                import ipywidgets as widgets
                from rx import Observer

                rockets_client = self.rockets_client
                brayns = self

                class ImageStreamObserver(Observer):
                    """Update a Image widget with the JPEG stream from Brayns."""

                    def __init__(self):
                        self.image = widgets.Image(format='jpg')

                        def _show_image(value):
                            self.image.value = base64decode(value['data'])
                            viewport = brayns.application_parameters.viewport
                            self.image.width = viewport[0]
                            self.image.height = viewport[1]
                            display(self.image)

                        if isinstance(rockets_client, rockets.AsyncClient):
                            task = rockets_client.async_request('image-jpeg')
                            task.add_done_callback(lambda x: _show_image(x.result()))
                        else:
                            _show_image(rockets_client.request('image-jpeg'))

                    def on_next(self, value):
                        viewport = brayns.application_parameters.viewport
                        self.image.width = viewport[0]
                        self.image.height = viewport[1]
                        self.image.value = value

                    def on_completed(self):
                        self.image.close()

                    def on_error(self, error):
                        print("Error occurred: {0}".format(error))
                        self.image.close()

                rockets_client.ws_observable \
                    .filter(lambda value: isinstance(value, (bytes, bytearray, memoryview))) \
                    .subscribe(ImageStreamObserver())

            return show

        setattr(self, 'show', function_builder())

    def _add_animation_slider(self):  # pragma: no cover
        """Add animation_slider() function for animation_parameters control."""
        def function_builder():
            """Wrapper for returning the animation_slider() function."""
            def animation_slider():
                """Show slider to control animation."""
                from IPython.display import display
                import ipywidgets as widgets
                from rx import Observer

                brayns = self

                class AnimationObserver(Observer):
                    """Update a animation slider from 'set-animation-parameters' notifications."""

                    def __init__(self):
                        # pylint: disable=E1101
                        anim_params = brayns.animation_parameters
                        self.button = widgets.ToggleButton(value=anim_params.delta != 0,
                                                           icon='play' if anim_params.delta == 0
                                                           else 'pause')
                        self.button.layout = widgets.Layout(width='40px')

                        def _on_button_change(change):
                            self.button.icon = 'pause' if change['new'] else 'play'
                            result = brayns.set_animation_parameters(delta=1 if change['new']
                                                                     else 0)
                            if inspect.iscoroutine(result):
                                asyncio.ensure_future(result)

                        self.button.observe(_on_button_change, names='value')

                        self.slider = widgets.IntSlider(min=anim_params.start,
                                                        max=anim_params.end,
                                                        value=anim_params.current)

                        def _on_value_change(change):
                            result = brayns.set_animation_parameters(current=change['new'])
                            if inspect.iscoroutine(result):
                                asyncio.ensure_future(result)

                        self.value_change = _on_value_change

                        self.slider.observe(self.value_change, names='value')

                        self.box = widgets.HBox([self.button, self.slider])
                        display(self.box)

                    def on_next(self, value):
                        self.slider.unobserve(self.value_change, names='value')
                        params = value.params
                        self.slider.min = params['start']
                        self.slider.max = params['end']
                        self.slider.value = params['current']
                        self.slider.observe(self.value_change, names='value')

                    def on_completed(self):
                        self.box.close()

                    def on_error(self, error):
                        print("Error occurred: {0}".format(error))
                        self.box.close()

                def _animation_filter(value):
                    return value.method == 'set-animation-parameters'
                self.rockets_client.notifications.filter(_animation_filter) \
                    .subscribe(AnimationObserver())

            return animation_slider

        setattr(self, 'animation_slider', function_builder())
