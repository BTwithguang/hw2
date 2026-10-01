#include "mydatastore.h"
#include "util.h"

MyDataStore::MyDataStore()
{
}

MyDataStore::~MyDataStore()
{
    for(std::vector<Product*>::iterator it = products_.begin(); it != products_.end(); ++it) {
        delete *it;
    }
    for(std::vector<User*>::iterator it = users_.begin(); it != users_.end(); ++it) {
        delete *it;
    }
}
void MyDataStore::addProduct(Product* p)
{
    products_.push_back(p);
    std::set<std::string> words = p->keywords();
    for(std::set<std::string>::iterator it = words.begin(); it != words.end(); ++it) {
        productsByKeyword_[convToLower(*it)].insert(p);
    }
}
void MyDataStore::addUser(User* u)
{
    users_.push_back(u);
    usersByName_[convToLower(u->getName())] = u;
}
std::vector<Product*> MyDataStore::search(std::vector<std::string>& terms, int type)
{
    std::set<Product*> matches;
    for(std::vector<std::string>::iterator it = terms.begin(); it != terms.end(); ++it) {
        std::string word = convToLower(*it);
        std::map<std::string, std::set<Product*> >::iterator found = productsByKeyword_.find(word);
        std::set<Product*> empty;
        std::set<Product*>& current = found == productsByKeyword_.end() ? empty : found->second;
        if(type == 0) {
            if(it == terms.begin()) {
                matches = current;
            }
            else {
                matches = setIntersection(matches, current);
            }
            if(matches.empty()) {
                break;
            }
        }
        else {
            matches = setUnion(matches, current);
        }
    }
    return std::vector<Product*>(matches.begin(), matches.end());
}
void MyDataStore::dump(std::ostream& ofile)
{
    ofile << "<products>\n";
    for(std::vector<Product*>::iterator it = products_.begin(); it != products_.end(); ++it) {
        (*it)->dump(ofile);
    }
    ofile << "</products>\n<users>\n";
    for(std::vector<User*>::iterator it = users_.begin(); it != users_.end(); ++it) {
        (*it)->dump(ofile);
    }
    ofile << "</users>\n";
}
User* MyDataStore::findUser(const std::string& username) const
{
    std::map<std::string, User*>::const_iterator it = usersByName_.find(convToLower(username));
    return it == usersByName_.end() ? NULL : it->second;
}

bool MyDataStore::addToCart(const std::string& username, Product* product)
{
    User* user = findUser(username);
    if(user == NULL) {
        return false;
    }
    carts_[convToLower(user->getName())].push_back(product);
    return true;
}
bool MyDataStore::viewCart(const std::string& username, std::ostream& os) const
{
    User* user = findUser(username);
    if(user == NULL) {
        return false;
    }
    std::map<std::string, std::deque<Product*> >::const_iterator it = carts_.find(convToLower(user->getName()));
    if(it == carts_.end()) {
        return true;
    }
    for(std::size_t i = 0; i < it->second.size(); ++i) {
        os << "Item " << i + 1 << "\n" << it->second[i]->displayString() << "\n\n";
    }
    return true;
}
bool MyDataStore::buyCart(const std::string& username)
{
    User* user = findUser(username);
    if(user == NULL) {
        return false;
    }
    std::deque<Product*>& cart = carts_[convToLower(user->getName())];
    std::deque<Product*> remaining;
    for(std::deque<Product*>::iterator it = cart.begin(); it != cart.end(); ++it) {
        Product* product = *it;
        if(product->getQty() > 0 && user->getBalance() >= product->getPrice()) {
            product->subtractQty(1);
            user->deductAmount(product->getPrice());
        }
        else {
            remaining.push_back(product);
        }
    }
    cart.swap(remaining);
    return true;
}
