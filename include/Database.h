#ifndef _DATABASE_H_
#define _DATABASE_H_

#include <string>
#include <map>
#include <vector>

class User {
 public:
  std::string email;
  std::string username;
  std::string password;
  std::string user_id;
  int balance;
};

class Transfer {
 public:
  User *from;
  User *to;
  int amount;
};

class Deposit {
 public:
  User *to;
  int amount;
  std::string stripe_charge_id;
};

// Note: you must use a single User object and hold pointers to it
// in the `users` and `auth_tokens` databases
class Database {
 public:
  // the key is the username
  std::map<std::string, User *> users;
  // the key is the auth_token
  std::map<std::string, User *> auth_tokens;
  // A vector of all transfers for all users
  std::vector<Transfer *> transfers;
  // A vector of all deposits for all users
  std::vector<Deposit *> deposits;

  // set by config.json
  std::string stripe_secret_key;
};

#endif
