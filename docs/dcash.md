# The dcash wallet

For this project you will write a command-line wallet, called `dcash`
for sending money to people. The main components of this project are
(1) command-line wallet application for interacting directly with
users, (2) a API server, based on [Gunrock
Web](https://github.com/kingst/gunrock_web), for supporting your
wallet, and (3) API calls to Stripe for charging credit cards.

In the source code that we provide you we took some shortcuts, but the
bones are all there for you to be able to build a realistic
distributed application.

## Part 1: The API server

The main role of the API server in this app is to keep a database of
users and their balances, enable users to transfer cash to each other,
and issue credit card charges using Stripe to deposit money into a
user's account.