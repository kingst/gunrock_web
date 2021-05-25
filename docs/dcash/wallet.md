# The dcash client

The dcash wallet is a C++ program that uses standard in and standard
out for I/O. For the prompt, we'll use `D$` and clients can issue one
of four commands:

| Command | Description                                                   |
|---------|---------------------------------------------------------------|
| auth    | Authenticates the user, saving the auth token for future APIs |
| deposit | Desposits money into the current user's account               |
| send    | Sends money to another user                                   |
| logout  | Deletes the auth token and exits the program                  | 

## Invoking the dcash process

The wallet can be invoked with either no arguments or a single argument;
anything else is an error. Here is the no-argument way:

```bash
prompt> ./dcash
D$ 
```

At this point, dcash is running, and ready to accept commands. Type away!

The mode above is called interactive mode, and allows the user to type
commands directly. The shell also supports a batch mode, which instead
reads input from a batch file and executes commands from therein. Here is
how you run the wallet with a batch file named batch.txt:

```bash
prompt> ./dcash batch.txt
```

One difference between batch and interactive modes: in interactive mode,
a prompt is printed (D$). In batch mode, no prompt should be printed.

## Command and API errors

From the wallet the only error checking we do is to make sure that the
user entered the correct number of arguments. If the user enters the
correct number of arguments then the wallet issues the appropriate API
calls.

If the user supplies the wrong number of arguments or the API calls
result in an error, then the wallet should return a line with the
string "Error". For example:

```bash
D$ auth kingst 123456
Error
D$
```

## The `auth` command

```bash
D$ auth username password email
```

The auth command creates a user with the given password and email if
no users with that username exists, or if the username exists the
command succeeds if and only if they supply the same password that was
used to create the account.

The `auth` command is the only command where the user doesn't need to
be logged in. They will be logged in after a successful `auth` command
and the wallet should reuse the `auth_token` that they get back from
the API server in subsequent API calls.

If the user issues the `auth` command and there is already another
user logged in, then the wallet should delete the already logged in
user's auth token after the new user logs in successfully.

On success, the command should print the user's balance like this:

```bash
D$ auth kingst 123456 kingst@ucdavis.edu
Balance: $0.00
D$
```

_Note:_ The server always operates in cents, so you'll need to convert
the balance to dollars.

## The `deposit` command

```bash
D$ deposit amount_in_dollars credit_card_number exp_year exp_month cvc
```

The deposit command takes in a credit card and charges it, depositing
the specified amount into their account.

On success, the command should print the user's balance like this:

```bash
D$ deposit 20.00 4242424242424242 04 24 242
Balance: $20.00
D$
```

## The `send` command

```bash
D$ send to_username amount_in_dollars
```

The `send` command sends money from the current user's account to
another user's account. To pick a user to send money to, you identify
them by their username.

On success, the command should print the user's balance like this:

```bash
D$ send honey 5.00
Balance: $15.00
D$
```

## The `logout` command

```bash
D$ logout
```

This command deletes the current `auth_token` from the server, and
exits the `dcash` wallet process.
