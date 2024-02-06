# Copyright (c) 2015-2024 EPFL/Blue Brain Project
# All rights reserved. Do not distribute without permission.
#
# Responsible Author: adrien.fleury@epfl.ch
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

from __future__ import annotations

from pathlib import Path

import brayns
from testapi.api_test_case import ApiTestCase


class TestConnector(ApiTestCase):
    @property
    def uri(self) -> str:
        return f"localhost:{self.port}"

    @property
    def key(self) -> Path:
        return self.asset_folder / "key.pem"

    @property
    def certificate(self) -> Path:
        return self.asset_folder / "certificate.pem"

    def test_connect(self) -> None:
        with self._start_service():
            with self._connect():
                pass

    def test_connect_secure(self) -> None:
        with self._start_service(secure=True):
            with self._connect(secure=True, cafile=str(self.certificate)):
                pass

    def test_connect_no_instance(self) -> None:
        with self.assertRaises(brayns.ServiceUnavailableError):
            self._connect(max_attempts=5)

    def test_connect_invalid_server_certificate(self) -> None:
        with self._start_service(secure=True):
            with self.assertRaises(brayns.InvalidServerCertificateError):
                with self._connect(secure=True):
                    pass

    def test_multiple_clients(self) -> None:
        with self._start_service():
            instances = [self._connect() for _ in range(3)]
            with self.assertRaises(brayns.ServiceUnavailableError):
                self._connect(max_attempts=1)
            for instance in instances:
                instance.disconnect()

    def _start_service(self, secure: bool = False) -> brayns.Process:
        service = brayns.Service(
            uri=self.uri,
            max_clients=3,
            ssl_context=brayns.SslServerContext(
                private_key_file=str(self.key),
                private_key_passphrase="test",
                certificate_file=str(self.certificate),
                ca_location=str(self.certificate),
            )
            if secure
            else None,
            executable=self.executable,
            env=self.env,
        )
        return service.start()

    def _connect(
        self,
        secure: bool = False,
        max_attempts: int | None = None,
        cafile: str | None = None,
    ) -> brayns.Instance:
        connector = brayns.Connector(
            uri=self.uri,
            ssl_context=brayns.SslClientContext(cafile=cafile) if secure else None,
            max_attempts=max_attempts,
        )
        return connector.connect()
