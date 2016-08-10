/* Copyright (c) 2015-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Nicolas Antille <nicolas.antille@epfl.ch>
 *                     Olivier Amblet <olivier.amblet@epfl.ch>
 *                     Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

(function() {
  'use strict';

  // Global environment config to connect to main HBP services
  /* eslint angular/window-service:0 */
  window.bbpConfig = {
    "api": {
      "collab": {
        "v0": "https://services.humanbrainproject.eu/collab/v0"
      },
      "document": {
        "v0": "https://services.humanbrainproject.eu/document/v0/api"
      },
      "mimetype": {
        "v0": "https://services.humanbrainproject.eu/mimetype/v0"
      },
      "stream": {
        "v0": "https://services.humanbrainproject.eu/stream/v0/api"
      },
      "user": {
        "v1": "https://services.humanbrainproject.eu/idm/v1/api"
      }
    },
    "auth": {
      "clientId": "9538e663-d4a3-491f-a45e-103dbcb3d3f5",
      "url": "https://services.humanbrainproject.eu/oidc",
      "ensureToken": false
    },
    "collab": {
      "features": {
        "identity": {
          "userApiV1": true
        }
      }
    },
    "hbpFileStore": {
      "maxFileUploadSize": 1048576000
    }
  };

  angular
    .module('visualisationUi')
    .config(config);

  /** @ngInject */
  function config($logProvider) {
    // Enable log
    $logProvider.debugEnabled(true);
  }

})();
