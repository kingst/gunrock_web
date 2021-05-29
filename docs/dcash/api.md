# APIs

This document lays out the API calls that you will implement as part
of this project. It includes information on
[authentication](#authentication), the [HTTP status codes that you
return on success](#client-success-status-codes), and the [HTTP status
codes that you return on errors](#client-error-status-codes). It also
includes [examples using curl](#an-example-using-curl) for all of our
endpoints while you are testing, before you have your client ready.

To support the `dcash` app, you will implement the following [API
calls](#api-call-specification):
- [POST /auth-tokens](#post-auth-tokens)
- [DELETE /auth-tokens/{auth_token}](#delete-auth-tokensauth_token)
- [PUT /users/{user_id}](#put-usersuser_id)
- [GET /users/{user_id}](#get-usersuser_id)
- [POST /transfers](#post-transfers)
- [POST /deposits](#post-deposits)

## Authentication

All of our API calls except for POSTing to `/auth-tokens` require
authentication. Authentication for our APIs is a two-step process
where first you POST to the `/auth-tokens` endpoint to create a new
account or to authenticate an existing user. The result of this API
call will include an `auth_token` value, which you use on subsequent
API calls to authenticate.

Authenticating using an `auth_token` requires setting the
`x-auth-token` HTTP header to your `auth_token`. Once the server has
the `auth_token` from your client, then it can lookup the user, if one
exists, that this `auth_token` points to and then that user is
authenticated.

## Client success status codes

When a client's API call succeed you use one of two status codes:
`200` or `201`

| Status code | Description                          |
|-------------|--------------------------------------|
| 200         | Success (general)                    |
| 201         | Success _and_ created a new resource |

## Client error status codes

When the client's API call results in an error, you use a `4xx` status
code to let the client know. If you encounter multiple errors on a
single request, pick the error with the higher numeric value as the
higher priority error.

| Status code | Description                                     |
|-------------|-------------------------------------------------|
| 400         | Missing a required argument or arg is malformed |
| 401         | Not logged in but is an API that requires auth  |
| 403         | Is logged in but doesn't have permission        |
| 403         | Incorrect password                              |
| 404         | Resource not found                              |

If the server encounters an error of some sort, it is typical to use a
`5xx` status code, but we'll just use `500` if something unexpected
happens.

_Note:_ For this assignment, make your best judgement call about which
error to use when. When we're grading it we'll check to make sure that
your server returns a 4xx status code but _not_ for a specific one.

## API call specification

For all API calls you pass arguments to the server using
www-form-encoded arguments (see the `WwwFormEncodedDict` class to
create and access these) and all response bodies are JSON objects.

### Errors

For this project, any API calls that result in errors will return an
appropriate status code but omit the message body on the
response. Typical APIs would normally return an error code and error
message in a JSON object, but for this project we're going to keep
things simple.

### Operating on users and their data

In general, for API calls you should only be able to operate on the
data that a particular user owns. For the `/users/*` and `/deposits`
endpoints you should only be able to access data associated directly
with the authenticated account, and you should only be able to delete
`auth_token` objects for the user who is currently logged in.

The one exception to operating exclusively on the current user's data
is when you transfer funds, your transfer API implementation will
increase the balance for another user as a part of your implementation.

### POST `/auth-tokens`

If the username doesn't exist this call will create a new user, and if
it does exist then it logs in the user if the password matches.

_Note:_ This API call is the only API call where the user does _not_
need to be authenticated.

#### Arguments

| Argument | Description                                       |
|----------|---------------------------------------------------|
| username | The username, must contain only lowercase letters |
| password | The password                                      |

_Note:_ Clients will pass their passwords without encryption directly
to our server. We don't need to worry about any sort of obfuscation
for this assignment just checking that the passwords match is
sufficient.

#### Response object

| Key        | Description                          |
|------------|--------------------------------------|
| auth_token | (String) The `auth_token`            |
| user_id    | (String) The `user_id` for this user |

_Note:_ The `auth_token` is temporary and represents an authenticated
session, the `user_id` is stable and always stays the same for a user.

### DELETE `/auth-tokens/{auth_token}`

Deletes the auth token, used to logout a user. The user can delete any
`auth_token` for their account, even if they authenticate with a
different `auth_token`.

#### Arguments

No arguments.

#### Response object

No response object.

### PUT `/users/{user_id}`

Updates the information for a user.

#### Arguments

| Argumemt | Description                    |
| ---------|--------------------------------|
| email    | The email address for the user |

#### Response object

| Key     | Description                               |
|---------| ------------------------------------------|
| email   | (String) The email address for this user  |
| balance | (Int) The balance on the account in cents |

### GET `/users/{user_id}`

Fetches the user object for this account. You can only fetch the user
object for a the account that you authenticated.

#### Arguments

No arguments.

#### Response object

| Key     | Description                               |
|---------| ------------------------------------------|
| email   | (String) The email address for this user  |
| balance | (Int) The balance on the account in cents |

### POST `/transfers`

Transfers money from your account to another user. You must make sure
that there aren't any negative balances and that the `to` username
exists for this API call to succeed.

#### Arguments

| Argument | Description                     |
|----------|---------------------------------|
| to       | The username of the recipient   |
| amount   | The amount to transfer in cents |

#### Response object

| Key       | Description                                   |
|-----------|-----------------------------------------------|
| balance   | (Int) The new balance on the account in cents |
| transfers | (Array) A transfer history for this user      |

#### Transfer object

Your response will include an array of transfer object, this is what
is in each transfer object:

| Key    | Description                                             |
|--------|---------------------------------------------------------|
| from   | (String) the username of the user who sent the transfer |
| to     | (String) the username of the user who recv the transfer |
| amount | (Int) The amount of the transfer in cents               |

### POST `/deposits`

Uses a credit card to deposit money into your account. For testing
before you have your client working you can use [Stripe testing
tokens](https://stripe.com/docs/testing#cards), (e.g., `tok_visa` and
`tok_chargeDeclined`).

#### Arguments

| Argument     | Description                                      |
|--------------|--------------------------------------------------|
| amount       | The amount to charge, in cents (must be >= 50)   |
| stripe_token | The Stripe token for the card you want to charge |

#### Response object

| Key      | Description                                     |
|----------|-------------------------------------------------|
| balance  | (Int) The new balance on the account in cents   |
| deposits | (Array) All of the deposit object for this user |

#### Deposit object

Your response includes a history of all deposits for this user, each
element in the array is `deposit` object:

| Key              | Description                                      |
|------------------|--------------------------------------------------|
| to               | (String) the username for the user who deposited |
| amount           | (Int) the amount of the deposit in cents         |
| stripe_charge_id | (String) the Stripe charge ID for the deposit    |

_Note:_ To service this request you'll need to send an API request to
Stripe. You should block this API call while you wait for a response
from Stripe.

## An example using curl

```bash
$ curl http://localhost:8080/auth-tokens -d username=kingst -d password=123456
{
    "auth_token": "2-fobvOk3LXY7v_6NYeuDnTu",
    "user_id": "a2zFGP53ZHk5uW6IjEh9Qxep"
}
$ curl http://localhost:8080/auth-tokens -d username=honey -d password=123456
{
    "auth_token": "OZ0zdN5xbX5feMXQVSVR1tTU",
    "user_id": "zxlmBCTO8Ye4eL52w_mUtGI-"
}
$ curl -X PUT -H 'x-auth-token: 2-fobvOk3LXY7v_6NYeuDnTu' http://localhost:8080/users/a2zFGP53ZHk5uW6IjEh9Qxep -d email=kingst%40ucdavis.edu
{
    "balance": 0,
    "email": "kingst@ucdavis.edu"
}
$ curl -X PUT -H 'x-auth-token: OZ0zdN5xbX5feMXQVSVR1tTU' http://localhost:8080/users/zxlmBCTO8Ye4eL52w_mUtGI- -d email=honey%40ucdavis.edu
{
    "balance": 0,
    "email": "honey@ucdavis.edu"
}
$ curl -H 'x-auth-token: 2-fobvOk3LXY7v_6NYeuDnTu' http://localhost:8080/deposits -d amount=2000 -d stripe_token=tok_visa
{
    "balance": 2000,
    "deposits": [
        {
            "to": "kingst",
            "amount": 2000,
            "stripe_charge_id": "ch_1IuQh22eZvKYlo2CB4kY96YU"
        }
    ]
}
$ curl -H 'x-auth-token: 2-fobvOk3LXY7v_6NYeuDnTu' http://localhost:8080/transfers -d amount=500 -d to=honey
{
    "balance": 1500,
    "transfers": [
        {
            "from": "kingst",
            "to": "honey",
            "amount": 500
        }
    ]
}
$ curl -H 'x-auth-token: OZ0zdN5xbX5feMXQVSVR1tTU' http://localhost:8080/users/zxlmBCTO8Ye4eL52w_mUtGI-
{
    "balance": 500,
    "email": "honey@ucdavis.edu"
}
$ curl -X DELETE -H 'x-auth-token: OZ0zdN5xbX5feMXQVSVR1tTU' http://localhost:8080/auth-tokens/OZ0zdN5xbX5feMXQVSVR1tTU
```
