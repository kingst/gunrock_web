# The dcash wallet

For this project you will write a command-line wallet, called `dcash`
for sending money to people. The main components of this project are
(1) command-line wallet application for interacting directly with
users, (2) a API server, based on [Gunrock
Web](https://github.com/kingst/gunrock_web), for supporting your
wallet, and (3) API calls to Stripe for charging credit cards.

In a modern distributed app, app designers often use a combination of
client code and server code. Examples of client code include iOS and
Android apps, and Javascript that runs in a browser. The key
distinguishing feature of client code is that it runs on a computer
system -- the user's phone, laptop, or desktop -- that the app
designers have no control over. App designers must assume that these
client machines are untrusted, and therefore focus client code of
user-facing aspects of an app, like input and display.

Server code runs in server systems that the app designers _do_ have
control over. For modern apps, this server infrastructure typically
comes in the form of cloud computing, and the app designers can run
their software in the cloud with higher assurance that the
hardware/software stack supporing their software can be
trusted. Therefore, app designers use servers to handle sensitive
computation, like storing the full database of users or any financial
transactions are managed by server software.

APIs are the glue that connects client and server code. Conceptually,
you can view APIs like remote function calls that the client
application makes. These function calls can access the state of the
app and initiate state changes. Some apps, like Twitter, even open up
their APIs to third parties so anyone can write client software that
uses their app, and other companies, like Twilio and Stripe, provide
APIs as a service as their main business.

In the source code that we provide you we took some shortcuts, but the
bones are all there for you to be able to build a realistic
distributed application.

## Part 1: The API server

The main role of the API server in this app is to keep a database of
users and their balances, enable users to transfer cash to each other,
and issue credit card charges using Stripe to deposit money into a
user's account.

## Part 2: Interacting with Stripe

## Part 3: The dcash wallet