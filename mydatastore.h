#ifndef MYDATASTORE_H
#define MYDATASTORE_H
#include <deque>
#include <map>
#include <set>
#include <vector>
#include <string>
#include "datastore.h"
class MyDataStore : public DataStore{
  public:
  MyDataStore();
  virtual ~MyDataStore();
  virtual void addProduct(Product* p);
  virtual void addUser(User* u);
  virtual std::vector<Product*> search(std::vector<std::string>&terms, int type);
  virtual void dump(std::ostream& ofile);
  bool addToCart(const std::string& username, Product* product);
  bool viewCart(const std::string& username, std::ostream& os) const;
  bool buyCart(const std::string& username);

  private:
  User* findUser(const std::string& username) const;
  std::vector<Product*> products_;
  std::vector<User*> users_;
  std::map<std::string, User*> usersByName_;
  std::map<std::string, std::set<Product*>> productsByKeyword_;
  std::map<std::string, std::deque<Product*>> carts_;
};

#endif
