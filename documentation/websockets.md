# WebSockets

A WebSocket is a full-duplex communication channel over a single long lived
TCP connection. It is a protocol and starts with a standard HTTP request.

## What to do
- handle websocket request
- create GUID and append to client's key
- SHA-1 hash it
- base64 encode it (the accept key)
- set client socket to non blocking mode
- send response
- handle pings and pongs
- close connection or respond to close connection

## Gotchas
- the websocket http request may not come in in a single read()
- check request details
- the data frames must be continuously read
- the frame's head must be parsed and the payload unmasked
- use select, poll, epoll for async, because the websocket is blocking

## WebSocket Connection
- Handshake (HTTP Upgrade)
- Data Transfer (Framing)

## WebSocket GET request example
```http

GET /chat HTTP/1.1
Host: example.com:8080
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==
Sec-WebSocket-Version: 13
Origin: http://example.com

```

## WebSocket response example
```http

HTTP/1.1 101 Switching Protocols
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=

```

## Steps

### Handshake
- the client sends an upgrade request
- the server appends a GUID string to their Sec-WebSocket-Key, computes the
    SHA-1 hash of the combined string, and then base64-encodes the hash. Then
    the server responds with this information and the Handshake is complete.

### Data Transfer (Framing)
- data is no longer sent as HTTP requests/responses, but as small units called
    frames.
- each frame contains: Header Information and Payload Data

## Opcode in frame header
- 0x1: Text frame (UTF-8 encoded)
- 0x2: Binary frame
- 0x8: Connection close frame
- 0x9: Ping frame (used for keep-alives)
- 0xA: Pong frame (response to a ping)
- 0x0: Continuation frame (for fragmented messages)

## Masking
- all client frames are masked with a 32-bit masking key
- the server does not mask frames

## Ping/Pong Frames
- the server and client should send and respond to pings to keep the connection
    alive
- the server and client both send close frames to indicate their intention to
    close the connection

## WebSocket Urls
- ws://
- wss://
