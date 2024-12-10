from aiohttp import web
from aiohttp_middlewares.cors import cors_middleware


async def hello(request: web.Request) -> web.Response:
    return web.HTTPOk()


app = web.Application(middlewares=[cors_middleware(allow_all=True)])

app.router.add_route("GET", "/hello", hello)

web.run_app(app, host="localhost", port=5000)
