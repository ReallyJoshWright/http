# HTTP Documentation

Client-Server Model that operates on a request-response paradigm.
HTTP listens on port 80 and HTTPS (HTTP over TLS/SSL) listens on port 443.
It is a stateless model. State can be controlled with cookies and sessions.

Start Line
HTTP Headers
Empty Line
Message Body

## HTTP Request Message

### Request Line
- Method (GET, POST, PUT, DELETE)
- URI (Uniform Resource Identifier, /index.html, /api/users)
- HTTP Version (HTTP/1.1)
- Example: GET /index.html HTTP/1.1

### Headers
- Host: www.example.com
- User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64)
- Accept: text/html, application/xhtml+xml
- Content-Type: application/json (if a body is present)
- Content-Length: 123 (if a body is present)

### Message Body
can be json data, xml data, form data

## HTTP Response Message
optional

### Status Line
- HTTP Version (HTTP/1.1)
- Status Code (3-digit number, 200, 404)
- Reason Phrase (short, human-readable description of the status code)
- Example: HTTP/1.1 200 OK

### Headers
- Content-Type: text/html
- Content-Length: 5000
- Server: Apache/2.4.41
- Date: Fri, 26 2025 18:00:00 GMT
- Set-Cookies: sessionid=xyz; HttpOnly

### Message Body
optional
can be HTML page, JSON data, image bytes

## HTTP Methods (verbs)
- GET: a data request like a sql select statement
- POST: like INSERT in SQL, it submits an entity to a specified resource
- PUT: replaces all current representations of the target resource like a sql
    update statement
- DELETE: like DELETE in SQL, deletes a resource
- HEAD: a GET request with the response body
- OPTIONS: describes the communication options for the target resource
- PATCH: applies partial modifications to a resource, like a sql update
    statement

## Status Codes
- 1xx (Informational, 100 Continue)
- 2xx (Success, 200 OK, 201 Created, 204 No Content)
- 3xx (Redirection, 301 Moved Permanently, 302 Found, 304 Not Modified)
- 4xx (Client Error, 400 Bad Request, 401 Unauthorized, 403 Forbidden,
    404 Not Found)
- 5xx (Server Error, 500 Internal Server Error, 502 Bad Gateway,
    503 Service Unavailable)
