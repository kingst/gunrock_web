# API server

The API server is a wrapper around the core database where we store
the state of the app, and it's the intermediary where we initiate
credit card charges on Stripe.

## Using the `config.json` file

To configure your API server, you should use a configuration json file
to specify your Stripe secret key. You do _not_ need to include this
file in your submission.

```javascript
{
    "stripe_secret_key": "sk_test_1234"
}
```

We have included some boilerplate code that uses `rapidjson` to parse
this file and set a few variable.

## Services

From an architectural perspective, the `gunrock` server enables you
establish routes and handlers for these routes. Routes are HTTP path
prefixes, that when a request matches, invokes the service that
registered with that prefix. Services inhert from the base
`HttpService` and implement whichever HTTP methods they need to handle
(e.g., GET or PUT) to implement that functionality. Our goal with
`gunrock` is to make handling API calls as easy as implementing a
function.

When `gunrock` invokes one of your service's HTTP method handlers, it
passes in two arguments: a `HTTPRequest` object and a `HTTPResponse`
object. The `HTTPRequest` is a C++ class that includes the parsed HTTP
request data, including the headers and message body. The
`HTTPResponse` object is initialized with an empty body and with a
status code of 200. So if your handler doesn't do anything, it will
result in a 200 status code. As you parse the arguments and process
the request, you can adjust the response object appropriately. The
most common operations you'll do on a successful HTTP request are
setting the response object's body and content type and using
`rapidjson` to create a JSON object that you serialize to a string in
and return in the body:

```c++
// use rapidjson to create a return object
Document document;
Document::AllocatorType& a = document.GetAllocator();
Value o;
o.SetObject();

// add a key value pair directly to the object
o.AddMember("key1", "value1", a);

// create an array
Value array;
array.SetArray();

// add an object to our array
Value to;
to.SetObject();
to.AddMember("key2", "value2", a);
array.PushBack(to, a);

// and add the array to our return object
o.AddMember("array_key", array, a);

// now some rapidjson boilerplate for converting the JSON object to a string
document.Swap(o);
StringBuffer buffer;
PrettyWriter<StringBuffer> writer(buffer);
document.Accept(writer);

// set the return object
response->setContentType("application/json");
response->setBody(buffer.GetString() + string("\n"));
```

Whenever your code encounters a client error (e.g., an API call that
accesses an authenticated method without including an `auth_token` for
authentication) you signify to `gunrock` that there was an error by
throwing one of the `ClientError.h` exceptions. The `gunrock`
framework will catch these exceptions and craft the return object
appripriately to convey the error to the client that invoked the API
call.

```c++
if (!authenticated) {
   throw ClientError::unauthorized();
}
```

The four services that you're responsible for implementing are:
`AccountService`, `AuthService`, `DepositService`, and
`TransferService`. Each of these services are responsible for
different portions of the HTTP path namespace. The [API
specification](api.md) outlines the details of each of the methods
that you're responsible for implementing.

For these services, you must implement the base-class method `User *HttpService::getAuthenticatedUser(HTTPRequest *)`
and use this method for all of your API calls that require authentication.

## Reading request arguments

To read request arguments, we recommend using the `formEncodedBody`
method on the `HTTPRequest` object that you get with each method
handler. This method returns a `WwwFormEncodedDict` object that
parses form-encoded bodies and provides access to the keys and
values that it parses.

## Helpful utilities

Some helpful utilities include:
- `HttpClient`: a class for making HTTP requests
- `StringUtils::createUserId()`: A function for creating unique user IDs
- `StringUtils::createAuthToken()`: A function for creating unique auth tokens

## Database support

To support data storage, all services include a pointer to a
`Database` object (see `HttpService::m_db` for more details). This
`Database` object contains four tables for you to use in your
implementation: `users`, `auth_tokens`, `transfers`, and `deposits`.

To keep this project simple, we only use an in-memory database, so all
of your data goes away after you exit the `gunrock` server.

## Threads

Only use one thread for both your wallet and API server
implementations.
