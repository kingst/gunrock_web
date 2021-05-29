# Working with Stripe APIs

In this project, you will work with Stripe APIs. There are two basic
API calls that you will make:
[Tokens](https://stripe.com/docs/api/tokens/create_card) API to
convert a credit card number, expiration, and CVV into a token, the
[Charges](https://stripe.com/docs/api/charges/create) API to charge
the tokenized card.

In general, Stripe has extensive
[documentation](https://stripe.com/docs/api), which you are encouraged
to read through to understand payments APIs in more detail.

To get started, create an account on
[Stripe](https://dashboard.stripe.com/register) and you'll be able to
get test credentials that you can use for this project. The process of
accepting payments from real cards requires a bit more setup, but you
can skip this part for this project as we'll use test credentials for
grading.

## Passing arguments to Stripe

To pass arguments to Stripe API calls, you must _form encode_ all of
your values and send them in the HTTP Request body. We have provided a
`WwwFormEncodedDict` class to help simplify this process.

Here's an example:

```c++
WwwFormEncodedDict body;
body.set("amount", amount);
body.set("foo", "bar");
string encoded_body = body.encode();
```

Once you have an encoded body, you can set the body of your HTTP
request to this value and Stripe's APIs will be able to decode them
correctly.

To make API requests to Stripe, you will use the `HttpClient` class
that we provide. See the header file for this in
`shared/include/HttpClient.h` for more details.

## Authentication

Stripe's API calls rely on authentication to process requests. You
will use your `publishable key` from the client to perform
tokenization and your `secret key` server side to charge the card.

Both of these keys should be in your `config.json` file for your API
server and your wallet. Please see the docs for both of these
applications for more details.

### Authenticating from the `dcash` wallet

To authenticate to Stripe from the wallet, you use your `publishable
key`:

```c++
// from the dcash wallet to Stripe
HttpClient client("api.stripe.com", 443, true);
client.set_header("Authorization", string("Bearer ") + PUBLISHABLE_KEY);
```

And with this type of authentication you can tokenize cards from the
`dcash` wallet.

### Authentication from the `gunrock` server

To authenticate to Stripe from your server, you use your `secret
key`. Stripe uses HTTP basic authentication with your secret key as
the username and the password field blank, but we've abstracted away
the implementation details of basic authentication:

```c++
// from the gunrock server to Stripe
HttpClient client("api.stripe.com", 443, true);
client.set_basic_auth(m_db->stripe_secret_key, "");
```

## Interpreting results

Stripe's API calls return JSON objects, and in order to extract the
`charge_id` and the credit card `token`, you'll need to be able to
parse an interpret the results from the server. To help simplify this
process we have included
[rapidjson](https://rapidjson.org/md_doc_tutorial.html) in the
gunrock_web repo. Using `rapidjson` in C++ isn't as easy as using json
in other programming languages, but it is well documented, fast, and
well supported.

```c++
HttpClient client("api.stripe.com", 443, true);
// ...
HTTPClientResponse *client_response = client.post("/v1/charges",
                                                  body);
// This method converts the HTTP body into a rapidjson document
Document *d = client_response->jsonBody();
string value = (*d)["foo"].GetString();
delete d;
```

There are plenty of tutorials and docs online for `rapidjson`, use
these resources as needed.

## Tokenization

The purpose of credit card tokenization is so that your servers never
need to deal with credit card details directly. Instead, your client,
which is running on one of your user's devices, can call Stripe APIs
directly to tokenize card details and using that token you can charge
the card from your server. The basic flow involves creating the token
in the `dcash` wallet and then passing that token to your `gunrock`
server where it can make the charge.

By separating the responsibility of tokenizing and charging a card,
your app can rely on the client to tokenize while still using your
server to set the amount appropriately.

The `dcash` wallet is your client program in this project. From the
`dcash` application, you will capture card details from your user and
then issue a `Token` API call on Stripe to convert these card details
into a token. Once you have a token, you can use that token to charge
the card server side.

The `gunrock` server is where you issue the actual charge using the
`Charges` APIs.
